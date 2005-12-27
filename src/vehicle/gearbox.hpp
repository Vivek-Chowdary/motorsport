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
#   include "tools/xercesc_fwd.hpp"

class GearboxGear : public WorldObject
{
  public:
    bool enabled;
    double ratio;   
    int number;
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    GearboxGear (WorldObject * container, std::string name, XERCES_CPP_NAMESPACE::DOMNode * n);
    ~GearboxGear ();
    int getNumber() { return number; } ;
    double getRatio() { return ratio; } ;
    bool isEnabled() { return enabled; } ;
}; 

class Gearbox : public DriveMass 
{
  private:
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    void processXmlGearListNode(XERCES_CPP_NAMESPACE::DOMNode * GearListNode);
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopPhysics ();
    std::map < int, GearboxGear *> gearMap;
    int currentGear;
    double gearRatio;
  public:
    Gearbox (WorldObject * container, std::string name, XERCES_CPP_NAMESPACE::DOMNode * n);
    ~Gearbox ();
    void setGear (int inputGear)      { currentGear = inputGear; };
    double getGearRatio()             { return gearMap[currentGear]->ratio; };
    std::string getCurrentGearName() { return gearMap[currentGear]->getName(); };
    int getCurrentGearIndex()         { return currentGear; };
    void stepPhysics ();
    void gearUp ();
    void gearDown ();
};


#endif
