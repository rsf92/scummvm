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

#ifndef CHEWY_TYPES_H
#define CHEWY_TYPES_H

#include "common/serializer.h"
#include "chewy/defines.h"
#include "chewy/objekt.h"
#include "chewy/detail.h"

namespace Chewy {

struct SpielerFlags {
	byte R0SchleimWurf : 1;
	byte R0KissenWurf : 1;
	byte R0Monokel : 1;
	byte R0GBuch : 1;
	byte R2KabelBork : 1;
	byte R2FussSchleim : 1;
	byte R5Terminal : 1;
	byte R5Tuer : 1;

	byte R6BolaSchild : 1;
	byte R6BolaOk : 1;
	byte R6BolaBecher : 1;
	byte R6DoorLeftF : 1;
	byte R6DoorLeftB : 1;
	byte R6DoorRightB : 1;
	byte R7DoorRight : 1;
	byte R7Hebel : 1;

	byte R7SeilLeft : 1;
	byte R7SeilOk : 1;
	byte R7RHaken : 1;
	byte R7BorkFlug : 1;
	byte R7ChewyFlug : 1;
	byte R8Folter : 1;
	byte R8Stein : 1;
	byte R8GTuer : 1;

	byte R8Kohle : 1;
	byte R8Abkuehlen : 1;
	byte R8GipsWurf : 1;
	byte R9Gitter : 1;
	byte R9Surimy : 1;
	byte R10Surimy : 1;
	byte R10SurimyOk : 1;
	byte R11CardOk : 1;

	byte R11TerminalOk : 1;
	byte R11DoorRightB : 1;
	byte R11DoorRightF : 1;
	byte R12Betreten : 1;
	byte R12ChewyBork : 1;
	byte R12Talisman : 1;
	byte R12TalismanOk : 1;
	byte R12RaumOk : 1;

	byte R12BorkTalk : 1;
	byte R12BorkInRohr : 1;
	byte R12TransOn : 1;
	byte R12KetteLinks : 1;
	byte R13BorkOk : 1;
	byte R13Band : 1;
	byte R13Bandlauf : 1;
	byte R13Surf : 1;

	byte R14Feuer : 1;
	byte R14Translator : 1;
	byte R14FluxoFlex : 1;
	byte R14GleiterAuf : 1;
	byte R14Sicherung : 1;
	byte R14Waffe : 1;
	byte R16F5Exit : 1;
	byte R17GitterWeg : 1;

	byte R17Seil : 1;
	byte R17HebelOk : 1;
	byte R17EnergieOut : 1;
	byte R17DoorKommand : 1;
	byte R18DoorBruecke : 1;
	byte R18CartFach : 1;
	byte R18CartTerminal : 1;
	byte R18CartSave : 1;

	byte R18SurimyWurf : 1;
	byte R18SondeMoni : 1;
	byte R18MoniSwitch : 1;
	byte R18Krone : 1;
	byte R18FirstEntry : 1;
	byte R18Gitter : 1;
	byte R21Salto : 1;
	byte R21GitterEnergie : 1;

	byte R21GitterMuell : 1;
	byte R21Hebel1 : 1;
	byte R21Hebel2 : 1;
	byte R21Hebel3 : 1;
	byte R21Laser1Weg : 1;
	byte R21Laser2Weg : 1;
	byte R21Gang : 1;
	byte R22Paint : 1;

	byte R22ChewyPlatt : 1;
	byte R22BorkPlatt : 1;
	byte R22GetBork : 1;
	byte R23FluxoFlex : 1;
	byte R23Cartridge : 1;
	byte R24FirstEntry : 1;
	byte R25FirstEntry : 1;
	byte R25GleiteLoesch : 1;

	byte R25SurimyLauf : 1;
	byte R25GleiterExit : 1;
	byte R27SurimyOk : 1;
	byte R28SurimyCar : 1;
	byte R28ChewyPump : 1;
	byte R28Briefkasten : 1;
	byte R28EntryHaus : 1;
	byte R28Manuskript : 1;

	byte R28RKuerbis : 1;
	byte R28ExitTown : 1;
	byte R28PostCar : 1;
	byte R29Pumpe : 1;
	byte R29Schlauch1 : 1;
	byte R29Schlauch2 : 1;
	byte R29AutoSitz : 1;
	byte R31PflanzeWeg : 1;

