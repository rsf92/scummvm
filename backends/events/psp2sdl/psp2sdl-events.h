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

#if !defined(DISABLE_DEFAULT_EVENTMANAGER)
#define BACKEND_EVENTS_PSP2_H

#include "backends/events/sdl/sdl-events.h"
#include <psp2/touch.h>

/**
 * SDL Events manager for the PSP2.
 */
class PSP2EventSource : public SdlEventSource {
public:
	PSP2EventSource();
protected:
	void preprocessEvents(SDL_Event *event) override;
private:

	enum {
		MAX_NUM_FINGERS = 3, // number of fingers to track per panel
		MAX_TAP_TIME = 250, // taps longer than this will not result in mouse click events
	}; // track three fingers per panel

	typedef struct {
		int id; // -1: no touch
		Uint32 timeLastDown;
		int lastX; // last known screen coordinates
		int lastY; // last known screen coordinates
	} Touch;

	Touch _finger[SCE_TOUCH_PORT_MAX_NUM][MAX_NUM_FINGERS]; // keep track of finger status

	typedef enum DraggingType {
		DRAG_NONE = 0,
		DRAG_TWO_FINGER,
		DRAG_THREE_FINGER,
	} DraggingType;

	DraggingType _multiFingerDragging[SCE_TOUCH_PORT_MAX_NUM]; // keep track whether we are currently drag-and-dropping

	void preprocessFingerDown(SDL_Event *event);
	void preprocessFingerUp(SDL_Event *event);
	void preprocessFingerMotion(SDL_Event *event);
	void convertTouchXYToGameXY(float touchX, float touchY, int *gameX, int *gameY);
};

#endif /* BACKEND_EVENTS_PSP2_H */
