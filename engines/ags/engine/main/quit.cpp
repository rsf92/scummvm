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

//
// Quit game procedure
//

#include "ags/shared/core/platform.h"
#include "ags/engine/ac/cdaudio.h"
#include "ags/engine/ac/gamesetup.h"
#include "ags/shared/ac/gamesetupstruct.h"
#include "ags/engine/ac/roomstatus.h"
#include "ags/engine/ac/translation.h"
#include "ags/engine/debugging/agseditordebugger.h"
#include "ags/engine/debugging/debug_log.h"
#include "ags/engine/debugging/debugger.h"
#include "ags/shared/debugging/out.h"
#include "ags/shared/font/fonts.h"
#include "ags/engine/main/config.h"
#include "ags/engine/main/engine.h"
#include "ags/engine/main/main.h"
#include "ags/engine/main/mainheader.h"
#include "ags/engine/main/quit.h"
#include "ags/shared/ac/spritecache.h"
#include "ags/engine/gfx/graphicsdriver.h"
#include "ags/shared/gfx/bitmap.h"
#include "ags/shared/core/assetmanager.h"
#include "ags/engine/plugin/plugin_engine.h"
#include "ags/engine/media/audio/audio_system.h"
#include "ags/engine/globals.h"
#include "ags/ags.h"

