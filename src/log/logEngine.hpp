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
#define LOG_ENGINE_HPP

/******************************************************************************
*
* Includes
*
******************************************************************************/

#include <stdio.h>


/******************************************************************************
*
* Classes
*
******************************************************************************/

class LogEngine
{
	FILE *logFile;
    signed char logLevel;
	public:
		//starts the log process, specifying the level of verbosity
		int start (signed char level, char *filePath, bool appendMode=false);
		//writes textToLog to the .logFile (level 0=error; level 1=warning; level >1=info)
		int put (signed char level, char *textToLog, bool useNewLine=true);
		//same as put - but appends the data to the same line
		int append (signed char level, char *textToLog);
		//stops the log process
		int stop (void);
};


#else
//#error multiple include of logEngine.hpp // this isn't normally done, but I like it :) MF
#endif
