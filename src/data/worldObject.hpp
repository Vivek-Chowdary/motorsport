/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef WORLDOBJECT_HPP
#   define WORLDOBJECT_HPP
#   include "tools/xercesc_fwd.hpp"

//forward declatations
class LogEngine;

class WorldObject
{
  protected:
    // data
    LogEngine * log;

    // physics
    virtual void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n){};
    virtual void stopPhysics (){};
    
    // graphics
    virtual void startGraphics (XERCES_CPP_NAMESPACE::DOMNode * n){};
    virtual void stopGraphics (){};
    
    // input
    virtual void startInput (XERCES_CPP_NAMESPACE::DOMNode * n){};
    virtual void stopInput (){};

  public:
    // data
    virtual ~WorldObject (){};
    virtual void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n) = 0;
    
    // physics
    virtual void stepPhysics (){};

    // graphics
    virtual void stepGraphics (){};

    // input
    virtual void stepInput (){};
};
#endif
