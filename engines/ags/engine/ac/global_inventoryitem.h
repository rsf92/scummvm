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

#ifndef __AGS_EE_AC__GLOBALINVENTORYITEM_H
#define __AGS_EE_AC__GLOBALINVENTORYITEM_H

void set_inv_item_pic(int invi, int piccy);
void SetInvItemName(int invi, const char *newName);
int  GetInvAt (int xxx, int yyy);
void GetInvName(int indx,char*buff);
int  GetInvGraphic(int indx);
void RunInventoryInteraction (int iit, int modd);
int  IsInventoryInteractionAvailable (int item, int mood);
int  GetInvProperty (int item, const char *property);
void GetInvPropertyText (int item, const char *property, char *bufer);

#endif // __AGS_EE_AC__GLOBALINVENTORYITEM_H
