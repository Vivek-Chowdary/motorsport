/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef WORLDOBJECT_HPP
#   define WORLDOBJECT_HPP
#   include "data/xercesc_fwd.hpp"

//forward declatations
class LogEngine;

class WorldObject
{
  protected:
    LogEngine * log;

  public:
    // data
    virtual ~WorldObject (){};
    virtual void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n) = 0;
    
    // physics
    virtual void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n){};
    virtual void stepPhysics (){};
    virtual void stopPhysics (){};

    // graphics
    virtual void startGraphics (XERCES_CPP_NAMESPACE::DOMNode * n){};
    virtual void stepGraphics (){};
    virtual void stopGraphics (){};

    // input
    virtual void startInput (XERCES_CPP_NAMESPACE::DOMNode * n){};
    virtual void stepInput (){};
    virtual void stopInput (){};
};
#endif
