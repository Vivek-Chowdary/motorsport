
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

#include "logEngine.hpp"
#include "world.hpp"
#include "system.hpp"
#include <string.h>
#include "Ogre.h"
#include "cube.hpp"
#include "camera.hpp"
#include "domParser.hpp"

struct DataData;

class DataEngine
{
  private:
    LogEngine *log;              //a log object for logging independently from other engines and main
    WorldData *worldData;
    SystemData *systemData;
  public:
    DataEngine ( );    //starts the data engine
    ~DataEngine ( void );          //stops the data engine
    int loadWorldData ( void );
    int unloadWorldData ( void );

    /*here will probably go some new functions such as:
       int loadCarModel(carList*,FILE*carXmlFile);
       int loadTrackModel(track*,FILE*trackXmlFile);
       or maybe...
       int loadStreamingTrack(track*,FILE*trackXmlFile,currentPosition*,distance*);
       etc... */

    static int processDataConfigFile ( DOMNode * n, void * data );
};

struct DataData
{
    DataEngine * data;
    LOG_LEVEL localLogLevel;
    char * localLogName;
};