	byte R31KoernerDa : 1;
	byte R31Wasser : 1;
	byte R31KlappeZu : 1;
	byte R31SurFurz : 1;
	byte R32HowardWeg : 1;
	byte R32UseSchreib : 1;
	byte R32PapierOk : 1;
	byte R32Script : 1;

	byte R33SchubFirst : 1;
	byte R33Messer : 1;
	byte R33MunterOk : 1;
	byte R33MunterGet : 1;
	byte R35TransCat : 1;
	byte R35Falle : 1;
	byte R35Schublade : 1;
	byte R35CatEat : 1;

	byte R37TransHahn : 1;
	byte R37Gebiss : 1;
	byte R37HundScham : 1;
	byte R37Kloppe : 1;
	byte R37Mes : 1;
	byte R39TransMensch : 1;
	byte R39TvOn : 1;
	byte R39ScriptOk : 1;

	byte R39HowardDa : 1;
	byte R39HowardWach : 1;
	byte R40TeilKarte : 1;
	byte R40TrainMove : 1;
	byte R40TrainOk : 1;
	byte R40Geld : 1;
	byte R40PoliceStart : 1;
	byte R40PoliceWeg : 1;

	byte R40PoliceAb : 1;
	byte R40Wettbewerb : 1;
	byte R40HoUse : 1;
	byte R40HaendlerOk : 1;
	byte R40DuengerMit : 1;
	byte R40DuengerTele : 1;
	byte R41FirstTalk : 1;
	byte R41LolaOk : 1;

	byte R41Einbruch : 1;
	byte R41BruchInfo : 1;
	byte R41KuerbisInfo : 1;
	byte R41RepairInfo : 1;
	byte R41HowardDiaOK : 1;
	byte R42FirstEntry : 1;
	byte R42BriefOk : 1;
	byte R42HoToBeamter : 1;

	byte R42MarkeOk : 1;
	byte R42BriefMarke : 1;
	byte R42BeamterWach : 1;
	byte R43GetPgLady : 1;
	byte R45TaxiOk : 1;
	byte R45MagOk : 1;
	byte R46GetLeder : 1;
	byte R47SchlossOk : 1;

	byte R48TaxiEntry : 1;
	byte R49WegFrei : 1;
	byte R49BoyWeg : 1;
	byte R49BoyAni : 1;
	byte R50Zigarre : 1;
	byte R50KeyOK : 1;
	byte R51FirstEntry : 1;
	byte R51HotelRoom : 1;

	byte R51KillerWeg : 1;
	byte R52HotDogOk : 1;
	byte R52TuerAuf : 1;
	byte R52LichtAn : 1;
	byte R52KakerJoke : 1;
	byte R52KakerWeg : 1;
	byte R53Kostuem : 1;
	byte R53Visit : 1;

	byte R54HotDogOk : 1;
	byte R54FputzerWeg : 1;
	byte R54LiftOk : 1;
	byte R54Schild : 1;
	byte R55SekWeg : 1;
	byte R55Location : 1;
	byte R55Entry : 1;
	byte R55Job : 1;

	byte R55ScriptWeg : 1;
	byte R55EscScriptOk : 1;
	byte R55RaumOk : 1;
	byte R55R54First : 1;
	byte R56GetTabak : 1;
	byte R56AbfahrtOk : 1;
	byte R56Kneipe : 1;
	byte R56WhiskyMix : 1;

	byte R57StudioAuf : 1;
	byte R59PosterWeg : 1;
	byte R62Flucht : 1;
	byte R62FirstEntry : 1;
	byte R62TShow : 1;
	byte R62LauraVerwandlung : 1;
	byte R63Feuer : 1;
	byte R63FxMannWeg : 1;

	byte R63Uhr : 1;
	byte R64ManWeg : 1;
	byte flags26_4 : 1;
	byte flags26_8 : 1;
	byte flags26_10 : 1;
	byte flags26_20 : 1;
	byte flags26_40 : 1;
	byte R67LiedOk : 1;

	byte R67KommodeAuf : 1;
	byte R67KostuemWeg : 1;
	byte R67PapageiWeg : 1;
	byte R68KarteDa : 1;
	byte R68Papagei : 1;
	byte R68DivaWeg : 1;
	byte R68Lied : 1;
	byte R68IndigoDia : 1;

