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

#include "dataEngine.hpp"


/******************************************************************************
*
* Functions
*
******************************************************************************/

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
