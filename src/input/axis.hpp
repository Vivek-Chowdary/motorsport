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

#ifndef AXIS_HPP
#   define AXIS_HPP
#   include <limits>
#   include <string>  

class Axis
{
  private:
    int minRawValue; //calibration data (automatically updated)
    int maxRawValue; //calibration data (automatically updated)
    double value; //stores the normalized value
    //std::list < AxisFilter * > axisFilterList; //deadzones, linearity, etc... the AxisFilter class hasn't been coded yet
    
  public:
    Axis();
    ~Axis();

    //management of axis values
    void setNewRawValue(int value); //updates internal normalized value, done once per input event
    void updateValue();      //applies the filters, always done once per input engine step
    double getValue();            //returns normalized value, done once per physics engine step
    
    //management of filters list
    //void appendFilter (AxisFilter * filter);
    //void removeFilter (int n);
    //void insertFilter (AxisFilter * filter);
    // etc...
};

void axisIDtoStr(int axisID, std::string& destString);

int getIDKeyboardKey (int keyCode);
int getIDMouseButton (int buttonNumber);
int getIDMouseAxis (int axisNumber);
int getIDJoyButton (int joystickNumber, int buttonNumber);
int getIDJoyAxis (int joystickNumber, int axisNumber);

#endif