	byte R68Gutschein : 1;
	byte flags28_2 : 1;
	byte flags28_4 : 1;
	byte flags28_8 : 1;
	byte flags28_10 : 1;
	byte flags28_20 : 1;
	byte flags28_40 : 1;
	byte flags28_80 : 1;

	byte flags29_1 : 1;
	byte flags29_2 : 1;
	byte flags29_4 : 1;
	byte flags29_8 : 1;
	byte flags29_10 : 1;
	byte flags29_20 : 1;
	byte flags29_40 : 1;
	byte flags29_80 : 1;

	byte flags30_1 : 1;
	byte flags30_2 : 1;
	byte flags30_4 : 1;
	byte flags30_8 : 1;
	byte flags30_10 : 1;
	byte flags30_20 : 1;
	byte flags30_40 : 1;
	byte flags30_80 : 1;

	byte flags31_1 : 1;
	byte flags31_2 : 1;
	byte flags31_4 : 1;
	byte flags31_8 : 1;
	byte flags31_10 : 1;

	// Further flags
	uint8 flags32_1 : 1;
	uint8 flags32_2 : 2;
	uint8 flags32_4 : 3;
	uint8 flags32_8 : 4;
	uint8 flags32_10 : 5;
	uint8 flags32_20 : 6;
	uint8 flags32_40 : 7;
	uint8 flags32_80 : 8;

	uint8 flags33_1 : 1;
	uint8 flags33_2 : 2;
	uint8 flags33_4 : 3;
	uint8 flags33_8 : 4;
	uint8 flags33_10 : 5;
	uint8 flags33_20 : 6;
	uint8 flags33_40 : 7;
	uint8 flags33_80 : 8;

	uint8 flags34_1 : 1;
	uint8 flags34_2 : 2;
	uint8 flags34_4 : 3;
	uint8 flags34_8 : 4;
	uint8 flags34_10 : 5;
	uint8 flags34_20 : 6;
	uint8 flags34_40 : 7;
	uint8 flags34_80 : 8;

	uint8 flags35_1 : 1;
	uint8 flags35_2 : 2;
	uint8 flags35_4 : 3;
	uint8 flags35_8 : 4;
	uint8 flags35_10 : 5;
	uint8 flags35_20 : 6;
	uint8 flags35_40 : 7;
	uint8 flags35_80 : 8;

	uint8 flags36_1 : 1;
	uint8 flags36_2 : 2;
	uint8 flags36_4 : 3;
	uint8 flags36_8 : 4;
	uint8 flags36_10 : 5;
	uint8 flags36_20 : 6;
	uint8 flags36_40 : 7;
	uint8 flags36_80 : 8;

	uint8 flags37_1 : 1;
	uint8 flags37_2 : 2;
	uint8 flags37_4 : 3;
	uint8 flags37_8 : 4;
	uint8 flags37_10 : 5;
	uint8 flags37_20 : 6;
	uint8 flags37_40 : 7;
	uint8 flags37_80 : 8;

	uint8 flags38_1 : 1;
	uint8 flags38_2 : 2;
};

struct Spieler : public SpielerFlags {
	Spieler() : SpielerFlags(), _flags(this) {
	}

	/**
	 * For loading or savign the structure data
	 */
	bool synchronize(Common::Serializer &s);

	SpielerFlags *_flags;
	uint8 Ats[ROOM_ATS_MAX * 3];
	uint8 InvAts[MAX_MOV_OBJ * 3];
	uint8 InvUse[INV_USE_ATS_MAX * 3];
	uint8 InvUseDef[40 * 3];

	int16 MainMenuY;
	int16 InvDisp;
	int16 DispZx;
	int16 DispZy;
	int16 DispFlag;
	int16 InventY;
	int16 InventSlot[MAX_MOV_OBJ];

	int16 AkInvent;
	bool inv_cur;
	int16 CurBreite;
	int16 CurHoehe;

	RoomMovObjekt room_m_obj[MAX_MOV_OBJ];
	RoomStaticInventar room_s_obj[MAX_FEST_OBJ];
	RoomExit room_e_obj[MAX_EXIT];

	int16 X[MAX_PERSON];
	int16 Y[MAX_PERSON];
	int16 Phase[MAX_PERSON];
	int16 PersonHide[MAX_PERSON];
	int16 PersonRoomNr[MAX_PERSON];
	int16 ZoomXy[MAX_PERSON][2];

