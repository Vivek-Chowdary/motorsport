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
Axis::Axis (AXIS_TYPE axisType) : type(axisType), minRawValue(std::numeric_limits<int>::min()), maxRawValue(std::numeric_limits<int>::max())
{
    //empty
}
/*Axis::Axis (AXIS_TYPE axisType)
{
    minRawValue = 10000000; //this is supposed to be the max possible int value
    maxRawValue = -1000000; //min possible int value
    type = axisType;
}*/
Axis::~Axis ()
{
    // delete filters
    // delete filters list
}

