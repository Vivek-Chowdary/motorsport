/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef LOG_ENGINE_HPP
#   define LOG_ENGINE_HPP
#   include <string>
#   include <fstream>
#   include "xercesc_fwd.hpp"
#define __format(X ,Y, ...) format((X), std::string(std::string(__FILE__) + ":%i: " + std::string((Y))).c_str(), __LINE__, ##__VA_ARGS__)

//forward declarations
class GuiEngine;

/// It's used to indicate the level of verbosity in a log.
/** It indicates the level of verbosity of a log. The lower the number is, the more important the messages are.
*/
enum LOG_LEVEL
{
    LOG_ERROR = 0,              // !< Can't continue, must be fixed.
    LOG_WARNING = 1,            // !< Can continue, should be fixed.
    LOG_ENDUSER = 2,            // !< Info for the end-user, for configuration and program (bin) debugging purporses
    LOG_CCREATOR = 3,           // !< Info for the contents-creator, for contents (data) debugging purposes
    LOG_DEVELOPER = 4           // !< Info for the developer, for code (src) debugging purposes
};

enum LOG_MASK
{
    LOG_FILE = 1,              // !< To standard log file
    LOG_TELEMETRY = 2,         // !< To telemetry window
    LOG_LOADSCREEN = 4,        // !< To initial load screen
    LOG_CONSOLE = 8,           // !< Send to console
    LOG_FILE_MUTE = 16         // !< Mute default log file output
    //    LOG_REPLAY = 32,     // !< Merged into replay file
};

struct LogConfig
{
  LOG_LEVEL level;    // !< How important is this log
  LOG_MASK mask;      // !< Where should we send the log
};

/// Number of characters for the log instance name.
/** Number of characters for the log instance name. Those 3 characters (real characters, not counting '\0') can be read in the log file as a way to identify the originator engine.
*/
#define LOG_NAME_LENGTH 3

/// Allows to automate the recording of log messages to a file.
/** Allows to automate the recording of log messages to a plain-text file. Every log engine has its own level of verbosity, meaning it can display only messages of a certain level of importance (discarding the less important messages).
*/
class LogEngine
{
    private:
    // / Indicates how much information will be stored in this log instance.
    /** Indicates the level of verbosity of the log. All log messages with a number higher to this one will be discarded, and therefore not writen to the log file.
    */
    LOG_LEVEL logLevel;
    // / Indicates a short name, prefixed to every log line.
    /** Indicates a short name or initials for instance of the log engine. It allows to know which "engine" this log is associated with.
    */
    std::string logName;
    // / Indicates the file where logs will be writen.
    /** Indicates a plain-text file where log messages can be writen.
    */
    static std::fstream logFile;
    // / Indicates how much information will be stored in the log file.
    /** Indiactes how much information will be stored in the log file, regardless of a higher level for current instance.
    */
    static LOG_LEVEL globalLevel;
    // / Stores the number of log engine instances.
    /** Stores the number of log engine instances that have been created. This number is increased or decreased everytime a log engine is created or deleted.
    */
    static int numberOfLogEngines;
    // / Amount of chars allocated for log message conversion.
    /** Indicates how many chars will be allocated for performing the conversion between the original printf-like format, to the final char* text.
    */
    static int textBuffer;
    // / Helper function translating log level into a short code.
    /** Helper function translating log level into a short code. 
	@param verbosity level to get code for.
	@return code as a short c-string.
    */
    const std::string GetLogLevelCode (LOG_LEVEL level);
    // / Called by the generic XML parser; it loads configuration data from a file.
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
  public:
    // / Creates a new log engine, creating the log file if needed.
    /** Creates a new log engine, and Initializes the pertinent data in order to allow logging. If it's the first log engine to be created, the log file will be opened/created.
        @param localLevel level of verbosity of this log.
        @param name 3 characters with an identifier for the log.
    */
      LogEngine (LOG_LEVEL localLevel, const std::string & name);
    // / Deletes the log engine, closing the log file if needed.
    /** Deletes the log engine and associated data. If there's no other log engines left, the log file will be closed.
    */
     ~LogEngine (void);

    // / Writes a log message to the log file.
    /** Writes a log message to the log file, provided its level is low enough (compared to the level of the log engine). It's possible to avoid writing a newline before the message.
        @param level level of the log message.
        @param textToLog message that is to be logged if its level is low enough.
    */
    void format (LOG_LEVEL level, const char *textToLogFormat, ...);
    void telemetry (LOG_LEVEL level, const char *textToLogFormat, ...);
    void loadscreen (LOG_LEVEL level, const char *textToLogFormat, ...);

    // / Writes a printf-like formated log message to the log file.
    /** Writes a printf-like formated log message to the log file, provided its level is low enough (compared to the level of the log engine). A newline is always writen before the message.
        @param level level of the log message.
        @param textToLogFormat first part of a printf-like formated log message.
        @param ... optional additional parameters that complete the log message format.
    */
    void log(const LOG_LEVEL level, const int mask, const char *textToLogFormat, ...);

    void setName(const std::string & name);

    // / Friends, this test function is a friend and can use private methods.
    friend void TestGetLogLevelCode ();
};

LOG_LEVEL stologlevel (const std::string &srcString);
#endif
