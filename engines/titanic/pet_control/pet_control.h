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

#ifndef TITANIC_PET_CONTROL_H
#define TITANIC_PET_CONTROL_H

#include "titanic/core/game_object.h"
#include "titanic/core/node_item.h"
#include "titanic/core/room_item.h"
#include "titanic/pet_control/pet_conversation_section.h"
#include "titanic/pet_control/pet_inventory_section.h"
#include "titanic/pet_control/pet_remote_section.h"
#include "titanic/pet_control/pet_rooms_section.h"
#include "titanic/pet_control/pet_save_section.h"
#include "titanic/pet_control/pet_control_sub5.h"
#include "titanic/pet_control/pet_control_sub7.h"
#include "titanic/pet_control/pet_control_sub8.h"

namespace Titanic {

class CPetControl : public CGameObject {
private:
	PetArea _currentArea;
	int _fieldC0;
	int _locked;
	int _fieldC8;
	CPetConversationSection _convSection;
	CPetInventorySection _invSection;
	CPetRemoteSection _remoteSection;
	CPetRoomsSection _roomsSection;
	CPetSaveSection _saveSection;
	CPetControlSub5 _sub5;
	CPetControlSub7 _sub7;
	CPetControlSub8 _sub8;
	CTreeItem *_treeItem1;
	CString _string1;
	CTreeItem *_treeItem2;
	CString _string2;
	int _field13A4;
	Rect _oldBounds;
private:
	/**
	 * Returns true if the control is in a valid state
	 */
	bool isValid() const;

	/**
	 * Loads data for the individual areas
	 */
	void loadAreas(SimpleFile *file, int param);

	/**
	 * Saves data for the individual areas
	 */
	void saveAreas(SimpleFile *file, int indent) const;

	/**
	 * Called at the end of the post game-load handling
	 */
	void loaded();
public:
	CLASSDEF

	/**
	 * Save the data for the class to file
	 */
	virtual void save(SimpleFile *file, int indent) const;

	/**
	 * Load the data for the class from file
	 */
	virtual void load(SimpleFile *file);

	/**
	 * Allows the item to draw itself
	 */
	virtual void draw(CScreenManager *screenManager);

	/**
	 * Called after loading a game has finished
	 */
	void postLoad();

	/**
	 * Called when a new node is entered
	 */
	void enterNode(CNodeItem *node);

	/**
	 * Called when a new room is entered
	 */
	void enterRoom(CRoomItem *room);

	/**
	 * Called to clear the PET display
	 */
	void clear();

	bool fn1(int val);

	void fn2(int val);

	void fn3(CTreeItem *item);

	void fn4();

	/**
	 * Sets the currently viewed area within the PET
	 */
	PetArea setArea(PetArea newSection);

	/**
	 * Returns true if the current area can be changed
	 */
	bool canChangeArea() const { return _locked == 0; }
};

} // End of namespace Titanic

#endif /* TITANIC_PET_CONTROL_H */
