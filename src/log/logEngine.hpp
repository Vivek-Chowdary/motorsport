/******************************************************************************
*
* Copyright (C) 2004 Bruno González Campo (stenyak@users.sourceforge.net)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
******************************************************************************/

#ifndef LOG_ENGINE_HPP
#define LOG_ENGINE_HPP

/******************************************************************************
*
* Includes
*
******************************************************************************/

#include <cstdio>
#include <cstdarg>
#include <ctime>

/******************************************************************************
*
* Classes
*
******************************************************************************/
enum LOG_LEVEL { LOG_ERROR = 0, LOG_WARNING = 1, LOG_INFO = 2, 
     LOG_VERBOSE = 3, LOG_TRACE = 4 };


class LogEngine
{
    FILE *logFile;
    LOG_LEVEL logLevel;
    public:
        //starts the log process, specifying the level of verbosity
        int start (LOG_LEVEL level, const char* filePath, bool appendMode=false);
        //writes textToLog to the .logFile
        int put (LOG_LEVEL level, const char* textToLog, bool useNewLine=true);
        //writes text with the level + printf-like format string (appending line by default)
        int format (LOG_LEVEL level, const char *textToLogFormat, ...);
        //same as put - but appends the data to the same line
        int append (LOG_LEVEL level, const char* textToLog);
        //stops the log process
        int stop (void);
};

#endif
