/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "chewy/defines.h"
#include "chewy/events.h"
#include "chewy/global.h"
#include "chewy/ani_dat.h"
#include "chewy/room.h"
#include "chewy/rooms/room44.h"

namespace Chewy {
namespace Rooms {

void Room44::look_news() {
	int16 tmpsx, tmpsy;
	int16 tmproom;

	tmproom = _G(spieler).PersonRoomNr[P_CHEWY];
	tmpsx = _G(spieler).scrollx;
	tmpsy = _G(spieler).scrolly;
	_G(spieler).scrollx = 0;
	_G(spieler).scrolly = 0;
	hide_person();
	fx_blend = BLEND1;
	_G(spieler).PersonRoomNr[P_CHEWY] = 44;
	room->load_room(&room_blk, _G(spieler).PersonRoomNr[P_CHEWY], &_G(spieler));
	ERROR

	start_aad_wait(172, -1);
	_G(spieler).scrollx = tmpsx;
	_G(spieler).scrolly = tmpsy;
	switch_room(tmproom);
	show_person();
}

} // namespace Rooms
} // namespace Chewy
