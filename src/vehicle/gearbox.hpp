/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef GEARBOX_HPP
#define GEARBOX_HPP
#include "driveMass.hpp"
#include "wheel.hpp"

SHARED_PTR(GearboxGear, pGearboxgear);
class GearboxGear : public WorldObject
{
  public:
    bool enabled;
    double ratio;   
    int number;
    GearboxGear (WorldObject * container, XmlTag * tag);
    ~GearboxGear ();
    int getNumber() { return number; } ;
    double getRatio() { return ratio; } ;
    bool isEnabled() { return enabled; } ;
}; 
SHARED_PTR(Gearbox, pGearbox);
class Gearbox : public DriveMass 
{
  private:
    std::map < int, GearboxGear *> gearMap;
    int currentGear;
    double gearRatio;
    Gearbox (WorldObject * container, XmlTag * tag);
  public:
    static pGearbox create(WorldObject * container, XmlTag * tag);
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
