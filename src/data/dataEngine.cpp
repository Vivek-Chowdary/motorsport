
#include "dataEngine.hpp"


void dataEngine::start (void)
{
    //first of all start the logger (automatically logs the start of itself)
    log.start ("logData.txt", false);

    //then do some useless things.
    //(it has no real use right now, but in the future, who knows... ;)
    log.put ("Starting useless loop");
    for (int i = 'a'; i <= 'g'; i++)
    {
//      log.put (&char (i));
    }
    log.put ("Finished doing useless loop");
}


void dataEngine::stop (void)
{
    /*do some stuff (unloading models and textures from memory, etc...)
    [...]
    */
    
    //finally stop the log engine
    log.stop ();
}
