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

void logEngine::start (char *filePath, bool appendMode)
{
	//check if we append (or rewrite the whole file)
	logFile = fopen (filePath, (appendMode ? "a" : "w"));

	//we put some text at the start of the current log
	put ("START OF LOG");
}


void logEngine::put (char *textToLog)
{
	//write the textToLog in the log file after a EOL
	fputc ('\n', logFile);
	fputs (textToLog, logFile);
}


void logEngine::stop (void)
{
	put ("END OF LOG");
	fclose (logFile);
}
