/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "part.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"
#include "system.hpp"
#include "ode/ode.h"
#include "world.hpp"
#include "area.hpp"
#include "SDL/SDL_keysym.h"

int Part::instancesCount = 0;

Part::Part (const std::string & partName)
    :WorldObject(partName)
{
    relativePartDir = partName;   
    std::string partPath = Paths::part(partName);
    std::string partXmlPath = Paths::partXml(partName);
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(partPath, "FileSystem", "Parts - " + partName);
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Parts - " + partName);
    XmlFile * xmlFile = new XmlFile (partXmlPath.c_str());
    processXmlRootNode (xmlFile->getRootNode());
    delete xmlFile;

    instancesCount++;
}

Part::~Part ()
{
    instancesCount--;

    stopPhysics ();
    stopGraphics ();
    stopInput ();
    
    log->__format(LOG_DEVELOPER, "Removed a part. %i left.", instancesCount);
}


void Part::updateOgrePosition ()
{
    const dReal *temp = dBodyGetPosition (partID);  // need to allocate memory first??
    partNode->setPosition (*(temp + 0), *(temp + 1), *(temp + 2));
}
void Part::updateOgreOrientation ()
{
    const dReal *temp = dBodyGetQuaternion (partID);    // need to allocate memory first??
    partNode->setOrientation (*(temp + 0), *(temp + 1), *(temp + 2), *(temp + 3));
}

void Part::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    DOMNode * partNode = n;
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string name;
            assignXmlString (name, n->getNodeName());
            if (name == "part")
            {
                log->__format (LOG_CCREATOR, "Found a part.");
                if (n->hasAttributes ())
                {
                    DOMNamedNodeMap *attList = n->getAttributes ();
                    int nSize = attList->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                        std::string attribute;
                        assignXmlString (attribute, attNode->getName());
                        if (attribute == "name")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the name: %s", attribute.c_str());
                        }
                        if (attribute == "description")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the description: %s", attribute.c_str());
                        }
                        if (attribute == "author")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the author: %s", attribute.c_str());
                        }
                        if (attribute == "contact")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the contact information: %s", attribute.c_str());
                        }
                        if (attribute == "license")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the license: %s", attribute.c_str());
                        }
                    }
                }
            }
        }
    }
    startGraphics(partNode);
    startPhysics(partNode);
    startInput ();
}

void Part::stepGraphics ()
{
    updateOgrePosition ();
    updateOgreOrientation ();
}

void Part::stopGraphics ()
{
    // empty
}

void Part::startGraphics (DOMNode * n)
{
    std::string author = "Anonymous";
    std::string contact = "None";
    std::string license = "Creative Commons Attribution-NonCommercial-ShareAlike License";
    std::string mesh = "None";
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
                assignXmlString (mesh, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the part graphics mesh filename: %s", mesh.c_str());
            }
        }
    }
    char number[256];
    sprintf (number, "%i", instancesCount);
    std::string name (relativePartDir + " #");
    name.append(number);
    std::string meshPath = Paths::part(relativePartDir) + mesh;
    partEntity = SystemData::getSystemDataPointer ()->ogreSceneManager->createEntity (name.c_str(), meshPath.c_str());
    partNode = static_cast < Ogre::SceneNode * >(SystemData::getSystemDataPointer ()->ogreSceneManager->getRootSceneNode ()->createChild ());
    partNode->attachObject (partEntity);
}
void Part::startInput ()
{

}

