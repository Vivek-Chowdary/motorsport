/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "logEngine.hpp"
#include "guiEngine.hpp"
#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <xmlParser.hpp>
#include "system.hpp"

//TODO use iostreams for file management/writing

std::fstream LogEngine::logFile;
LOG_LEVEL LogEngine::globalLevel = LOG_ENDUSER;
int LogEngine::numberOfLogEngines = 0;
int LogEngine::textBuffer = 128;

LogEngine::LogEngine (LOG_LEVEL localLevel, const std::string & name):logLevel (localLevel), logName (name)
{
    // we set the log name (3 chars, extended with space if needed)
    logName.resize (3, ' ');

    // open the file for writing in rewrite mode if necessary.
    if ((numberOfLogEngines == 0) || (!logFile.is_open ()))
    {
        std::cout << "Creating first LogEngine instance (" << name << "). Reading LogEngine config file..." << std::endl;
#ifdef MACOSX
        XmlFile *xmlFile = new XmlFile ("motorsport.app/Contents/Resources/logConfig.xml");
#else
        XmlFile *xmlFile = new XmlFile ("logConfig.xml");
#endif
        processXmlRootNode (xmlFile->getRootNode ());
        delete xmlFile;
    }
    // increase logEngines counter
    numberOfLogEngines++;
    format (LOG_DEVELOPER, "Start of logging for this engine. There's %i log engine[s] now.", numberOfLogEngines);

    return;
}

void LogEngine::loadscreen (LOG_LEVEL level, const char *textToLogFormat, ...)
{
    // check now so that we don't incur any unnecessary formatting expense
    if (level > globalLevel || level > logLevel)
        return;

    char buffer[1024];
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

    char buffer[1024];
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

    char buffer[1024];
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

void LogEngine::put (LOG_LEVEL level, const char *textToLog)
{
    log(level, LOG_FILE, textToLog);
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
    logLine << '(' << logName << ")(" << GetLogLevelCode (level) << "): ";

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
            GuiEngine::getGuiEnginePointer ()->addTelemetryLine (buffer);
            logFile << logLine.str () << "\n";
            logFile.flush ();
        }
        if (mask & LOG_LOADSCREEN)
        {
            GuiEngine::getGuiEnginePointer ()->addLoadscreenLine (buffer);
        }
    } else {
    if (mask & LOG_TELEMETRY)
    {
        GuiEngine::getGuiEnginePointer ()->addTelemetryLine (buffer);
    }
    if (mask & LOG_CONSOLE)
    {
        std::cerr << logLine.str () << std::endl;;
    }
    if (mask & LOG_LOADSCREEN)
    {
        GuiEngine::getGuiEnginePointer ()->addLoadscreenLine (std::string(buffer));
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

LogEngine::~LogEngine ()
{
    // decrease number of logEngines
    numberOfLogEngines--;
    format (LOG_DEVELOPER, "End of logging for this engine. There's %i log engine[s] left now.", numberOfLogEngines);

    if (numberOfLogEngines == 0)
    {
        put (LOG_ENDUSER, "Closing logFile");
        logFile.close ();
    }

}

void LogEngine::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    std::string fileName = "motorsport-default.log";
    globalLevel = LOG_DEVELOPER;
    std::cout << "Reading LogEngine configuration..." << std::endl;
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string name;
            assignXmlString (name, n->getNodeName ());
            if (name == "logConfig")
            {
                if (n->hasAttributes ())
                {
                    // get all the attributes of the node
                    DOMNamedNodeMap *attList = n->getAttributes ();
                    int nSize = attList->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                        std::string attribute;
                        assignXmlString (attribute, attNode->getName ());
                        if (attribute == "globalLevel")
                        {
                            assignXmlString (attribute, attNode->getValue ());
                            globalLevel = stologlevel (attribute);
                        }
                        if (attribute == "fileName")
                        {
                            assignXmlString (fileName, attNode->getValue ());
                        }
                        if (attribute == "textBuffer")
                        {
                            assignXmlString (attribute, attNode->getValue ());
                            textBuffer = stoi (attribute);
                        }
                    }
                }
            }
        }
    } else {
        std::cerr << "ERROR: could not read LogEngine configuration values! Using default values." << std::endl;
    }
    std::cout << "Log messages will all be recorded in: " << fileName << std::endl;
    logFile.open (fileName.c_str (), std::fstream::out);
    if (!logFile.good ())
    {
        std::cerr << "ERROR: Logfile (" << fileName << ") could not be opened!\n";
        return;
    }
    put (LOG_ENDUSER, "LogFile created");
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
