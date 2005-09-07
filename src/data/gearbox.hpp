/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef GEARBOX_HPP
#   define GEARBOX_HPP
#   include <string>
#   include <map>
#   include "driveMass.hpp"
#   include "wheel.hpp"
#   include "data/xercesc_fwd.hpp"

class GearboxGear : public WorldObject
{
  public:
    double ratio;   
    bool enabled;
    int index;
    std::string label;

    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);

    GearboxGear (XERCES_CPP_NAMESPACE::DOMNode * n);
    ~GearboxGear () { } ;

    int getIndex() { return index; } ;
    double getRatio() { return ratio; } ;
    bool isEnabled() { return enabled; } ;
    
}; 

class Gearbox : public DriveMass 
{
  private:
    // data
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    void processXmlGearListNode(XERCES_CPP_NAMESPACE::DOMNode * GearListNode);
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopPhysics ();
    std::map < int, GearboxGear *> gearMap;
    int currentGear;

    // physics
    double gearRatio;
  public:
    // data
    Gearbox (XERCES_CPP_NAMESPACE::DOMNode * n);
    ~Gearbox ();
    void setGear (int inputGear)      { currentGear = inputGear; };
    double getGearRatio()             { return gearMap[currentGear]->ratio; };
    std::string getCurrentGearLabel() { return gearMap[currentGear]->label; };
    int getCurrentGearIndex()         { return currentGear; };
    
    // physics
    void stepPhysics ();
    void gearUp ();
    void gearDown ();
};


#endif
