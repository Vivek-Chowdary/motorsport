
//#include "main.hpp"
//#include "logEngine.hpp"
#include "dataEngine.hpp"


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
