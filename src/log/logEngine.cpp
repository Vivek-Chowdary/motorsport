
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
    //open the file for writing in rewrite mode if necessary.
    if (!numberOfLogEngines || !logFile)
    {
        if ( ( logFile = fopen ("motorsport.log", "w") ) == NULL ) return;// return ( -1 );
    }
    
    //check if the level is correct
    if ( localLevel < 0 || localLevel > MAX_LOG_LEVEL ); // return ( -2 );
    
    //we set the global level of verbosity
    initialize (localLevel, name);
}

int LogEngine::initialize ( LOG_LEVEL level, const char *name)
{
    //increase logEngines counter
    numberOfLogEngines++;
    format ( LOG_INFO, "Number of logEngines created so far: %i.", numberOfLogEngines);
    
    //check if the level is correct
    if ( level < 0 || level > MAX_LOG_LEVEL ) return ( -2 );
        
    //we set the level of verbosity
    logLevel = level;

    //we set the log name (3 chars)
    for (int i=0; i<LOG_NAME_LENGTH; i++)
    {
        logName[i] = name[i];
    }
    logName[LOG_NAME_LENGTH] = '\0';

    //we put some text at the start of the current log
    //TODO check returning value
    put ( LOG_INFO, "Start of log file" );

    return ( 1 );
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
#if defined( _STLPORT_VERSION ) || !defined(WIN32)
    vsnprintf ( buffer, sizeof ( buffer ), textToLogFormat, arglist );
#else
#    pragma message ("[BUILDMESG] Unsafe buffer semantices used!")
    vsprintf ( buffer, textToLogFormat, arglist );
#endif
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
    
    fputs ("\n(", logFile );
    fputs (logName, logFile );
    
    //write log level information
    switch ( level )
    {
    case LOG_ERROR:
        if ( fputs ( ")(EE): ", logFile ) == EOF ) return ( -3 );
        break;
    case LOG_WARNING:
        if ( fputs ( ")(WW): ", logFile ) == EOF ) return ( -3 );
        break;
    default:
        if ( fputs ( ")(II): ", logFile ) == EOF ) return ( -3 );
        break;
    }
    
    //write log text
    if ( fputs ( textToLog, logFile ) == EOF ) return ( -3 );
    if ( fflush ( logFile ) != 0) return ( -3 );
    return ( 0 );
}

LogEngine::~LogEngine ( )
{
    put ( LOG_INFO, "End of logging for this engine" );
    //decrease number of logEngines
    numberOfLogEngines--;
    format ( LOG_INFO, "Number of logEngines left: %i.", numberOfLogEngines);

    if (!numberOfLogEngines)
    {
        put ( LOG_INFO, "Closing logFile" );
        if ( fclose ( logFile ) != 0 ); // return ( -2 );
    }
}

