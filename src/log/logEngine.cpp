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

#include "logEngine.hpp"

int LogEngine::start (LOG_LEVEL level, const char* filePath, bool appendMode)
{
    //check if we append (or rewrite the whole file)
    logFile = fopen (filePath, (appendMode ? "a" : "w"));
    
    //we set the level of verbosity
    logLevel = level;
    
    //we put some text at the start of the current log
    put (LOG_INFO, "Start of LogFile");

    return (0);
}

int LogEngine::format (LOG_LEVEL level, const char *textToLogFormat, ...)
{
    char buffer[1024];
    va_list arglist;

    //convert parameters to a string
    va_start (arglist, textToLogFormat);
#if defined( _STLPORT_VERSION ) || !defined(WIN32)
    vsnprintf (buffer, sizeof(buffer), textToLogFormat, arglist);
#else
#   pragma message ("[BUILDMESG] Unsafe buffer semantices used!")
    vsprintf (buffer, textToLogFormat, arglist);
#endif
    va_end (arglist);
    
    //put the string with a new line
    return (put (level, buffer));
}


int LogEngine::put (LOG_LEVEL level, const char* textToLog, bool useNewLine)
{ 
    //check if we have been told to write this kind of log
    if (level <= logLevel) {
        //check if we should append or create a new line
        if (!useNewLine) {
            // separate previous log with a blank space
            fputc (' ', logFile);
        } else {
            //write log level information
            switch (level) {
                case LOG_ERROR:   fputs ("\n(EE): ", logFile); break;
                case LOG_WARNING: fputs ("\n(WW): ", logFile); break;
                default:          fputs ("\n(II): ", logFile); break;
            }
        }
        //write log text
        fputs (textToLog, logFile);
        fflush (logFile);
        return (0);
    }
    return (-1);
}

int LogEngine::append (LOG_LEVEL level, const char *textToLog)
{
    return put(level, textToLog, false /* don't useNewLine */);
}

int LogEngine::stop (void)
{
    put (LOG_INFO, "End of LogFile");
    fclose (logFile);

    return (0);
}
