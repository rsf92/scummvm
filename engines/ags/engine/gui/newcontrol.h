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

#ifndef __AGS_EE_GUI__NEWCONTROL_H
#define __AGS_EE_GUI__NEWCONTROL_H

#include "gfx/bitmap.h"

using namespace AGS; // FIXME later

struct NewControl
{
  int x, y, wid, hit, state, typeandflags, wlevel;
  char visible, enabled;        // not implemented
  char needredraw;
  virtual void draw(Common::Bitmap *ds) = 0;
  virtual int pressedon(int mousex, int mousey) = 0;
  virtual int processmessage(int, int, long) = 0;

  NewControl(int xx, int yy, int wi, int hi);
  NewControl();
  virtual ~NewControl() = default;
  int mouseisinarea(int mousex, int mousey);
  void drawifneeded();
  void drawandmouse();
};

#endif // __AGS_EE_GUI__NEWCONTROL_H
