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

#include "axis.hpp"

void Axis::setNewRawValue(int value)
{
    //update device axis calibration values
    if (value < minRawValue)
    {
        minRawValue = value;
    }
    if (value > maxRawValue)
    {
        maxRawValue = value;
    }
    //normalize raw value and store it. 
    if (maxRawValue == minRawValue)
    {
        this->value = value?1:0;
    }else{
        this->value = ( double(value - minRawValue) / double(maxRawValue - minRawValue)); //store normalized value
    }
}
void Axis::updateValue()
{
    //apply filters in order
    for (int i = 0; i< 10 /*axisFilterList.size()*/; i++)
    {
    //    value = axisFilterList[i]->processValue(value);
    }
}
double Axis::getValue()
{
    //return the preprocessed value
    return value;
}
Axis::Axis () : minRawValue(std::numeric_limits<int>::min()), maxRawValue(std::numeric_limits<int>::max())
{
    //empty
}
Axis::~Axis ()
{
    // delete filters
    // delete filters list
}

int getIDJoyAxis (int joystickNumber, int axisNumber)
{
/*
    int result = 0;
                DD  NNN                
        DeviceType  DeviceNumber
   EE            ^  ^               EEE
   EngineType<-. |  |  ,->ElementNumber
               | |  |  |
               | |  |  |
              EEDDNNNEEE        
    result +=  100000000 * 0;
    result +=    1000000 * 4;
    result +=       1000 * joystickNumber;
    result +=          1 * axisNumber;
    return result;
*/
    return 4000000 + (joystickNumber * 1000) + axisNumber;
}
int getIDJoyButton (int joystickNumber, int buttonNumber)
{
/*
    int result = 0;
                DD  NNN                
        DeviceType  DeviceNumber
   EE            ^  ^               EEE
   EngineType<-. |  |  ,->ElementNumber
               | |  |  |
               | |  |  |
              EEDDNNNEEE        
    result +=  100000000 * 0;
    result +=    1000000 * 3;
    result +=       1000 * joystickNumber;
    result +=          1 * buttonNumber;
    return result;
*/
    return 3000000 + (joystickNumber * 1000) + buttonNumber;
}
int getIDMouseAxis (int axisNumber)
{
/*
    int result = 0;
                DD  NNN                
        DeviceType  DeviceNumber
   EE            ^  ^               EEE
   EngineType<-. |  |  ,->ElementNumber
               | |  |  |
               | |  |  |
              EEDDNNNEEE        
    result +=  100000000 * 0;
    result +=    1000000 * 2;
    result +=       1000 * 0;
    result +=          1 * axisNumber;
    return result;
*/
    return 2000000 + axisNumber;
}
int getIDMouseButton (int buttonNumber)
{
/*
    int result = 0;
                DD  NNN                
        DeviceType  DeviceNumber
   EE            ^  ^               EEE
   EngineType<-. |  |  ,->ElementNumber
               | |  |  |
               | |  |  |
              EEDDNNNEEE        
    result +=  100000000 * 0;
    result +=    1000000 * 1;
    result +=       1000 * 0;
    result +=          1 * buttonNumber;
    return result;
*/
    return 1000000 + buttonNumber;
}
int getIDKeyboardKey (int keyCode)
{
/*
    int result = 0;
                DD  NNN                
        DeviceType  DeviceNumber
   EE            ^  ^               EEE
   EngineType<-. |  |  ,->ElementNumber
               | |  |  |
               | |  |  |
              EEDDNNNEEE        
    result +=  100000000 * 0;
    result +=    1000000 * 0;
    result +=       1000 * 0;
    result +=          1 * keyCode;
    return result;
*/
    return keyCode;
}
