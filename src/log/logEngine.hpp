/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#ifndef LOG_ENGINE_HPP
#define LOG_ENGINE_HPP
#include <shared.hpp>
#include <fstream>

#ifdef _MSC_VER
// Apparently visual studio does not like this macro
#define __format format
#else
#define __format(X ,Y, ...) format((X), std::string("[%s:%3i] " + std::string((Y))).c_str(),__FILE__, __LINE__, ##__VA_ARGS__)
#endif

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

// Forward declarations
class GuiEngine;
SHARED_PTR_MAP(LogEngine, pLogEngine, LogEngines, LogEnginesIt, wLogEngine);

/// Allows to automate the recording of log messages to a file.
/** Allows to automate the recording of log messages to a plain-text file. Every log engine has its own level of verbosity, meaning it can display only messages of a certain level of importance (discarding the less important messages).
*/
class LogEngine
{
    private:
    static unsigned int instancesCount;
    static LogEngines logEngines;
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
    // / Creates a new log engine, creating the log file if needed.
    /** Creates a new log engine, and Initializes the pertinent data in order to allow logging. If it's the first log engine to be created, the log file will be opened/created.
        @param localLevel level of verbosity of this log.
        @param name 3 characters with an id for the log.
    */
      LogEngine (LOG_LEVEL localLevel, const std::string & name);
  public:
    static void deleteAll();
    static pLogEngine create(LOG_LEVEL localLevel, const std::string & name);
    // / Deletes the log engine, closing the log file if needed.
    /** Deletes the log engine and associated data. If there's no other log engines left, the log file will be closed.
    */
     ~LogEngine (void);

    static void logAll();

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

    void setName(std::string name);

    // / Friends, this test function is a friend and can use private methods.
    friend void TestGetLogLevelCode ();
};

LOG_LEVEL stologlevel (const std::string &srcString);
#endif
