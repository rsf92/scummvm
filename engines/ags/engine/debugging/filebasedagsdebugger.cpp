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

#include <cstdio>
#include "debug/filebasedagsdebugger.h"
#include "ac/file.h"                    // filelength()
#include "util/stream.h"
#include "util/textstreamwriter.h"
#include "util/wgt2allg.h"              // exists()
#include "platform/base/agsplatformdriver.h"

using AGS::Common::Stream;
using AGS::Common::TextStreamWriter;

const char* SENT_MESSAGE_FILE_NAME = "dbgrecv.tmp";

bool FileBasedAGSDebugger::Initialize()
{
    if (exists(SENT_MESSAGE_FILE_NAME))
    {
        ::remove(SENT_MESSAGE_FILE_NAME);
    }
    return true;
}

void FileBasedAGSDebugger::Shutdown()
{
}

bool FileBasedAGSDebugger::SendMessageToEditor(const char *message) 
{
    while (exists(SENT_MESSAGE_FILE_NAME))
    {
        platform->YieldCPU();
    }

    Stream *out = Common::File::CreateFile(SENT_MESSAGE_FILE_NAME);
    // CHECKME: originally the file was opened as "wb" for some reason,
    // which means the message should be written as a binary array;
    // or shouldn't it?
    out->Write(message, strlen(message));
    delete out;
    return true;
}

bool FileBasedAGSDebugger::IsMessageAvailable()
{
    return (exists("dbgsend.tmp") != 0);
}

char* FileBasedAGSDebugger::GetNextMessage()
{
    Stream *in = Common::File::OpenFileRead("dbgsend.tmp");
    if (in == nullptr)
    {
        // check again, because the editor might have deleted the file in the meantime
        return nullptr;
    }
    int fileSize = in->GetLength();
    char *msg = (char*)malloc(fileSize + 1);
    in->Read(msg, fileSize);
    delete in;
    ::remove("dbgsend.tmp");
    msg[fileSize] = 0;
    return msg;
}