void Part::stopInput ()
{

}
void Part::stepInput ()
{

}
void Part::startPhysics (DOMNode * n)
{
    double mass = -1;
    std::string author = "Anonymous";
    std::string contact = "None";
    std::string license = "Creative Commons Attribution-NonCommercial-ShareAlike License";
    if (n->hasAttributes ())
    {
        // get all the attributes of the node
        DOMNamedNodeMap *attList = n->getAttributes ();
        int nSize = attList->getLength ();

        for (int i = 0; i < nSize; ++i)
        {
            DOMAttr *attNode = (DOMAttr *) attList->item (i);
            std::string attribute;
            assignXmlString (attribute, attNode->getName());
            if (attribute == "mass")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the part physics mass: %s", attribute.c_str() );
                mass = stod (attribute);
            }
        }
    }
    partID = dBodyCreate (World::getWorldPointer ()->worldID);
    if (mass <= 0)
    {
        log->__format (LOG_WARNING, "No mass has been defined for this part! Defaulting to 100kg.");
        mass = 100;
    }
    dMass dmass;
    std::string shape = "none";
    for (n = n->getFirstChild (); n != 0; n = n->getNextSibling ())
    {
        if (n)
        {
            if (n->getNodeType () == DOMNode::ELEMENT_NODE)
            {
                std::string name;
                assignXmlString (name, n->getNodeName());
                if (name == "box")
                {
                    shape = name;
                    log->__format (LOG_CCREATOR, "Found the part physics shape: %s.", name.c_str());
                    Vector3d dimensions (1, 1, 1);
                    DOMNamedNodeMap *attList = n->getAttributes ();
                    int nSize = attList->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                        std::string attribute;
                        assignXmlString (attribute, attNode->getName());
                        if (attribute == "dimensions")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the part dimensions: %s", attribute.c_str() );
                            dimensions = Vector3d (attribute);
                        }
                    }
                    partGeomID = dCreateBox (World::getWorldPointer ()->spaceID, dimensions.x, dimensions.y, dimensions.z);
                    dMassSetBoxTotal (&dmass, mass, dimensions.x, dimensions.y, dimensions.z);
                }
                if (name == "sphere")
                {
                    shape = name;
                    log->__format (LOG_CCREATOR, "Found the part physics shape: %s.", name.c_str());
                    double radius = 1;
                    DOMNamedNodeMap *attList = n->getAttributes ();
                    int nSize = attList->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                        std::string attribute;
                        assignXmlString (attribute, attNode->getName());
                        if (attribute == "radius")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the part radius: %s", attribute.c_str() );
                            radius = stod (attribute);
                        }
                    }
                    partGeomID = dCreateSphere (World::getWorldPointer ()->spaceID, radius);
                    dMassSetSphereTotal (&dmass, mass, radius);
                }
                if (name == "cappedCylinder")
                {
                    shape = name;
                    log->__format (LOG_CCREATOR, "Found the part physics shape: %s.", name.c_str());
                    double radius = 1;
                    double length = 1;
                    int directionAxis = 3;
                    DOMNamedNodeMap *attList = n->getAttributes ();
                    int nSize = attList->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                        std::string attribute;
                        assignXmlString (attribute, attNode->getName());
                        if (attribute == "radius")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the part radius: %s", attribute.c_str() );
                            radius = stod (attribute);
                        }
                        if (attribute == "length")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the part length: %s", attribute.c_str() );
                            length = stod (attribute);
                        }
                        if (attribute == "directionAxis")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the part length: %s", attribute.c_str() );
                            if (attribute == "x") directionAxis = 1;
                            if (attribute == "y") directionAxis = 2;
                            if (attribute == "z") directionAxis = 3;
                        }
                    }
                    partGeomID = dCreateCCylinder (World::getWorldPointer ()->spaceID, radius, length);
                    dMassSetCappedCylinderTotal (&dmass, mass, directionAxis, radius, length);
                }
            }
        }
    }
    if (shape == "none") log->__format(LOG_ERROR, "No physics shape specified for this part.");
    dGeomSetBody (partGeomID, partID);
    dBodySetMass (partID, &dmass);
}

void Part::setPosition (Vector3d position)
{
    dBodySetPosition (partID, position.x, position.y, position.z);
}

void Part::setRotation (Quaternion rotation)
{
    dMatrix3 rot;
    rotation.getOdeMatrix (rot);
    dBodySetRotation (partID, rot);
}

void Part::stopPhysics ()
{
    dGeomDestroy (partGeomID);
    dBodyDestroy (partID);
}

void Part::stepPhysics ()
{
    // //////////////simplified air friction (test)(should be forces!)
    dBodySetAngularVel (partID, (*(dReal *) (dBodyGetAngularVel (partID) + 0)) * (dReal) (0.999), (*(dReal *) (dBodyGetAngularVel (partID) + 1)) * (dReal) (0.999), (*(dReal *) (dBodyGetAngularVel (partID) + 2)) * (dReal) (0.999));
    // ////////////////////////////////////simplified air friction
    // applying user input [forces]
    if (this == World::getWorldPointer ()->areaList[0]->partList[0])
    {
        float moveX = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_l)]->getValue();
        float moveY = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_i)]->getValue();
        moveX *= 100;
        moveY *= 100;
        dBodyAddForce (partID, moveX, moveY, 0.0f);
        moveX = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_j)]->getValue();
        moveY = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_k)]->getValue();
        moveX *= 100;
        moveY *= 100;
        dBodyAddForce (partID, moveX, moveY, 0.0f);
        float moveZ = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_SPACE)]->getValue() * 500;
        dBodyAddForce (partID, 0, 0, moveZ);
    }
    const dReal * pos;
    pos = dBodyGetPosition(partID);
    log->__format(LOG_DEVELOPER, "part:x=%f y=%f z=%f", pos[0], pos[1], pos[2]);

}

