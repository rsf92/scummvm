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

#include "common/config-manager.h"
#include "chewy/dialogs/files.h"
#include "chewy/dialogs/options.h"
#include "chewy/events.h"
#include "chewy/file.h"
#include "chewy/global.h"

namespace Chewy {
namespace Dialogs {

#define NUM_VISIBLE_SLOTS 6

enum Widget {
	SCROLL_UP = 0, SCROLL_DOWN = 1, SAVE = 2, LOAD = 3,
	GAME = 4, QUIT = 5, OPTIONS = 6, W7 = 7, W8 = 8
};

static const int16 FILE_ICONS[8 * 4] = {
	14, 73, 32, 94,
	14, 96, 32, 118,
	36, 64, 310, 128,
	16, 143, 76, 193,
	78, 143, 130, 193,
	132, 143, 178, 193,
	180, 143, 228, 193,
	232, 143, 310, 193
};


int16 Files::execute(bool isInGame) {
	int16 key = 0;
	int16 i, j;
	Common::Point pt[8];
	int16 mode[9];
	bool visibility[8];
	int16 ret = 0;
	char *fnames;
	char *tmp;
	int16 text_off, active_slot;
	int16 rect, line;
	bool flag = false;

	if (!ConfMan.getBool("original_menus")) {
		g_engine->showGmm(isInGame);
		return 0;
	}

	taf_info *ti = _G(mem)->taf_adr(OPTION_TAF);
	EVENTS_CLEAR;

	_G(room)->open_handle(GBOOK, "rb", 0);
	_G(room)->load_tgp(1, &_G(room_blk), GBOOK_TGP, 0, GBOOK);
	_G(out)->setze_zeiger(_G(workptr));
	_G(out)->map_spr2screen(_G(ablage)[_G(room_blk).AkAblage], 0, 0);
	_G(out)->setze_zeiger(_G(screen0));
	_G(room)->set_ak_pal(&_G(room_blk));
	fnames = _G(iog)->io_init(&_G(ioptr));
	fnames += 1;
 
	_G(fx)->blende1(_G(workptr), _G(screen0), _G(pal), 150, 0, 0);
	_G(out)->setze_zeiger(_G(workptr));
	show_cur();

	if (!_G(modul)) {
		pt[SCROLL_UP] = pt[SCROLL_DOWN] = Common::Point(1, 0);
		for (i = SAVE; i <= W7; i++)
			pt[i] = Common::Point(5, 5);

		Common::fill(visibility, visibility + 8, true);
		Common::fill(mode, mode + 8, 0);
		mode[W8] = 1;

		if (!isInGame) {
			visibility[SAVE] = false;
			visibility[GAME] = false;
			mode[LOAD] = 1;
			mode[W8] = 0;
		}

		text_off = 0;		// Top visible save slot
		active_slot = 0;	// Currently selected slot

		while (key != Common::KEYCODE_ESCAPE && !SHOULD_QUIT) {
			// Draw the dialog background
			_G(out)->map_spr2screen(_G(ablage)[_G(room_blk).AkAblage], 0, 0);

			// Draw the buttons at the bottom
			for (i = 28, j = SCROLL_UP; j <= OPTIONS; i++, j++) {
				if (visibility[j]) {
					if (!mode[j])
						// Not pressed
						_G(out)->sprite_set(
							ti->image[i], 16 + ti->korrektur[i << 1] + pt[j].x,
							76 + ti->korrektur[(i << 1) + 1] + pt[j].y, 0);
					else
						// Pressed
						_G(out)->sprite_set(ti->image[i], 16 + ti->korrektur[i << 1],
							76 + ti->korrektur[(i << 1) + 1], 0);
				}
			}

			// Write the list of savegame slots
			tmp = fnames + (text_off * 40);
			for (i = 0; i < NUM_VISIBLE_SLOTS; i++, tmp += 40) {
				if (i != active_slot) {
					_G(out)->printxy(40, 68 + (i * 10), 14, 300, 0, "%2d.", text_off + i);
					_G(out)->printxy(70, 68 + (i * 10), 14, 300, 0, tmp);
				} else {
					_G(out)->box_fill(40, 68 + (i * 10), 308, 68 + 8 + (i * 10), 42);
					_G(out)->printxy(40, 68 + (i * 10), 255, 300, 0, "%2d.", text_off + i);
					_G(out)->printxy(70, 68 + (i * 10), 255, 300, 0, tmp);
				}
			}

			key = _G(in)->get_switch_code();

			if (mode[SCROLL_UP])
				--mode[SCROLL_UP];
			if (mode[SCROLL_DOWN])
				--mode[SCROLL_DOWN];

			if (mode[GAME])
				--mode[GAME];
			if (mode[GAME] == 1)
				key = Common::KEYCODE_ESCAPE;

			if (mode[QUIT])
				--mode[QUIT];
			if (mode[QUIT] == 1) {
				_G(out)->printxy(120, 138, 255, 300, 0, QUIT_MSG);
				_G(out)->back2screen(_G(workpage));
				_G(in)->alter_kb_handler();

				key = getch();
				if (key == 'j' || key == 'J' || key == 'y' || key == 'Y' || key == 'z' || key == 'Z') {
					ret = 1;
					key = Common::KEYCODE_ESCAPE;
				} else {
					key = 0;
				}

				_G(in)->neuer_kb_handler(&_G(kbinfo));
			}

			if (mode[OPTIONS])
				--mode[OPTIONS];
			if (mode[OPTIONS] == 1) {
				Dialogs::Options::execute(ti);
			}

			if (!flag && _G(minfo).button == 1) {
				rect = _G(in)->maus_vector(_G(minfo).x, _G(minfo).y, FILE_ICONS, 8);
				flag = true;
				key = 0;

				switch (rect) {
				case 0:
					key = Common::KEYCODE_UP;
					break;
				case 1:
					key = Common::KEYCODE_DOWN;
					break;
				case 2:
					line = (_G(minfo).y - 68) / 10;
					if (line == active_slot)
						key = Common::KEYCODE_RETURN;
					else
						active_slot = line;
					if (active_slot > 5)
						active_slot = 5;

					if (!isInGame)
						goto enter;
					break;
				case 3:
					key = Common::KEYCODE_F1;
					break;
				case 4:
					key = Common::KEYCODE_F2;
					break;
				case 5:
					key = Common::KEYCODE_F3;
					break;
				case 6:
					key = Common::KEYCODE_F4;
					break;
				case 7:
					key = Common::KEYCODE_F5;
					break;
				}

			} else if (flag && _G(minfo).button == 0) {
				flag = false;
			}

			switch (key) {
			case Common::KEYCODE_F1:
				if (visibility[SAVE]) {
					mode[LOAD] = 0;
					mode[SAVE] = 1;
					mode[W8] = 0;
					goto enter;
				}
				break;

			case Common::KEYCODE_F2:
				mode[LOAD] = 1;
				mode[SAVE] = 0;
				mode[W8] = 0;
				goto enter;
				break;

			case Common::KEYCODE_F3:
				if (visibility[GAME])
					mode[GAME] = 10;
				break;

			case Common::KEYCODE_F4:
				mode[QUIT] = 10;
				break;

			case Common::KEYCODE_F5:
				mode[OPTIONS] = 10;
				break;

			case Common::KEYCODE_UP:
				mode[SCROLL_UP] = 10;
				if (active_slot > 0)
					--active_slot;
				else if (text_off > 0)
					--text_off;
				break;

			case Common::KEYCODE_DOWN:
				mode[SCROLL_DOWN] = 10;
				if (active_slot < (NUM_VISIBLE_SLOTS - 1))
					++active_slot;
				else if (text_off < (20 - NUM_VISIBLE_SLOTS))
					++text_off;
				break;

			case Common::KEYCODE_RETURN:
			case Common::KEYCODE_RETURN + ALT:
enter:
				if (mode[LOAD]) {
					tmp = fnames + ((text_off + active_slot) * 40);
					if (tmp[0]) {
						_G(currentSong) = -1;
						_G(iog)->load(text_off + active_slot, _G(ioptr).save_path);
						key = Common::KEYCODE_ESCAPE;
					}
				} else if (mode[SAVE]) {
					_G(out)->back2screen(_G(workpage));
					_G(out)->setze_zeiger(_G(screen0));
					_G(in)->alter_kb_handler();
					tmp = fnames + ((text_off + active_slot) * 40);
					key = _G(out)->scanxy(70, 68 + (active_slot * 10),
						255, 42, 14, 0, "%36s36", tmp);
					_G(in)->neuer_kb_handler(&_G(kbinfo));
					_G(out)->setze_zeiger(_G(workptr));
					if (key != Common::KEYCODE_ESCAPE) {
						_G(iog)->save_entry(text_off + active_slot,
							_G(ioptr).save_path);
					}
					key = Common::KEYCODE_ESCAPE;
				}
				break;

			default:
				break;
			}

			_G(cur)->plot_cur();
			_G(out)->back2screen(_G(workpage));

			EVENTS_UPDATE;
		}
	}

	free(ti);

	_G(room)->open_handle(EPISODE1, "rb", 0);
	_G(room)->load_tgp(_G(spieler).PersonRoomNr[P_CHEWY], &_G(room_blk), EPISODE1_TGP, GED_LOAD, EPISODE1);

	_G(fx_blend) = BLEND1;
	_G(room)->set_ak_pal(&_G(room_blk));

	return ret;
}

} // namespace Dialogs
} // namespace Chewy
