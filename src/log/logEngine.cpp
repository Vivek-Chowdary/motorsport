
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

std::fstream LogEngine::logFile;
LOG_LEVEL LogEngine::globalLevel = LOG_INFO;
int LogEngine::numberOfLogEngines = 0;
int LogEngine::textBuffer = 128;

LogEngine::LogEngine (LOG_LEVEL localLevel, const char *name):logName (name)
{
    // we set the local level of verbosity
    logLevel = localLevel;

    // we set the log name (3 chars, extended with space if needed)
    logName.resize (3, ' ');

    // open the file for writing in rewrite mode if necessary.

    if (!numberOfLogEngines || !logFile.is_open ())
    {
        LogData *data = new LogData;
        data->log = this;

        processXmlFile ("logConfig.xml", &LogEngine::processLogConfigFile, data);
        logFile.open (data->fileName, std::fstream::out);
        if (!logFile.good ())
        {
            std::cerr << "Error: Logfile could not be opened.\n";
            return;
        }
        put (LOG_INFO, "Temporary parsing data already loaded into memory...");
        put (LOG_INFO, "LogFile created");
        put (LOG_INFO, "Setting global log level...");
        globalLevel = data->globalLevel;

        put (LOG_INFO, "Setting log format() method text buffer length...");
        textBuffer = data->textBuffer;

        put (LOG_INFO, "Unloading temporary parsing data from memory...");
        delete[](data->fileName);
        delete data;
    }
    // increase logEngines counter
    numberOfLogEngines++;
    format (LOG_TRACE, "Start of logging for this engine. There's %i log engine[s] now.", numberOfLogEngines);

    return;
}

int LogEngine::format (LOG_LEVEL level, const char *textToLogFormat, ...)
{
    // TODO use strings instead of simple char*
    char buffer[1024];
    va_list arglist;

    // TODO check returning values
    // convert parameters to a string
    va_start (arglist, textToLogFormat);
#if defined( _STLPORT_VERSION ) || !defined(WIN32)
    vsnprintf (buffer, sizeof (buffer), textToLogFormat, arglist);
#else
#    pragma message ("[BUILDMESG] Unsafe buffer semantices used!")
    vsprintf (buffer, textToLogFormat, arglist);
#endif
    va_end (arglist);

    // put the string with a new line
    return (put (level, buffer));
}

const char *LogEngine::GetLogLevelCode (LOG_LEVEL level)
{
    switch (level)
    {
    case LOG_ERROR:
        return "EE";
    case LOG_WARNING:
        return "WW";
    case LOG_INFO:
        return "II";
    case LOG_VERBOSE:
        return "VV";
    case LOG_TRACE:
        return "TT";
    default:
        return "  ";
    }
}

int LogEngine::put (LOG_LEVEL level, const char *textToLog)
{
    // check if we have been told to write this kind of log
    if (level > globalLevel || level > logLevel)
        return (-1);

    // write line header
    logFile << '(' << logName << ")(" << GetLogLevelCode (level) << "): ";

    // write log text
    logFile << textToLog << "\n";
    logFile.flush ();

    if (level == LOG_ERROR)
    {
        exit (1);
    }
    return (0);
}

LogEngine::~LogEngine ()
{
    // decrease number of logEngines
    numberOfLogEngines--;
    format (LOG_TRACE, "End of logging for this engine. There's %i log engine[s] left now.", numberOfLogEngines);

    if (numberOfLogEngines == 0)
    {
        put (LOG_INFO, "Closing logFile");
        logFile.close ();
    }

}
