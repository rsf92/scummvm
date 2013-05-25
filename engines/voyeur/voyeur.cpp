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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "voyeur/voyeur.h"
#include "voyeur/graphics.h"
#include "common/scummsys.h"
#include "common/config-manager.h"
#include "common/debug-channels.h"

namespace Voyeur {

VoyeurEngine *g_vm;

VoyeurEngine::VoyeurEngine(OSystem *syst, const VoyeurGameDescription *gameDesc) : Engine(syst),
		_gameDescription(gameDesc), _randomSource("Voyeur") {
	DebugMan.addDebugChannel(kDebugPath, "Path", "Pathfinding debug level");
	_bVoyBoltFile = NULL;
}

VoyeurEngine::~VoyeurEngine() {
	delete _bVoyBoltFile;
}

Common::String VoyeurEngine::generateSaveName(int slot) {
	return Common::String::format("%s.%03d", _targetName.c_str(), slot);
}

/**
 * Returns true if it is currently okay to restore a game
 */
bool VoyeurEngine::canLoadGameStateCurrently() {
	return true;
}

/**
 * Returns true if it is currently okay to save the game
 */
bool VoyeurEngine::canSaveGameStateCurrently() {
	return true;
}

/**
 * Load the savegame at the specified slot index
 */
Common::Error VoyeurEngine::loadGameState(int slot) {
	return Common::kNoError;
}

/**
 * Save the game to the given slot index, and with the given name
 */
Common::Error VoyeurEngine::saveGameState(int slot, const Common::String &desc) {
	//TODO
	return Common::kNoError;
}

Common::Error VoyeurEngine::run() {
	ESP_Init();
	globalInitBolt();

	_eventManager.resetMouse();

	//doHeadTitle();

	return Common::kNoError;
}


int VoyeurEngine::getRandomNumber(int maxNumber) {
	return _randomSource.getRandomNumber(maxNumber);
}

void VoyeurEngine::initialiseManagers() {
	_eventManager.setVm(this);
	_graphicsManager.setVm(this);
}

void VoyeurEngine::ESP_Init() {
}

void VoyeurEngine::globalInitBolt() {
	initBolt();

	_filesManager.openBoltLib("bvoy.blt", _bVoyBoltFile);
	_bVoyBoltFile->getBoltGroup(0x10000);
	_bVoyBoltFile->getBoltGroup(0x10100);
	_fontPtr = _bVoyBoltFile->memberAddr(0x101);

	// Setup default flags
	Common::fill((byte *)&_voy, (byte *)&_voy + sizeof(SVoy), 0);
	_voy._eCursorOff[0x74 / 2] = 1;
	_voy._eCursorOff[0x68 / 2] = 0;
	_voy._eventTable[0x3e6]._data3 = 63;
	_voy._eventTable[0x3e6]._data4 = 63;
	_voy._evidence[19] = 0;
	_voy._evidence[17] = 0;
	_voy._evidence[18] = 9999;
	
	_voy._curICF0 = _graphicsManager._palFlag ? 0xFFFFA5E0 : 0x5F90;
	_graphicsManager.addFadeInt();
}

void VoyeurEngine::initBolt() {
	vInitInterrupts();
	_graphicsManager.sInitGraphics();
	_graphicsManager.vInitColor();
	initInput();
}

void VoyeurEngine::vInitInterrupts() {
	_intPtr._colors = &_graphicsManager._VGAColors[0];
}

void VoyeurEngine::initInput() {
}

} // End of namespace Voyeur
