
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
    if ((numberOfLogEngines == 0) || (!logFile.is_open ()))
    {
        std::cout << "Creating first LogEngine instance (" << name << "). Reading LogEngine config file..." << std::endl;
        XmlFile * xmlFile = new XmlFile ("logConfig.xml");
        processXmlRootNode (xmlFile->getRootNode());
        delete xmlFile;
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
//#    pragma message ("[BUILDMESG] Unsafe buffer semantices used!")
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

void LogEngine::processXmlRootNode (DOMNode * n)
{
    std::string fileName = "motorsport-default.log";
    globalLevel = LOG_TRACE;
    std::cout << "Reading LogEngine configuration..." << std::endl;
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string name;
            assignXmlString (name, n->getNodeName());
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
                        assignXmlString (attribute, attNode->getName());
                        if (attribute == "globalLevel")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            globalLevel = stologlevel (attribute);
                        }
                        if (attribute == "fileName")
                        {
                            fileName.clear();
                            assignXmlString (fileName, attNode->getValue());
                        }
                        if (attribute == "textBuffer")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            textBuffer = stoi (attribute);
                        }
                        attribute.clear();
                    }
                }
            }
            name.clear();
        }
    } else {
        std::cerr << "ERROR: could not read LogEngine configuration values! Using default values." << std::endl;
    }
    std::cout << "Log messages will all be recorded in: " << fileName << std::endl;
    logFile.open (fileName.c_str(), std::fstream::out);
    if (!logFile.good ())
    {
        std::cerr << "ERROR: Logfile (" << fileName << ") could not be opened!\n";
        return;
    }
    put (LOG_INFO, "LogFile created");
    fileName.clear();
}

LOG_LEVEL stologlevel (const std::string &srcString)
{
    if (srcString == "LOG_ERROR")
        return LOG_ERROR;
    if (srcString == "LOG_WARNING")
        return LOG_WARNING;
    if (srcString == "LOG_INFO")
        return LOG_INFO;
    if (srcString == "LOG_VERBOSE")
        return LOG_VERBOSE;
    if (srcString == "LOG_TRACE")
        return LOG_TRACE;
    return LOG_TRACE;
}
