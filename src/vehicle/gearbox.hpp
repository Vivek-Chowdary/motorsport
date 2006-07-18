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

SHARED_PTR(GearboxGear, pGearboxGear, wGearboxGear);
class GearboxGear : public WorldObject
{
  private:
    GearboxGear (XmlTag * tag);
  public:
    static pGearboxGear create(XmlTag * tag);
    bool enabled;
    double ratio;   
    int number;
    ~GearboxGear ();
    int getNumber() { return number; } ;
    double getRatio() { return ratio; } ;
    bool isEnabled() { return enabled; } ;
}; 
SHARED_PTR(Gearbox, pGearbox, wGearbox);
class Gearbox : public DriveMass 
{
  private:
    std::map < int, pGearboxGear> gearMap;
    int currentGear;
    double gearRatio;
    Gearbox (XmlTag * tag);
  public:
    static pGearbox create(XmlTag * tag);
    void setContainer(pWorldObject container);
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
