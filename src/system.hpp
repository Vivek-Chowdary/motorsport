/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef SYSTEM_HPP
#   define SYSTEM_HPP
#   include "SDL/SDL_types.h"
#   include <map>
#   include "axis.hpp"

namespace Ogre {
class RenderWindow;
class SceneManager;
}
class SystemData;
class SystemData
{                               // this contains all the data not related with the simulated/virtual world
    static SystemData systemDataObject;
    bool mainLoopEnabled;
    double cfmValue;     
    double erpValue;
  public:
    static SystemData *getSystemDataPointer ();
    SystemData ();
    ~SystemData ();

    double getCfmValue();
    void setCfmValue(const double & value);
    double getErpValue();
    void setErpValue(const double & value);
    

    Uint32 physicsTimeStep;
    Uint32 physicsDesiredFrequency;

    // main loop time control data
    Uint32 realTime;
    Uint32 simulationTime;
    Uint32 statisticsTime;
    int physicsSteps;
    Uint32 physicsFrequency;
    int graphicsSteps;
    Uint32 graphicsFrequency;
    int timeScale;
    int pauseStep;
    int height;
    int width;

    int videoRecordTimestep;
    bool isMainLoopEnabled (void); // does the program have to keep running?
    void enableMainLoop (void); // allows the program to start running
    void disableMainLoop (void);    // allows the program to stop running

    Ogre::RenderWindow * ogreWindow;
    Ogre::SceneManager * ogreSceneManager;
    std::map <int, Axis * > axisMap;
    std::string dataDir;
    std::string tmpPath;
};

#endif
