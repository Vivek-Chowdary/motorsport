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
#include <sstream>

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
    if (maxRawValue != minRawValue)
    {
        this->value = ( double(value - minRawValue) / double(maxRawValue - minRawValue)); //store normalized value
    }
}
void Axis::updateValue()
{
    if (maxRawValue <= minRawValue)
    {
        value = 0.5;
    }
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
Axis::Axis () : minRawValue(std::numeric_limits<int>::max()), maxRawValue(std::numeric_limits<int>::min())
{
    //empty
}
Axis::~Axis ()
{
    // delete filters
    // delete filters list
}

void inttostr (int number, std::string &destString)
{
    std::stringstream tempString;
    tempString << number;
    destString.assign (tempString.str());
}


/* gahh stupid switches and cases won't work!! FIXME
void aidtostr (int axisID, std::string destString)
{
    int engineType = int (axisID / 100000000);
    int rest = axisID % 100000000;
    std::string tmpStr;
//    switch (engineType)
//    {
    if (engineType == 0)
    {
//        case 0: //input player
            int deviceType = int (rest / 1000000);
            int deviceNumber = axisID % 1000000; //device number
            switch (deviceType)
            {
                case 0: //kkey
                    int key = axisID % 1000;
                    destString.assign ("Keyboard #");
                    inttostr (deviceNumber, tmpStr);
                    destString.append (tmpStr);
                    destString.append (", SDL key code #");
                    inttostr (key, tmpStr);
                    destString.append (tmpStr);
                    break;
                case 1: //mbut
                    int button = axisID % 1000;
                    destString.assign ("Mouse #");
                    inttostr (deviceNumber, tmpStr);
                    destString.append (tmpStr);
                    destString.append (", button #");
                    inttostr (button, tmpStr);
                    destString.append (tmpStr);
                    break;
                case 2: //maxis
                    int axis = axisID % 1000;
                    destString.assign ("Mouse #");
                    inttostr (deviceNumber, tmpStr);
                    destString.append (tmpStr);
                    destString.append (", axis #");
                    inttostr (axis, tmpStr);
                    destString.append (tmpStr);
                    break;
                case 3: //jbut
                    int button = axisID % 1000;
                    destString.assign ("Joystick #");
                    inttostr (deviceNumber, tmpStr);
                    destString.append (tmpStr);
                    destString.append (", button #");
                    inttostr (button, tmpStr);
                    destString.append (tmpStr);
                    break;
                case 4: //jaxis
                    int axis = axisID % 1000;
                    destString.assign ("Joystick #");
                    inttostr (deviceNumber, tmpStr);
                    destString.append (tmpStr);
                    destString.append (", axis #");
                    inttostr (axis, tmpStr);
                    destString.append (tmpStr);
                    break;
                case 5: //jhaxis
                    int axis = axisID % 10;
                    int hat = (axisID % 1000) - axis;
                    destString.assign ("Joystick #");
                    inttostr (deviceNumber, tmpStr);
                    destString.append (tmpStr);
                    destString.append (", hat #");
                    inttostr (hat, tmpStr);
                    destString.append (tmpStr);
                    destString.append (", axis #");
                    inttostr (axis, tmpStr);
                    destString.append (tmpStr);
                    break;
                case 6: //jtaxis
                    int axis = axisID % 10;
                    int trackball = (axisID % 1000) - axis;
                    destString.assign ("Joystick #");
                    inttostr (deviceNumber, tmpStr);
                    destString.append (tmpStr);
                    destString.append (", trackball #");
                    inttostr (trackball, tmpStr);
                    destString.append (tmpStr);
                    destString.append (", axis #");
                    inttostr (axis, tmpStr);
                    destString.append (tmpStr);
                    break;
                case 7: //tir
                    int axis = axisID % 1000;
                    destString.assign ("Track IR #");
                    inttostr (deviceNumber, tmpStr);
                    destString.append (tmpStr);
                    destString.append (", axis #");
                    inttostr (axis, tmpStr);
                    destString.append (tmpStr);
                    break;
                default: //other
                    destString.assign ("Local human player, unknown ID (#");
                    inttostr (axisID, tmpStr);
                    destString.append (tmpStr);
                    destString.append (")");
                    break;
            }
//            break;
    } else if (engineType == 1) {
//        case 1: //replay player
            destString.assign ("Replay vehicle #");
            int axis = axisID % 100000;
            int driver = (axisID % 100000000) - axis;
            inttostr (driver, tmpStr);
            destString.append (tmpStr);
            destString.append (", axis #");
            inttostr (axis, tmpStr);
            destString.append (tmpStr);
//            break;
    } else if (engineType == 2) {
//        case 2: //ai player
            destString.assign ("AI driver #");
            int axis = axisID % 100000;
            int driver = (axisID % 100000000) - axis;
            inttostr (driver, tmpStr);
            destString.append (tmpStr);
            destString.append (", axis #");
            inttostr (axis, tmpStr);
            destString.append (tmpStr);
//            break;
    } else {
//        default: //other
            destString.assign ("Unknown ID (#");
            inttostr (axisID, tmpStr);
            destString.append (tmpStr);
            destString.append (")");
            break;
    }
}
*/

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
