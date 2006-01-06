/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "body.hpp"
#include "xmlParser.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "log/logEngine.hpp"
#include "system.hpp"
#include "world.hpp"
#include "axis.hpp"
#include "ode/ode.h"
#include "math.h"
#include "SDL/SDL_keysym.h"
#include "vehicle.hpp"

Body::Body (WorldObject * container, XmlTag * tag)
    :WorldObject(container, "body")
{
    dragCoefficient = 0.3;
    frontalArea = 0;
    VehicleBodyOdeObjectData data;
    OgreObjectData ogreData;

    if (tag->getName() == "body")
    {
        setName (     tag->getAttribute("name"));
        data.length = stod (tag->getAttribute("length"));
        data.width = stod (tag->getAttribute("width"));
        data.height = stod (tag->getAttribute("height"));
        data.mass = stod (tag->getAttribute("mass"));
        frontalArea = stod (tag->getAttribute("frontalArea"));
        dragCoefficient = stod (tag->getAttribute("dragCoefficient"));
        ogreData.meshPath = tag->getAttribute("mesh");
    }

    odeObjects[getId()] = new OdeObject(this, data, getId());

    // set the air drag variables correctly
    if (frontalArea == 0)
    {
        frontalArea = data.width * data.height * 0.6;
    }
    ogreData.meshPath = getPath() + ogreData.meshPath;
    OgreObject * ogreObject = new OgreObject(this, ogreData, getId());
    ogreObjects[getId()] = ogreObject;
    ogreObjects.begin()->second->setOdeReference(getMainOdeObject());
}

Body::~Body ()
{
}

void Body::stepPhysics ()
{
    dBodyID bodyID = getMainOdeObject()->getBodyID();
    if (this == World::getWorldPointer ()->vehicleList[0]->body)
    {
        double moveZ = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_BACKSPACE)]->getValue() * 50000;
        moveZ += SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_RETURN)]->getValue() * 12200;
        moveZ -= SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_RSHIFT)]->getValue() * 10000;
        dBodyAddForce (bodyID, 0, 0, moveZ);
    }
    
    // apply simple air drag forces:
    const double airDensity = 1.225;
    //  f = Cx              * 0.5 * airDensity * v^2     * area;
    //  f = dragCoefficient * 0.5 * 1.225      * vel*vel * frontalArea;
    Vector3d velocity (dBodyGetLinearVel (bodyID));
    double velModule = velocity.distance();
    Vector3d normalizedVel (velocity);
    normalizedVel.scalarDivide(velModule);
    normalizedVel.scalarMultiply(-1);
    Vector3d force (normalizedVel);
    force.scalarMultiply (0.5 * dragCoefficient * airDensity * frontalArea * velModule * velModule);

    dBodyAddForce (bodyID, force.x, force.y, force.z);

    log->__format(LOG_DEVELOPER, "Body air drag force = (%f, %f, %f)", force.x, force.y, force.z);
}
