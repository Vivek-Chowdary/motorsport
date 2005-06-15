/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "camera.hpp"
#include "xmlParser.hpp"
#include "ode/ode.h"
#include "log/logEngine.hpp"
#include "world.hpp"
#include "system.hpp"
#include "track.hpp"
#include "SDL/SDL_keysym.h"
#include "vector3d.hpp"

void Camera::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    log = new LogEngine(LOG_DEVELOPER, "CAM");
    log->put (LOG_DEVELOPER, "Creating a camera");
    positionOffset = new Vector3d (0, 0, 0);
    targetOffset = new Vector3d (0, 0, 0);
    positionID = 0;
    targetID = 0;
    index = "0";
    if (n->hasAttributes ())
    {
        DOMNamedNodeMap *attList = n->getAttributes ();
        int nSize = attList->getLength ();
        for (int i = 0; i < nSize; ++i)
        {
            DOMAttr *attNode = (DOMAttr *) attList->item (i);
            std::string attribute;
            assignXmlString (attribute, attNode->getName());
            if (attribute == "index")
            {
                assignXmlString (index, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the position index: %s", index.c_str());
            }
            if (attribute == "position")
            {
                assignXmlString (attribute, attNode->getValue()); 
                log->format (LOG_CCREATOR, "Found the position: %s", attribute.c_str());
                *positionOffset = Vector3d(attribute);
            }
            if (attribute == "target")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the target: %s", attribute.c_str());
                *targetOffset = Vector3d(attribute);
            }
        }
    }
}

void Camera::stopPhysics ()
{
    delete positionOffset;
    delete targetOffset;
}

void Camera::stepPhysics ()
{
    if (this == World::getWorldPointer()->getActiveCamera() )
    {
        // Move the camera
        float moveX = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_d)]->getValue();
        float moveY = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_s)]->getValue();
        moveX *= SystemData::getSystemDataPointer()->physicsTimeStep * 1000;
        moveY *= SystemData::getSystemDataPointer()->physicsTimeStep * 1000;
        //World::getWorldPointer ()->getActiveCamera()->ogreCamera->moveRelative (Ogre::Vector3 (moveX / 100, 0, moveZ / 100));
        *positionOffset += Vector3d(moveX/1000, moveY/1000, 0);

        //moveX = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_a)]->getValue();
        //moveY = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_w)]->getValue();
        moveX *= SystemData::getSystemDataPointer()->physicsTimeStep * 1000;
        moveY *= SystemData::getSystemDataPointer()->physicsTimeStep * 1000;
        //World::getWorldPointer ()->getActiveCamera()->ogreCamera->moveRelative (Ogre::Vector3 (moveX / 100, 0, moveZ / 100));
        *positionOffset += Vector3d(moveX/1000, moveY/1000, 0);

        /*
        // Rotate the camera
        float rotX = -SystemData::getSystemDataPointer()->axisMap[getIDMouseAxis(0)]->getValue() + 0.5;
        float rotY = SystemData::getSystemDataPointer()->axisMap[getIDMouseAxis(1)]->getValue() - 0.5;
        rotX *= SystemData::getSystemDataPointer()->physicsTimeStep * 1000;
        rotY *= SystemData::getSystemDataPointer()->physicsTimeStep * 1000;
        *targetOffset += Vector3d (rotX/1000, rotY/1000, 0);
        */
    }
}

void Camera::setPositionID (dBodyID positionID)
{
    this->positionID = positionID;
}

void Camera::setTargetID (dBodyID targetID)
{
    this->targetID = targetID;
}
