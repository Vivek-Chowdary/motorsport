
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

/// It's used to indicate the level of verbosity in a log.
/** It indicates the level of verbosity of a log. The lower the number is, the more important the messages are.
*/
enum LOG_LEVEL
{
    LOG_ERROR = 0,              //!< Message of a critical error.
    LOG_WARNING = 1,            //!< Message of a non-critical error, a warning.
    LOG_INFO = 2,               //!< Message with basic information.
    LOG_VERBOSE = 3,            //!< Message with information for complete information.
    LOG_TRACE = 4               //!< Message with lots of information, good for debugging.
};
#define MAX_LOG_LEVEL 4

/// Allows to automate the recording of log messages to a file.
/** Allows to automate the recording of log messages to a plain-text file. Every log engine has its own level of verbosity, meaning it can display only messages of a certain level of importance (discarding the less important messages).
*/
class LogEngine
{
    /// Indicates the file where logs will be writen.
    /** Indicates a plain-text file where log messages can be writen.
    */
    FILE *logFile;

    /// Indicates how much information will be stored in this log.
    /** Indicates the level of verbosity of the log. All log messages with a number higher to this one will be discarded, and therefore not writen to the log file.
    */
    LOG_LEVEL logLevel;
  public:

    /// Starts the log process.
    /** Initializes the pertinent data in order to allow logging. No logging should be attempted before executing this method.
        @param level level of verbosity of this log.
        @param filePath path to the plain-text file where logs will be writen.
        @param appendMode allows to append data to the log file. It rewrites the whole file by default.
        @return 1 on success.
        @return -1 if there was problems opening the log file.
        @return -2 if the level is incorrect.
    */
    int start ( LOG_LEVEL level,
                const char *filePath, bool appendMode = false );

    /// Writes a log message to the log file.
    /** Writes a log message to the log file, provided its level is low enough (compared to the level of the log engine). It's possible to avoid writing a newline before the message.
        @param level level of the log message.
        @param textToLog message that is to be logged if its level os low enough.
        @param useNewLine writes a '\\n' right before the message (default behaviour) if set to 'true'.
        @return 1 on success.
        @return -1 if the message level is too high to be logged.
        @return -2 if there was problem writing to the file. 
    */
    int put ( LOG_LEVEL level, const char *textToLog, bool useNewLine = true );

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

    /// Writes a log message to the log file, appending it to the previous line.
    /** Writes a log message to the log file, provided its level is low enough (compared to the level of the log engine). No newline is ever writen before the log message.
        @param level level of the log message.
        @param textToLog message that is to be logged if its level os low enough.
        @return 1 on success.
        @return -1 if the message level is too high to be logged.
        @return -2 if the message level is incorrect.
        @return -3 if there was a problem writing to the file.
    */
    int append ( LOG_LEVEL level, const char *textToLog );

    /// Stops the log process
    /** Closes the log file.
        @return 1 on success.
        @return -1 if there was a problem writing the end of log message
        @return -2 if there was problems while closing the file.
    */
    int stop ( void );
};

#endif
