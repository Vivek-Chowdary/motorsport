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

//#include "main.hpp"
//#include "logEngine.hpp"
#include "dataEngine.hpp"


/******************************************************************************
*
* Functions
*
******************************************************************************/

int main (int argc, char **argv)
{
	//we declare the engines
	//(more engines will be declared here when available)
	//How now brown cow - this is a test of Kirk's mediocre CVS skills
	logEngine log;
	dataEngine data;


	//we start the main program log
	log.start ("./logMain.txt", false);

	//we start the data engine
	log.put ("Starting data engine");
	data.start ();

	/*we start the rest of the engines the same way (when possible)
	[...]
	(we will probably check for errors returned by the .start functions)
	*/


	/*here will go either the 'game code', or a  "gui.start();" which will take
	care of it all (gui, main loop, etc.)... we still need to decide that.
	probably the best idea is to use a single piece of code for managing the
	sim-mainLoop and the gui code (which would mean the gui isn't a separate
	engine).
	[...]
	end of 'game' code
	*/


	//starting to stop the program: we stop all the engines that where loaded
	log.put ("Stopping data engine");
	data.stop ();

	/*the rest of engines will be closed the same way
	[...]
	(we will probably check for errors returned by the .stop functions)
	*/

	//we stop the main program log
	log.stop ();

	//and finally back to the OS
	return (0);
}
