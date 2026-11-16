/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

////////////////////////////////////////////////////////////////////////////////
//																																						//
//  (c) 2001-2003 Electronic Arts Inc.																				//
//																																						//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
// FILE: FileTransfer.cpp
// Author: Matthew D. Campbell, December 2002
// Description: File Transfer wrapper using TheNetwork
///////////////////////////////////////////////////////////////////////////////////////

#include "PreRTS.h"	// This must go first in EVERY cpp file in the GameEngine

#include "Common/FileSystem.h"
#include "Common/GlobalData.h"
#include "GameClient/LoadScreen.h"
#include "GameClient/Shell.h"
#include "GameNetwork/FileTransfer.h"
#include "GameNetwork/networkutil.h"

#include <stdarg.h>
#include <time.h>

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

void MapTransferLog(const char *format, ...)
{
	char message[1024];
	va_list args;
	va_start(args, format);
	vsnprintf(message, sizeof(message), format, args);
	va_end(args);

	char stamp[32];
	time_t now = time(nullptr);
	strftime(stamp, sizeof(stamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

	fprintf(stderr, "[MapTransfer] %s\n", message);
	fflush(stderr);

	if (TheGlobalData == nullptr)
		return;

	// start over once per session when the log has grown large
	static Bool checkedSize = FALSE;
	AsciiString logPath = TheGlobalData->getPath_UserData();
	logPath.concat("MapTransfer.log");
	const char *mode = "a";
	if (!checkedSize)
	{
		checkedSize = TRUE;
		FILE *existing = fopen(logPath.str(), "rb");
		if (existing)
		{
			fseek(existing, 0, SEEK_END);
			if (ftell(existing) > 512 * 1024)
				mode = "w";
			fclose(existing);
		}
	}

	FILE *log = fopen(logPath.str(), mode);
	if (log)
	{
		fprintf(log, "%s %s\n", stamp, message);
		fclose(log);
	}
}

static Bool doFileTransfer( AsciiString filename, MapTransferLoadScreen *ls, Int mask )
{
	Bool fileTransferDone = FALSE;
	Int fileTransferPercent = 0;
	Int i;

	if (mask)
	{
		ls->setCurrentFilename(filename);
		UnsignedInt startTime = timeGetTime();
		const Int timeoutPeriod = 2*60*1000;
		ls->processTimeout(timeoutPeriod/1000);

		ls->update(0);
		fileTransferDone = FALSE;
		fileTransferPercent = 0;

		UnsignedShort fileCommandID = 0;
		Bool sentFile = FALSE;
		if (TheGameInfo->amIHost())
		{
			Sleep(500);
			fileCommandID = TheNetwork->sendFileAnnounce(filename, mask);
		}
		else
		{
			sentFile = TRUE;
		}

		DEBUG_LOG(("Starting file transfer loop"));
		MapTransferLog("Transfer of '%s' to mask %X started, %s", filename.str(), mask,
			TheGameInfo->amIHost() ? "sending" : "receiving");
		Int lastSlotPercent[MAX_SLOTS];
		for (i=0; i<MAX_SLOTS; ++i)
			lastSlotPercent[i] = -1;

		while (!fileTransferDone)
		{
			if (!sentFile && TheNetwork->areAllQueuesEmpty())
			{
				TheNetwork->sendFile(filename, mask, fileCommandID);
				sentFile = TRUE;
			}

			// get the progress for each player, and take the min for our overall progress
			fileTransferDone = TRUE;
			fileTransferPercent = 100;
			for (i=1; i<MAX_SLOTS; ++i)
			{
				if (TheGameInfo->getConstSlot(i)->isHuman() && !TheGameInfo->getConstSlot(i)->hasMap())
				{
					Int slotTransferPercent = TheNetwork->getFileTransferProgress(i, filename);
					fileTransferPercent = min(fileTransferPercent, slotTransferPercent);
					if (slotTransferPercent != lastSlotPercent[i])
					{
						MapTransferLog("Slot %d is at %d%% for '%s'", i, slotTransferPercent, filename.str());
						lastSlotPercent[i] = slotTransferPercent;
					}

					if (slotTransferPercent == 0)
						ls->processProgress(i, slotTransferPercent, "MapTransfer:Preparing");
					else if (slotTransferPercent < 100)
						ls->processProgress(i, slotTransferPercent, "MapTransfer:Recieving");
					else
						ls->processProgress(i, slotTransferPercent, "MapTransfer:Done");
				}
			}
			if (fileTransferPercent < 100)
			{
				fileTransferDone = FALSE;
				if (fileTransferPercent == 0)
					ls->processProgress(0, fileTransferPercent, "MapTransfer:Preparing");
				else
					ls->processProgress(0, fileTransferPercent, "MapTransfer:Sending");
			}
			else
			{
				DEBUG_LOG(("File transfer is 100%%!"));
				MapTransferLog("Transfer of '%s' done", filename.str());
				ls->processProgress(0, fileTransferPercent, "MapTransfer:Done");
			}

			Int now = timeGetTime();
			if (now > startTime + timeoutPeriod) // bail if we don't finish in a reasonable amount of time
			{
				DEBUG_LOG(("Timing out file transfer"));
				MapTransferLog("Transfer of '%s' timed out at %d%%", filename.str(), fileTransferPercent);
				break;
			}
			else
			{
				ls->processTimeout((startTime + timeoutPeriod - now)/1000);
			}

			ls->update(fileTransferPercent);
		}

		if (!fileTransferDone)
		{
			return FALSE;
		}
	}

	return TRUE;
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

AsciiString GetBasePathFromPath( AsciiString path )
{
	const char *s = getLastPathSeparator(path.str());
	if (s)
	{
		return AsciiString(path.str(), s - path.str());
	}
	return AsciiString::TheEmptyString;
}

AsciiString GetBaseFileFromFile( AsciiString fname )
{
	const char *s = getExtension(fname.str());
	if (s)
	{
		return AsciiString(fname.str(), s - fname.str());
	}
	return AsciiString::TheEmptyString;
}

static AsciiString GetFileInMapDirectory( const AsciiString &mapPath, const AsciiString &filename )
{
	const char *file = getFileName(mapPath.str());
	AsciiString path;
	path.set(mapPath.str(), file - mapPath.str());
	path.concat(filename);
	return path;
}

AsciiString GetPreviewFromMap( AsciiString path )
{
	FileSystem::removeExtension(path);
	path.concat(".tga");
	return path;
}

AsciiString GetINIFromMap( const AsciiString &path )
{
	return GetFileInMapDirectory(path, "map.ini");
}

AsciiString GetStrFileFromMap( const AsciiString &path )
{
	return GetFileInMapDirectory(path, "map.str");
}

AsciiString GetSoloINIFromMap( const AsciiString &path )
{
	return GetFileInMapDirectory(path, "solo.ini");
}

AsciiString GetAssetUsageFromMap( const AsciiString &path )
{
	return GetFileInMapDirectory(path, "assetusage.txt");
}

AsciiString GetReadmeFromMap( const AsciiString &path )
{
	return GetFileInMapDirectory(path, "readme.txt");
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

Bool DoAnyMapTransfers(GameInfo *game)
{
	TheGameInfo = game;
	Int mask = 0;
	Int i=0;
	for (i=1; i<MAX_SLOTS; ++i)
	{
		if (TheGameInfo->getConstSlot(i)->isHuman() && !TheGameInfo->getConstSlot(i)->hasMap())
		{
			DEBUG_LOG(("Adding player %d to transfer mask", i));
			mask |= (1<<i);
		}
	}
	if (!mask)
		return TRUE;

	MapTransferLog("Map '%s' needs a transfer to mask %X (contents %X, local slot %d%s)",
		game->getMap().str(), mask, game->getMapContentsMask(), game->getLocalSlotNum(),
		game->amIHost() ? ", host" : "");

	TheShell->hideShell();
	MapTransferLoadScreen *ls = NEW MapTransferLoadScreen;
	ls->init(TheGameInfo);
	Bool ok = TRUE;
	if (TheGameInfo->getMapContentsMask() & 2)
		ok = doFileTransfer(GetPreviewFromMap(game->getMap()), ls, mask);
	if (ok && TheGameInfo->getMapContentsMask() & 4)
		ok = doFileTransfer(GetINIFromMap(game->getMap()), ls, mask);
	if (ok && TheGameInfo->getMapContentsMask() & 8)
		ok = doFileTransfer(GetStrFileFromMap(game->getMap()), ls, mask);
	if (ok && TheGameInfo->getMapContentsMask() & 16)
		ok = doFileTransfer(GetSoloINIFromMap(game->getMap()), ls, mask);
	if (ok && TheGameInfo->getMapContentsMask() & 32)
		ok = doFileTransfer(GetAssetUsageFromMap(game->getMap()), ls, mask);
	if (ok && TheGameInfo->getMapContentsMask() & 64)
		ok = doFileTransfer(GetReadmeFromMap(game->getMap()), ls, mask);
	if (ok)
		ok = doFileTransfer(game->getMap(), ls, mask);
	delete ls;
	ls = nullptr;
	MapTransferLog("Map transfer %s", ok ? "succeeded" : "FAILED");
	if (!ok)
		TheShell->showShell();
	return ok;
}
