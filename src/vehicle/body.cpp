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

Body::Body (WorldObject * container, std::string name, XERCES_CPP_NAMESPACE::DOMNode * n)
    :WorldObject(container, name)
{
    processXmlRootNode (n);
}

Body::~Body ()
{
}


void Body::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    startPhysics (n);
    startGraphics (n);
    ogreObjects.begin()->second->setOdeReference(getMainOdeObject());
}
void Body::startGraphics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    OgreObjectData data;
    if (n->hasAttributes ())
    {
        DOMNamedNodeMap *attList = n->getAttributes ();
        int nSize = attList->getLength ();
        for (int i = 0; i < nSize; ++i)
        {
            DOMAttr *attNode = (DOMAttr *) attList->item (i);
            std::string attribute;
            assignXmlString (attribute, attNode->getName());
            if (attribute == "mesh")
            {
                assignXmlString (data.meshPath, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the body graphics mesh filename: %s", data.meshPath.c_str());
            }
        }
    }
    data.meshPath = Paths::vehicle(container->getName()) + data.meshPath;
    OgreObject * ogreObject = new OgreObject(this, data, getId());
    ogreObjects[getId()] = ogreObject;
}

void Body::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    VehicleBodyOdeObjectData data;
    dragCoefficient = 0.3;
    frontalArea = 0;
    if (n->hasAttributes ())
    {
        DOMNamedNodeMap *attList = n->getAttributes ();
        int nSize = attList->getLength ();
        for (int i = 0; i < nSize; ++i)
        {
            DOMAttr *attNode = (DOMAttr *) attList->item (i);
            std::string attribute;
            assignXmlString (attribute, attNode->getName());
            if (attribute == "length")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the body physics length: %s", attribute.c_str() );
                data.length = stod (attribute);
            }
            if (attribute == "width")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the body physics width: %s", attribute.c_str() );
                data.width = stod (attribute);
            }
            if (attribute == "height")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the body physics height: %s", attribute.c_str() );
                data.height = stod (attribute);
            }
            if (attribute == "mass")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the body physics mass: %s", attribute.c_str() );
                data.mass = stod (attribute);
            }
            if (attribute == "frontalArea")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the body frontal area: %s", attribute.c_str() );
                frontalArea = stod (attribute);
            }
            if (attribute == "dragCoefficient")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the body drag coefficient: %s", attribute.c_str() );
                dragCoefficient = stod (attribute);
            }
        }
    }
    odeObjects[getId()] = new OdeObject(this, data, getId());

    // set the air drag variables correctly
    if (frontalArea == 0)
    {
        frontalArea = data.width * data.height * 0.6;
    }
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
