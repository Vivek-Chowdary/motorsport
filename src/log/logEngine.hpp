
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
#    define LOG_ENGINE_HPP

#    include <cstdio>
#    include <cstdarg>
#    include <cstdlib>
#    include <string>

#include <fstream>
#include <iostream>

/// It's used to indicate the level of verbosity in a log.
/** It indicates the level of verbosity of a log. The lower the number is, the more important the messages are.
*/
enum LOG_LEVEL
{
    LOG_ERROR = 0,              //!< Message of a critical error.
    LOG_WARNING = 1,            //!< Message of a non-critical error, a warning.
    LOG_INFO = 2,               //!< Message with basic information.
    LOG_VERBOSE = 3,            //!< Message with lots of information.
    LOG_TRACE = 4               //!< Message with lots of information, aimed at debugging.
};

/// Max. number that can be used as LOG_LEVEL.
#define MAX_LOG_LEVEL 4
/// Number of characters for the log instance name.
/** Number of characters for the log instance name. Those 3 characters (real characters, not counting '\0') can be read in the log file as a way to identify the originator engine.
*/
#define LOG_NAME_LENGTH 3

/// Allows to automate the recording of log messages to a file.
/** Allows to automate the recording of log messages to a plain-text file. Every log engine has its own level of verbosity, meaning it can display only messages of a certain level of importance (discarding the less important messages).
*/
class LogEngine
{

    /// Indicates how much information will be stored in this log instance.
    /** Indicates the level of verbosity of the log. All log messages with a number higher to this one will be discarded, and therefore not writen to the log file.
    */
    LOG_LEVEL logLevel;
    /// Indicates a short name, prefixed to every log line.
    /** Indicates a short name or initials for instance of the log engine. It allows to know which "engine" this log is associated with.
    */
    std::string logName;
    /// Indicates the file where logs will be writen.
    /** Indicates a plain-text file where log messages can be writen.
    */
    static std::fstream logFile;
    /// Indicates how much information will be stored in the log file.
    /** Indiactes how much information will be stored in the log file, regardless of a higher level for current instance.
    */
    static LOG_LEVEL globalLevel;
    /// Stores the number of log engine instances.
    /** Stores the number of log engine instances that have been created. This number is increased or decreased everytime a log engine is created or deleted.
    */
    static int numberOfLogEngines;
    /// Helper function translating log level into a short code.
    /** Helper function translating log level into a short code. 
	@param verbosity level to get code for.
	@return code as a short c-string.
    */
    const char* GetLogLevelCode(LOG_LEVEL level);
  public:
    /// Creates a new log engine, creating the log file if needed.
    /** Creates a new log engine, and Initializes the pertinent data in order to allow logging. If it's the first log engine to be created, the log file will be opened/created.
        @param localLevel level of verbosity of this log.
        @param name 3 characters with an identifier for the log.
    */
    LogEngine ( LOG_LEVEL localLevel, const char *name);
    /// Deletes the log engine, closing the log file if needed.
    /** Deletes the log engine and associated data. If there's no other log engines left, the log file will be closed.
    */
    ~LogEngine ( void );

    /// Writes a log message to the log file.
    /** Writes a log message to the log file, provided its level is low enough (compared to the level of the log engine). It's possible to avoid writing a newline before the message.
        @param level level of the log message.
        @param textToLog message that is to be logged if its level is low enough.
        @return 1 on success.
        @return -1 if the message level is too high to be logged.
        @return -2 if there was problem writing to the file. 
    */
    int put ( LOG_LEVEL level, const char *textToLog );

    /// Writes a printf-like formated log message to the log file.
    /** Writes a printf-like formated log message to the log file, provided its level is low enough (compared to the level of the log engine). A newline is always writen before the message.
        @param level level of the log message.
        @param textToLogFormat first part of a printf-like formated log message.
        @param ... optional additional parameters that complete the log message format.
        @return 1 on success.
        @return -1 if the message level is too high to be logged.
        @return -2 if the message level is incorrect.
        @return -3 if there was a problem writing to the file.
    */
    int format ( LOG_LEVEL level, const char *textToLogFormat, ... );



    /// Friends, this test function is a friend and can use private methods.
    friend void TestGetLogLevelCode();
  

};

#endif
