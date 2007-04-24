/*****************************************************************************\
|* Copyright (C) 2003, 2007 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#include "body.hpp"
#include <Ogre.h>
#include <OgreNoMemoryMacros.h>
#include "system.hpp"
#include "world.hpp"
#include "axis.hpp"
#include <ode/ode.h>
#include <cmath>
#include <SDL/SDL_keysym.h>
#include "vehicle.hpp"

pBody Body::create (XmlTag * tag)
{
    pBody tmp (new Body(tag));
    return tmp;
}
void Body::readCustomDataTag(XmlTag * tag)
{
    frontalArea = stod (tag->getAttribute("frontalArea"));
    dragCoefficient = stod (tag->getAttribute("dragCoefficient"));
}
Body::Body (XmlTag * tag)
    :WorldObject("body")
{
    constructFromTag(tag);
}

Body::~Body ()
{
}

void Body::stepPhysics ()
{
    WorldObject::stepPhysics();
    dBodyID bodyID = getMainOdeObject()->getBodyID();
    if (userDriver)
    {
        double moveZ = System::get()->axisMap[getIDKeyboardKey(SDLK_BACKSPACE)]->getValue() * 50000;
        moveZ += System::get()->axisMap[getIDKeyboardKey(SDLK_RETURN)]->getValue() * 12200;
        moveZ -= System::get()->axisMap[getIDKeyboardKey(SDLK_RSHIFT)]->getValue() * 10000;
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

    //log->__format(LOG_DEVELOPER, "Body air drag force = (%f, %f, %f)", force.x, force.y, force.z);
}
void Body::setUserDriver ()
{
    log->__format(LOG_ENDUSER, "Body %s will now be driven by a human.", getName().c_str());
    userDriver = true;
}