	int16 ChewyAni;

	int16 PersonGlobalDia[MAX_PERSON];
	int16 PersonDia[MAX_PERSON];

	int16 PersonDiaRoom[MAX_PERSON];

	int16 PersonDiaTmpRoom[MAX_PERSON];

	int16 DiaAMov;

	int16 scrollx;
	int16 scrolly;
	int16 ScrollxStep;
	int16 ScrollyStep;

	int16 MausSpeed;
	int16 DelaySpeed;
	int16 AadDelay;
	int16 AadSilent;

	uint8 R0FueterLab;
	uint8 R6RaumBetreten;
	uint8 R6BolaJoke;
	uint8 R7BellCount;
	uint8 R11IdCardNr;
	uint8 R12BorkCount;

	uint8 R13MonitorStatus;

	uint8 R17Location;

	int16 R23GleiterExit;

	uint8 R24Hebel[3];

	uint8 R24HebelDir[3];

	uint8 R24KristallLast[3];
	uint8 R25SurimyGo;
	uint8 R27HowardGed;
	uint8 R28PumpTxt1;
	uint8 R28PumpTxt;

	uint8 R31SurimyGo;
	uint8 R33SurimyGo;
	uint8 R33Munter[4];

	int16 R39TvKanal;
	uint8 R39TvRecord;
	uint8 R39ClintNews;

	uint8 R40PoliceAniStatus;
	uint8 R41TrainCount;

	uint8 R47Schloss[3];
	uint8 R48TaxiPerson[MAX_PERSON];

	uint8 R48Auswahl[5];

	uint8 R49BoyAniCount;
	uint8 R51DoorCount;
	uint8 R54LiftCount;
	uint8 R54HowardVorne;

	short R55ExitDia;

	int16 R58TmpRoom;
	int16 R58TmpRoom1;
	int16 R58TmpSx;
	int16 R58TmpSy;
	int16 R58TmpX;
	int16 R58TmpY;
	int16 R64Moni1Ani;

	int16 R64Moni2Ani;

	uint8 mi[8];
	int16 SVal1;
	int16 SVal2;
	int16 SVal3;
	int16 SVal4;
	int16 soundLoopMode;
	bool SoundSwitch;
	uint8 SoundVol;
	bool MusicSwitch;
	uint8 MusicVol;
	bool SpeechSwitch;
	uint8 FramesPerSecond;
	bool DisplayText;
};

struct AutoMov {
	int16 X;
	int16 Y;
};

struct MovPhasen {
	int16 AtsText;
	int16 Phase[4][2];

	int16 Lines;
	int16 Repeat;

	uint8 ZoomFak;
	uint8 Start;
};

struct MovLine {
	int16 EndXyz[3];
	uint8 PhNr;
	uint8 Vorschub;
};

struct AniBlock {
	int16 Nr;
	uint8 Repeat;
	uint8 Dir;

	uint8 Mode;

	uint8 Dummy;
};

struct Flags {
	uint16 StopAutoObj : 1;
	uint16 AniUserAction : 1;
	uint16 AutoAniPlay : 1;
	uint16 StaticUseTxt : 1;
	uint16 GedAction : 1;
	uint16 ChAutoMov : 1;
	uint16 ChAutoMovXy : 1;
	uint16 BreakAMov : 1;

	uint16 AtsAction : 1;
	uint16 AdsAction : 1;
	uint16 AtsText : 1;
	uint16 AdsDialog : 1;
	uint16 ShowAtsInvTxt : 1;
	uint16 MainInput : 1;
	uint16 InventMenu : 1;
	uint16 NoScroll : 1;

	bool InitSound : true;
	uint16 ExitMov : 1;
	uint16 MausTxt : 1;
	uint16 LoadGame : 1;
	uint16 main_maus_flag: 1;
	uint16 ZoomMov : 1;
	uint16 IcmEntry : 1;
	uint16 SaveMenu : 1;

	uint16 SpzAni : 1;
	uint16 MausLinks : 1;
	uint16 ChewyDontGo : 1;
	uint16 NoPalAfterFlc : 1;
	uint16 NoDiaBox : 1;
	uint16 NoEndPosMovObj: 1;
	uint16 NoShad : 1;
	uint16 SavePersonRnr : 1;

	uint16 CursorStatus : 1;
};

} // namespace Chewy

#endif
