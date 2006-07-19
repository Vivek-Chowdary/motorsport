/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "body.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "system.hpp"
#include "world.hpp"
#include "axis.hpp"
#include "ode/ode.h"
#include "math.h"
#include "SDL/SDL_keysym.h"
#include "vehicle.hpp"

pBody Body::create (XmlTag * tag)
{
    pBody tmp (new Body(tag));
    return tmp;
}
Body::Body (XmlTag * tag)
    :WorldObject("body")
{
    userDriver = false;
    dragCoefficient = 0.3;
    frontalArea = 0;
    pBodyOdeData data(new BodyOdeData);
    pOgreObjectData ogreData(new OgreObjectData);

    if (tag->getName() == "body")
    {
        setName (     tag->getAttribute("name"));
        data->length = stod (tag->getAttribute("length"));
        data->width = stod (tag->getAttribute("width"));
        data->height = stod (tag->getAttribute("height"));
        data->mass = stod (tag->getAttribute("mass"));
        frontalArea = stod (tag->getAttribute("frontalArea"));
        dragCoefficient = stod (tag->getAttribute("dragCoefficient"));
        ogreData->meshPath = "";
        //create main mesh
        ogreData->meshPath = getPath() + ogreData->meshPath;
        pOgreObject ogreObject (new OgreObject(this, ogreData, getId(), false));
        ogreObjects[ogreObject->getId()] = ogreObject;
        odeObjects[getName()] = pOdeObject(new OdeObject(this, data, getName()));
        ogreObjects[ogreObject->getId()]->setOdeReference(getMainOdeObject());
        //create child meshes
        XmlTag * t = tag->getTag(0); for (int i = 0; i < tag->nTags(); t = tag->getTag(++i))
        {
            if (t->getName() == "mesh")
            {
                pOgreObjectData childData(new OgreObjectData);
                childData->meshPath = getPath() + t->getAttribute("file");
                Vector3d posDiff (t->getAttribute("position"));
                Vector3d scale (t->getAttribute("scale"));
                Quaternion rotDiff (t->getAttribute("rotation"));
                pOgreObject ogreChild (new OgreObject(this, childData, getId()));
                ogreObjects[ogreChild->getId()] = ogreChild;
                ogreChild->setOgreReference(ogreObjects[ogreObject->getId()], rotDiff, posDiff, scale);
                System::get()->ogreWindow->update ();
            }
        }
    }

    // set the air drag variables correctly
    if (frontalArea == 0)
    {
        frontalArea = data->width * data->height * 0.6;
    }
}

Body::~Body ()
{
}

void Body::stepPhysics ()
{
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

    log->__format(LOG_DEVELOPER, "Body air drag force = (%f, %f, %f)", force.x, force.y, force.z);
}
void Body::setUserDriver ()
{
    userDriver = true;
}
