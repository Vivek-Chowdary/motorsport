
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
//TODO use iostreams for file management/writing

FILE * LogEngine::logFile = NULL;
LOG_LEVEL LogEngine::globalLevel = LOG_INFO;
int LogEngine::numberOfLogEngines = 0;

LogEngine::LogEngine ( LOG_LEVEL localLevel, const char* name )
{
    //check if the level is correct
    if ( localLevel < 0 || localLevel > MAX_LOG_LEVEL ) return;
    
    //we set the local level of verbosity
    logLevel = localLevel;

    //we set the log name (3 chars)
    for (int i=0; i<LOG_NAME_LENGTH; i++)
    {
        logName[i] = name[i];
    }
    logName[LOG_NAME_LENGTH] = '\0';

    //open the file for writing in rewrite mode if necessary.
    if (!numberOfLogEngines || !logFile)
    {
        if ( ( logFile = fopen ("motorsport.log", "w") ) == NULL ) return;
        put (LOG_INFO, "LogFile created");
    }

    //increase logEngines counter
    numberOfLogEngines++;
    format ( LOG_INFO, "Start of logging for this engine. There's %i engine[s] now.", numberOfLogEngines);

    return;
}

int LogEngine::format ( LOG_LEVEL level, const char *textToLogFormat, ... )
{
    //check if the level is correct
    if ( level < 0 || level > MAX_LOG_LEVEL ) return ( -2 );

    //TODO use strings instead of simple char*
    char buffer[1024];
    va_list arglist;

    //TODO check returning values
    //convert parameters to a string
    va_start ( arglist, textToLogFormat );
#   if defined( _STLPORT_VERSION ) || !defined(WIN32)
        vsnprintf ( buffer, sizeof ( buffer ), textToLogFormat, arglist );
#   else
#       pragma message ("[BUILDMESG] Unsafe buffer semantices used!")
        vsprintf ( buffer, textToLogFormat, arglist );
#   endif
    va_end ( arglist );

    //put the string with a new line
    return ( put ( level, buffer ) );
}

int LogEngine::put ( LOG_LEVEL level, const char *textToLog )
{
    //check if the level is correct
    if ( level < 0 || logLevel > MAX_LOG_LEVEL ) return ( -2 );
    
    //check if we have been told to write this kind of log
    if ( level > globalLevel || level > logLevel ) return ( -1 );
    
    fputc ('(', logFile );
    fputs (logName, logFile );
    
    //write log level information
    switch ( level )
    {
    case LOG_ERROR:
        fputs ( ")(EE): ", logFile );
        break;
    case LOG_WARNING:
        fputs ( ")(WW): ", logFile );
        break;
    case LOG_INFO:
        fputs ( ")(II): ", logFile );
        break;
    case LOG_VERBOSE:
        fputs ( ")(VV): ", logFile );
        break;
    case LOG_TRACE:
        fputs ( ")(TT): ", logFile );
    default:
        break;
    }
    
    //write log text
    fputs ( textToLog, logFile );
    fputc ('\n', logFile );
    fflush ( logFile );
    if ( level == LOG_ERROR ) exit ( 1 );
    return ( 0 );
}

LogEngine::~LogEngine ( )
{
    //decrease number of logEngines
    numberOfLogEngines--;
    format ( LOG_INFO, "End of logging for this engine. There's %i engine[s] now.", numberOfLogEngines);

    if (!numberOfLogEngines)
    {
        put ( LOG_INFO, "Closing logFile" );
        if ( fclose ( logFile ) != 0 ); // return ( -2 );
    }
}

