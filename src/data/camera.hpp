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

#ifndef CAMERA_HPP
#   define CAMERA_HPP
#   include "Ogre.h"
#   include <vector>
#   include "system.hpp"

class Camera
{
  private:
    int rotateLeft;
    int rotateRight;
    int rotateUp;
    int rotateDown;

  public:
      Camera (int cameraNumber, float posX, float posY, float posZ, float lookAtX, float lookAtY, float lookAtZ);
     ~Camera ();

    // graphics
    void startGraphics (int cameraNumber, float posX, float posY, float posZ, float lookAtX, float lookAtY, float lookAtZ);
    void stepGraphics ();
    void stopGraphics ();
      Ogre::Camera * ogreCamera;

    // input
    void startInput ();
    void stepInput ();
    void stopInput ();
    void setRotateUp (int multiplier);
    void setRotateDown (int multiplier);
    void setRotateLeft (int multiplier);
    void setRotateRight (int multiplier);
    bool goBack;
    bool goForward;
    bool goLeft;
    bool goRight;

    // data
    static std::vector < Camera * >cameraList;
    int getRotateUp ();
    int getRotateDown ();
    int getRotateLeft ();
    int getRotateRight ();
};

#endif
