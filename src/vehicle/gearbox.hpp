/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
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
    std::string description;
    void readCustomDataTag(XmlTag * tag);
    bool enabled;
    double ratio;   
  public:
    static pGearboxGear create(XmlTag * tag);
    ~GearboxGear ();
    int getNumber();
    std::string getDescription();
    double getRatio();
    bool isEnabled();
}; 
SHARED_PTR(Gearbox, pGearbox, wGearbox);
class Gearbox : public DriveMass 
{
  private:
    int currentGear;
    double gearRatio;
    std::string itos(int integer);
    Gearbox (XmlTag * tag);
    void readCustomDataTag(XmlTag * tag);
  public:
    static pGearbox create(XmlTag * tag);
    ~Gearbox ();
    void setGear (int inputGear);
    int getCurrentGearIndex();
    pGearboxGear getCurrentGear();
    void stepPhysics ();
    void gearUp ();
    void gearDown ();
};
#endif
