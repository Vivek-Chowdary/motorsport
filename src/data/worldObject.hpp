/******************************************************************************
*
* Copyright (C) 2004 Bruno Gonz�lez Campo (stenyak@users.sourceforge.net)
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
#   include "xmlParser.hpp"
#   include "logEngine.hpp"

class WorldObject
{
  protected:
    LogEngine * log;
    static int instancesCount;

  public:
    // data
    virtual ~WorldObject (){};
    virtual void processXmlRootNode (DOMNode * n) = 0;
    
    // physics
    virtual void startPhysics (DOMNode * n){};
    virtual void stepPhysics (){};
    virtual void stopPhysics (){};

    // graphics
    virtual void startGraphics (DOMNode * n){};
    virtual void stepGraphics (){};
    virtual void stopGraphics (){};

    // input
    virtual void startInput (DOMNode * n){};
    virtual void stepInput (){};
    virtual void stopInput (){};
};
#endif
