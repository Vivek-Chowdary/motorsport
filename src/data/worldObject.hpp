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

#ifndef WORLDOBJECT_HPP
#   define WORLDOBJECT_HPP
#   include "system.hpp"
#   include "world.hpp"
#   include "xmlParser.hpp"
#   include "logEngine.hpp"

class WorldObject
{
  protected:
    LogEngine * log;

  public:
    // data
    virtual ~WorldObject (){};
    virtual void processXmlRootNode (DOMNode * n) = 0;
    
    // physics
    virtual void processPhysicsDataNode (DOMNode * n) = 0;
    virtual void stepPhysics () = 0;
    virtual void stopPhysics () = 0;

    // graphics
    
    virtual void processGraphicsDataNode (DOMNode * n) = 0;
    virtual void stepGraphics () = 0;
    virtual void stopGraphics () = 0;

    // input
    virtual void stepInput () = 0;
    virtual void stopInput () = 0;
};
#endif
