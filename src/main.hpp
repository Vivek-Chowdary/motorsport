
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

#ifndef MAIN_HPP
#    define MAIN_HPP

#    include <stdlib.h>
#    include "SDL.h"
#    include "world.hpp"        //contains the IDF for the simulated/virtual world data
#    include "system.hpp"       //contains the IDF for the system data
#    include "logEngine.hpp"    //allows to easily log actions
#    include "dataEngine.hpp"   //loads/saves data from memory into the virtual world
#    include "inputEngine.hpp"  //process the queue of input events
#    include "graphicsEngine.hpp"
                                //displays the virtual and system data (sim+gui)
#    include "physicsEngine.hpp"    //calculates the physics of the world data
#    include "guiEngine.hpp"    //displays all the user interface on screen

class TRunnable
{
  public:
    virtual void Initialize (  ) = 0;
    virtual void Run (  ) = 0;
    virtual void Shutdown (  ) = 0;
};

class TGlobals
{
  private:
    TGlobals (  )
    {
    };                          //forbidden

  protected:
    SystemData * systemData;
    WorldData *worldData;

    LogEngine *log;
    DataEngine *data;
    InputEngine *input;
    GraphicsEngine *graphics;
    PhysicsEngine *physics;
    GuiEngine *gui;

  public:
    //never mind the _name its just a hack to overcome that we have no fixed nameing convention
  TGlobals ( WorldData & _worldData, SystemData & _systemData, LogEngine & _log, DataEngine & _data, InputEngine & _input, GraphicsEngine & _graphics, PhysicsEngine & _physics, GuiEngine & _gui ):systemData ( &_systemData ),
        worldData ( &_worldData ),
        log ( &_log ),
        data ( &_data ),
        input ( &_input ),
        graphics ( &_graphics ), physics ( &_physics ), gui ( &_gui )
    {
        //empty
    };
};

class GuiLoop:public TRunnable,
      public TGlobals
{
  public:
    virtual void Initialize (  );
    virtual void Run (  );
    virtual void Shutdown (  );

         GuiLoop ( WorldData & _worldData, SystemData & _systemData,
                   LogEngine & _log, DataEngine & _data, InputEngine & _input,
                   GraphicsEngine & _graphics, PhysicsEngine & _physics,
                   GuiEngine & _gui ):TGlobals ( _worldData, _systemData, _log,
                                                 _data, _input, _graphics,
                                                 _physics, _gui )
    {
        //empty
    };
};

class SimLoop:public TRunnable,
      public TGlobals
{
  public:
    virtual void Initialize (  );
    virtual void Run (  );
    virtual void Shutdown (  );

         SimLoop ( WorldData & _worldData, SystemData & _systemData,
                   LogEngine & _log, DataEngine & _data, InputEngine & _input,
                   GraphicsEngine & _graphics, PhysicsEngine & _physics,
                   GuiEngine & _gui ):TGlobals ( _worldData, _systemData, _log,
                                                 _data, _input, _graphics,
                                                 _physics, _gui )
    {
        //empty
    };
};

class MainLoop:public TRunnable,
      public TGlobals
{
  public:
    virtual void Initialize (  );
    virtual void Run (  );
    virtual void Shutdown (  );

         MainLoop ( WorldData & _worldData, SystemData & _systemData,
                    LogEngine & _log, DataEngine & _data, InputEngine & _input,
                    GraphicsEngine & _graphics, PhysicsEngine & _physics,
                    GuiEngine & _gui ):TGlobals ( _worldData, _systemData, _log,
                                                  _data, _input, _graphics,
                                                  _physics, _gui )
    {
        //empty
    };

};

#endif
