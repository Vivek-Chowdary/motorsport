
/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright © 2000-2002 The OGRE Team
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
Filename:    ExampleApplication.h
Description: Base class for all the OGRE examples
-----------------------------------------------------------------------------
*/

#ifndef SIMULATION_HPP
#    define SIMULATION_HPP

#    include "Ogre.h"
#    include "OgreConfigFile.h"
#    include "stdio.h"
#    include "world.hpp"
#    include "system.hpp"

using namespace Ogre;

/** Base class which manages the standard startup of an Ogre application.
    Designed to be subclassed for specific examples if required.
*/
class Simulation
{
  public:
    /// Standard constructor
    Simulation (  );
    /// Start the example
    void go ( int resX, int resY, WorldData * wrlData, SystemData * sysData );

//  protected:
    Root *mRoot;
    SceneManager *mSceneMgr;
    WorldData *worldData;
    SystemData *systemData;

    /// Standard destructor
              ~Simulation (  );
    // These internal methods package up the stages in the startup process

    /** Configures the application - returns false if the user chooses to abandon configuration. */
    bool configure ( int resX, int resY );
    void createCamera ( void );
    bool manualInitialize ( const String & desiredRenderer, int resX,
                            int resY );
    void createScene ( void );  // pure virtual - this has to be overridden
    virtual void createViewports ( void );
    /// Method which will define the source of resources (other than current folder)
    virtual void setupResources ( void );
};

#endif
