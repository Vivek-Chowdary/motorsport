/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "logEngine.hpp"
#include "gui.hpp"
#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <xmlTag.hpp>

//TODO use iostreams for file management/writing

std::fstream LogEngine::logFile;
LOG_LEVEL LogEngine::globalLevel = LOG_ENDUSER;
int LogEngine::textBuffer = 128;

unsigned int LogEngine::instancesCount = 0;
LogEngines LogEngine::logEngines;

void LogEngine::deleteAll()
{
    while( !logEngines.empty() )
    {
        logEngines.erase( logEngines.begin() );
    }
}
pLogEngine LogEngine::create(LOG_LEVEL localLevel, const std::string & name)
{
    // open the file for writing in rewrite mode if necessary.
    if ((instancesCount == 0) || (!logFile.is_open ()))
    {
        std::cout << "Starting log engine...";
#ifdef MACOSX
        XmlTag * tag = new XmlTag ("motorsport.app/Contents/Resources/logConfig.xml", false);
#else
        XmlTag * tag = new XmlTag ("../cfg/logConfig.xml", false);
#endif
        std::string fileName;
        globalLevel = LOG_DEVELOPER;
        if (tag->getName() == "logConfig")
        {
            globalLevel = stologlevel(tag->getAttribute("globalLevel"));
            fileName = tag->getAttribute("fileName");
            textBuffer = stoi(tag->getAttribute("textBuffer"));
        } else {
            std::cerr << std::endl << "ERROR: Could not read Log Engine configuration. Using default values!" << std::endl;
            fileName = "motorsport-default.log";
        }
        std::cout << " [Ok : \"" << fileName << "\"]" << std::endl;
        logFile.open (fileName.c_str (), std::fstream::out);
        if (!logFile.good ())
        {
            std::cerr << "ERROR: Log file \"" << fileName << "\" could not be opened!\n";
            //FIXME: exit program?
            //return;
        }
        delete tag;
    }
    pLogEngine log (new LogEngine(localLevel, name));
    // increase logEngines counter
    instancesCount++;
    const int numberStringSize = 64;
    char numberString[numberStringSize];
    snprintf (numberString, numberStringSize, "%i", instancesCount);
    logEngines[std::string("LOG#") + numberString + "#." + name] = log;

    return log;
}
LogEngine::LogEngine (LOG_LEVEL localLevel, const std::string & name):logLevel (localLevel), logName (name)
{
    if ((instancesCount == 0) || (!logFile.is_open ()))
    {
        format (LOG_ENDUSER, "Start of logs.");
    }
    return;
}
LogEngine::~LogEngine ()
{
    LogEnginesIt i = logEngines.begin();
    LogEnginesIt d;
    for(;i != logEngines.end(); i++)
    {
        if (this == i->second.get())
        {
            d = i;
            //d->second->format(LOG_ENDUSER, "I'm gonna be deleted! :D");
        }
        else
        {
            //i->second->format(LOG_ENDUSER, "I'm still left! :D");
        }
    }
    logEngines.erase(d);
    //d->second.reset();
    //Check if this is the last log engine in the map.If it is, since we're gonna delete it, we close the log file.
    if (logEngines.size() <= 1)
    {
        format (LOG_ENDUSER, "Closing logFile");
        logFile.close ();
    }
    //format (LOG_ENDUSER, "--------------------------LogFile size: %i", logEngines.size());
}

void LogEngine::logAll()
{
    LogEnginesIt i = logEngines.begin();
    for(;i != logEngines.end(); i++)
    {
        i->second->__format(LOG_DEVELOPER, "LogEngine Id: %s\t References #: %i", i->first.c_str(), i->second.use_count());
    }
}

void LogEngine::loadscreen (LOG_LEVEL level, const char *textToLogFormat, ...)
{
    // check now so that we don't incur any unnecessary formatting expense
    if (level > globalLevel || level > logLevel)
        return;

    char buffer[2048];
    va_list arglist;
    va_start (arglist, textToLogFormat);
#if defined( _STLPORT_VERSION ) || !defined(WIN32)
    vsnprintf (buffer, sizeof (buffer), textToLogFormat, arglist);
#else
    vsprintf (buffer, textToLogFormat, arglist);
#endif
    va_end (arglist);
    log (level, LOG_FILE | LOG_LOADSCREEN, buffer);
}