namespace AGS3 {

using namespace AGS::Shared;
using namespace AGS::Engine;

extern GameSetupStruct game;
extern SpriteCache spriteset;
extern RoomStruct thisroom;
extern RoomStatus troom;    // used for non-saveable rooms, eg. intro
extern int our_eip;
extern GameSetup usetup;
extern char pexbuf[STD_BUFFER_SIZE];
extern int proper_exit;
extern char check_dynamic_sprites_at_exit;
extern int editor_debugging_initialized;
extern IAGSEditorDebugger *editor_debugger;
extern int need_to_stop_cd;
extern int use_cdplayer;
extern IGraphicsDriver *gfxDriver;

bool handledErrorInEditor;

void quit_tell_editor_debugger(const String &qmsg, QuitReason qreason) {
	if (editor_debugging_initialized) {
		if (qreason & kQuitKind_GameException)
			handledErrorInEditor = send_exception_to_editor(qmsg);
		send_message_to_editor("EXIT");
		editor_debugger->Shutdown();
	}
}

void quit_stop_cd() {
	if (need_to_stop_cd)
		cd_manager(3, 0);
}

void quit_shutdown_scripts() {
	ccUnregisterAllObjects();
}

void quit_check_dynamic_sprites(QuitReason qreason) {
	if ((qreason & kQuitKind_NormalExit) && (check_dynamic_sprites_at_exit) &&
		(game.options[OPT_DEBUGMODE] != 0)) {
		// game exiting normally -- make sure the dynamic sprites
		// have been deleted
		for (int i = 1; i < spriteset.GetSpriteSlotCount(); i++) {
			if (game.SpriteInfos[i].Flags & SPF_DYNAMICALLOC)
				debug_script_warn("Dynamic sprite %d was never deleted", i);
		}
	}
}

void quit_shutdown_platform(QuitReason qreason) {
	// Be sure to unlock mouse on exit, or users will hate us
	platform->UnlockMouse();
	platform->AboutToQuitGame();

	our_eip = 9016;

	pl_stop_plugins();

	quit_check_dynamic_sprites(qreason);

	platform->FinishedUsingGraphicsMode();

	if (use_cdplayer)
		platform->ShutdownCDPlayer();
}

void quit_shutdown_audio() {
	our_eip = 9917;
	game.options[OPT_CROSSFADEMUSIC] = 0;
	stopmusic();
#ifndef PSP_NO_MOD_PLAYBACK
	if (usetup.mod_player)
		remove_mod_player();
#endif

	// Quit the sound thread.
	audioThread.Stop();

	remove_sound();
}

QuitReason quit_check_for_error_state(const char *&qmsg, String &alertis) {
	if (qmsg[0] == '|') {
		return kQuit_GameRequest;
	} else if (qmsg[0] == '!') {
		QuitReason qreason;
		qmsg++;

		if (qmsg[0] == '|') {
			qreason = kQuit_UserAbort;
			alertis = "Abort key pressed.\n\n";
		} else if (qmsg[0] == '?') {
			qmsg++;
			qreason = kQuit_ScriptAbort;
			alertis = "A fatal error has been generated by the script using the AbortGame function. Please contact the game author for support.\n\n";
		} else {
			qreason = kQuit_GameError;
			alertis.Format("An error has occurred. Please contact the game author for support, as this "
				"is likely to be a scripting error and not a bug in AGS.\n"
				"(ACI version %s)\n\n", _G(EngineVersion).LongString.GetCStr());
		}

		alertis.Append(get_cur_script(5));

		if (qreason != kQuit_UserAbort)
			alertis.Append("\nError: ");
		else
			qmsg = "";
		return qreason;
	} else if (qmsg[0] == '%') {
		qmsg++;
		alertis.Format("A warning has been generated. This is not normally fatal, but you have selected "
			"to treat warnings as errors.\n"
			"(ACI version %s)\n\n%s\n", _G(EngineVersion).LongString.GetCStr(), get_cur_script(5).GetCStr());
		return kQuit_GameWarning;
	} else {
		alertis.Format("An internal error has occurred. Please note down the following information.\n"
			"If the problem persists, post the details on the AGS Technical Forum.\n"
			"(ACI version %s)\n"
			"\nError: ", _G(EngineVersion).LongString.GetCStr());
		return kQuit_FatalError;
	}
}

void quit_message_on_exit(const char *qmsg, String &alertis, QuitReason qreason) {
	// successful exit displays no messages (because Windoze closes the dos-box
	// if it is empty).
	if ((qreason & kQuitKind_NormalExit) == 0 && !handledErrorInEditor) {
		// Display the message (at this point the window still exists)
		sprintf(pexbuf, "%s\n", qmsg);
		alertis.Append(pexbuf);
		platform->DisplayAlert("%s", alertis.GetCStr());
	}
}

void quit_release_data() {
	resetRoomStatuses();
	thisroom.Free();
	play.Free();

	/*  _CrtMemState memstart;
	_CrtMemCheckpoint(&memstart);
	_CrtMemDumpStatistics( &memstart );*/

	Shared::AssetManager::DestroyInstance();
}

void quit_delete_temp_files() {
#ifdef TODO
	al_ffblk    dfb;
	int dun = al_findfirst("~ac*.tmp", &dfb, FA_SEARCH);
	while (!dun) {
		::remove(dfb.name);
		dun = al_findnext(&dfb);
	}
	al_findclose(&dfb);
#endif
}

// TODO: move to test unit
extern Bitmap *test_allegro_bitmap;
extern IDriverDependantBitmap *test_allegro_ddb;
void allegro_bitmap_test_release() {
	delete test_allegro_bitmap;
	if (test_allegro_ddb)
		gfxDriver->DestroyDDB(test_allegro_ddb);
}

char return_to_roomedit[30] = "\0";
char return_to_room[150] = "\0";
char quit_message[256] = "\0";

// quit - exits the engine, shutting down everything gracefully
// The parameter is the message to print. If this message begins with
// an '!' character, then it is printed as a "contact game author" error.
// If it begins with a '|' then it is treated as a "thanks for playing" type
// message. If it begins with anything else, it is treated as an internal
// error.
// "!|" is a special code used to mean that the player has aborted (Alt+X)
void quit(const char *quitmsg) {
	strncpy(quit_message, quitmsg, 256);
	_G(abort_engine) = true;
}

void quit_free() {
	String alertis;
	if (strlen(quit_message) == 0)
		strcpy(quit_message, "|bye!");

	const char *quitmsg = quit_message;
	QuitReason qreason = quit_check_for_error_state(quitmsg, alertis);

	if (qreason & kQuitKind_NormalExit)
		save_config_file();

	allegro_bitmap_test_release();

	handledErrorInEditor = false;

	quit_tell_editor_debugger(quit_message, qreason);

	our_eip = 9900;

	quit_stop_cd();

	our_eip = 9020;

	quit_shutdown_scripts();

	quit_shutdown_platform(qreason);

	our_eip = 9019;

	quit_shutdown_audio();

	our_eip = 9901;

	shutdown_font_renderer();
	our_eip = 9902;

	spriteset.Reset();

	our_eip = 9907;

	close_translation();

	our_eip = 9908;

	shutdown_pathfinder();

	engine_shutdown_gfxmode();

	quit_message_on_exit(quitmsg, alertis, qreason);

	quit_release_data();

	// release backed library
	// WARNING: no Allegro objects should remain in memory after this,
	// if their destruction is called later, program will crash!
	allegro_exit();

	platform->PostAllegroExit();

	our_eip = 9903;

	quit_delete_temp_files();

	proper_exit = 1;

	Debug::Printf(kDbgMsg_Alert, "***** ENGINE HAS SHUTDOWN");

	shutdown_debug();

	our_eip = 9904;
}

extern "C" {
	void quit_c(char *msg) {
		quit(msg);
	}
}

} // namespace AGS3
