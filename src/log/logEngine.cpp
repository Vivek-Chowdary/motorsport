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

/******************************************************************************
*
* Includes
*
******************************************************************************/

#include "logEngine.hpp"

/******************************************************************************
*
* Functions
*
******************************************************************************/

int LogEngine::start (bool appendMode, signed char level, char *filePath)
{
	//check if we append (or rewrite the whole file)
	logFile = fopen (filePath, (appendMode ? "a" : "w"));
    
    //we set the level of verbosity
    logLevel = level;
    
	//we put some text at the start of the current log
	put (false, 2, "Start of LogFile");
	
	return (0);
}


int LogEngine::put (bool appendMode, signed char level, char *textToLog)
{ 
    //check if we have been told to write this kind of log
    if (level <= logLevel)
    {
        //check if we should append or create a new line
        if (appendMode)
        {
            // separate previous log with a blank space
            fputc (' ', logFile);
        } else {
            //write log level information
            // (level 0=error; 1=warning; >1=info)
            switch (level)
            {
                case 0: fputs ("\n(EE): ", logFile); break;
                case 1: fputs ("\n(WW): ", logFile); break;
                default:fputs ("\n(II): ", logFile); break;
            }
        }
        //write log text
        fputs (textToLog, logFile);
    }
	
	return (0);
}


int LogEngine::stop (void)
{
	put (false, 2, "End of LogFile");
	fclose (logFile);
	
	return (0);
}