void LogEngine::telemetry (LOG_LEVEL level, const char *textToLogFormat, ...)
{
    // check now so that we don't incur any unnecessary formatting expense
    if (level > globalLevel || level > logLevel)
        return;

    char buffer[2048];
    va_list arglist;
    va_start (arglist, textToLogFormat);
#if defined( _STLPORT_VERSION ) || !defined(WIN32)
    vsnprintf (buffer, sizeof (buffer), textToLogFormat, arglist);
#else
    vsprintf (buffer, textToLogFormat, arglist);
#endif
    va_end (arglist);
    log (level, LOG_CONSOLE | LOG_TELEMETRY, buffer);
}

void LogEngine::format (LOG_LEVEL level, const char *textToLogFormat, ...)
{
    // check now so that we don't incur any unnecessary formatting expense
    if (level > globalLevel || level > logLevel)
        return;

    char buffer[2048];
    va_list arglist;
    va_start (arglist, textToLogFormat);
#if defined( _STLPORT_VERSION ) || !defined(WIN32)
    vsnprintf (buffer, sizeof (buffer), textToLogFormat, arglist);
#else
    vsprintf (buffer, textToLogFormat, arglist);
#endif
    va_end (arglist);
    log (level, LOG_FILE, buffer);
}

const std::string LogEngine::GetLogLevelCode (LOG_LEVEL level)
{
    switch (level)
    {
    case LOG_ERROR:
        return "EE";
    case LOG_WARNING:
        return "WW";
    case LOG_ENDUSER:
        return "UU";
    case LOG_CCREATOR:
        return "CC";
    case LOG_DEVELOPER:
        return "DD";
    default:
        return "  ";
    }
}


void LogEngine::log (const LOG_LEVEL level, const int mask, const char *textToLogFormat, ...)
{
    // LOG_LEVEL level = config.level;
    // LOG_MASK mask = config.mask;

    // apply filter
    if (level > globalLevel || level > logLevel)
        return;

    // create line, add header
    std::stringstream logLine;
    logLine << GetLogLevelCode (level) << " " << logName << " ";

    // format string
    char buffer[2048];
    va_list arglist;
    va_start (arglist, textToLogFormat);
#if defined( _STLPORT_VERSION ) || !defined(WIN32)
    vsnprintf (buffer, sizeof (buffer), textToLogFormat, arglist);
#else
    vsprintf (buffer, textToLogFormat, arglist);
#endif
    va_end (arglist);
    logLine << buffer;

    // send to recievers
    const bool telemetryOnly = false; // only modifiable at compile time
    if (telemetryOnly)
    {
        if (mask & LOG_TELEMETRY)
        {
            Gui::get()->addTelemetryLine (buffer);
            logFile << logLine.str () << "\n";
            logFile.flush ();
        }
        if (mask & LOG_LOADSCREEN)
        {
            Gui::get()->addLoadscreenLine (buffer);
        }
    } else {
    if (mask & LOG_TELEMETRY)
    {
        Gui::get()->addTelemetryLine (buffer);
    }
    if (mask & LOG_CONSOLE)
    {
        std::cerr << logLine.str () << std::endl;;
    }
    if (mask & LOG_LOADSCREEN)
    {
        Gui::get()->addLoadscreenLine (std::string(buffer));
    }
    if ((mask & LOG_FILE))
    {
        logFile << logLine.str () << "\n";
        logFile.flush ();
    }
    }
    // exit after logging an error
    if (level == LOG_ERROR)
    {
        exit (1);
    }
}

void LogEngine::setName(std::string name)
{
    this->logName = name;
}

LOG_LEVEL stologlevel (const std::string & srcString)
{
    if (srcString == "LOG_ERROR")
        return LOG_ERROR;
    if (srcString == "LOG_WARNING")
        return LOG_WARNING;
    if (srcString == "LOG_ENDUSER")
        return LOG_ENDUSER;
    if (srcString == "LOG_CCREATOR")
        return LOG_CCREATOR;
    if (srcString == "LOG_DEVELOPER")
        return LOG_DEVELOPER;
    return LOG_DEVELOPER;
}
