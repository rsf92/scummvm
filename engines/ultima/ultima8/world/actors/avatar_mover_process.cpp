/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "ultima/ultima8/misc/pent_include.h"
#include "ultima/ultima8/world/actors/avatar_mover_process.h"
#include "ultima/ultima8/world/actors/animation.h"
#include "ultima/ultima8/ultima8.h"
#include "ultima/ultima8/world/actors/main_actor.h"
#include "ultima/ultima8/gumps/game_map_gump.h"
#include "ultima/ultima8/kernel/kernel.h"
#include "ultima/ultima8/world/actors/actor_anim_process.h"
#include "ultima/ultima8/world/actors/targeted_anim_process.h"
#include "ultima/ultima8/world/actors/avatar_gravity_process.h"
#include "ultima/ultima8/graphics/shape_info.h"
#include "ultima/ultima8/conf/setting_manager.h"
#include "ultima/ultima8/audio/music_process.h"
#include "ultima/ultima8/world/get_object.h"
#include "ultima/ultima8/misc/direction.h"
#include "ultima/ultima8/misc/direction_util.h"

namespace Ultima {
namespace Ultima8 {

AvatarMoverProcess::AvatarMoverProcess() : Process(),
		_lastFrame(0), _lastAttack(0), _idleTime(0),
		_movementFlags(0) {
	_type = 1; // CONSTANT! (type 1 = persistent)
}


AvatarMoverProcess::~AvatarMoverProcess() {
}

void AvatarMoverProcess::run() {
	Kernel *kernel = Kernel::get_instance();
	uint32 framenum = kernel->getFrameNum();

	// only run once per frame
	if (framenum == _lastFrame)
		return;
	_lastFrame = framenum;

	// busy, so don't move
	if (kernel->getNumProcesses(1, ActorAnimProcess::ACTOR_ANIM_PROC_TYPE) > 0) {
		_idleTime = 0;
		return;
	}

	MainActor *avatar = getMainActor();

	if (avatar->getLastAnim() == Animation::hang) {
		handleHangingMode();
		return;
	}

	// falling, so don't move
	if (avatar->getGravityPID() != 0) {
		_idleTime = 0;
		return;
	}

	bool combatRun = avatar->hasActorFlags(Actor::ACT_COMBATRUN);
	if (avatar->isInCombat() && !combatRun)
		handleCombatMode();
	else
		handleNormalMode();
}


void AvatarMoverProcess::tryAttack() {
	MainActor *avatar = getMainActor();
	Direction dir = avatar->getDir();
	if (!avatar->isInCombat()) {
		avatar->setInCombat(0);
		waitFor(avatar->doAnim(Animation::readyWeapon, dir));
	} else {
		if (canAttack()) {
			waitFor(avatar->doAnim(Animation::attack, dir));
			if (GAME_IS_CRUSADER) {
				// FIXME: put some real values in here.
				int32 xs, ys, zs;
				avatar->getFootpadWorld(xs, ys, zs);
				avatar->fireWeapon(xs / 2, ys / 2, zs / 2, dir, 1, 1);
			}
		}
	}
}

bool AvatarMoverProcess::checkTurn(Direction direction, bool moving) {
	MainActor *avatar = getMainActor();
	Direction curdir = avatar->getDir();
	bool combat = avatar->isInCombat() && !avatar->hasActorFlags(Actor::ACT_COMBATRUN);

	// Note: don't need to turn if moving backward in combat stance
	// CHECKME: currently, first turn in the right direction
	if (direction != curdir && !(combat && Direction_Invert(direction) == curdir)) {
		Animation::Sequence lastanim = avatar->getLastAnim();

		if (moving &&
				(lastanim == Animation::walk || lastanim == Animation::run ||
				 lastanim == Animation::combatStand) &&
				(ABS(direction - curdir) + 2) % 16 <= 4) {
			// don't need to explicitly do a turn animation
			return false;
		}

		if (moving && lastanim == Animation::run) {
			// slow down to a walk first
			waitFor(avatar->doAnim(Animation::walk, curdir));
			return true;
		}

		turnToDirection(direction);
		return true;
	}

	return false;
}

void AvatarMoverProcess::turnToDirection(Direction direction) {
	MainActor *avatar = getMainActor();
	uint16 turnpid = avatar->turnTowardDir(direction);
	if (turnpid)
		waitFor(turnpid);
}

void AvatarMoverProcess::onMouseDown(int button, int32 mx, int32 my) {
	int bid = 0;

	switch (button) {
	case Shared::BUTTON_LEFT: {
		bid = 0;
		break;
	}
	case Shared::BUTTON_RIGHT: {
		bid = 1;
		break;
	}
	default:
		CANT_HAPPEN_MSG("invalid MouseDown passed to AvatarMoverProcess");
		break;
	};

	_mouseButton[bid]._lastDown = _mouseButton[bid]._curDown;
	_mouseButton[bid]._curDown = g_system->getMillis();
	_mouseButton[bid].setState(MBS_DOWN);
	_mouseButton[bid].clearState(MBS_HANDLED);
}

void AvatarMoverProcess::onMouseUp(int button) {
	int bid = 0;

	if (button == Shared::BUTTON_LEFT) {
		bid = 0;
	} else if (button == Shared::BUTTON_RIGHT) {
		bid = 1;
	} else {
		CANT_HAPPEN_MSG("invalid MouseUp passed to AvatarMoverProcess");
	}

	_mouseButton[bid].clearState(MBS_DOWN);
}


void AvatarMoverProcess::saveData(Common::WriteStream *ws) {
	Process::saveData(ws);

	ws->writeUint32LE(_lastAttack);
	ws->writeUint32LE(_idleTime);
}

bool AvatarMoverProcess::loadData(Common::ReadStream *rs, uint32 version) {
	if (!Process::loadData(rs, version)) return false;

	_lastAttack = rs->readUint32LE();
	_idleTime = rs->readUint32LE();

	return true;
}

} // End of namespace Ultima8
} // End of namespace Ultima
