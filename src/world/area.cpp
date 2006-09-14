/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#include "area.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "log/logEngine.hpp"
#include "system.hpp"
#include "camera.hpp"
#include "part.hpp"
#include "vehicle.hpp"
#include "ode/ode.h"
#include "location.hpp"
#include "world.hpp"
#include "SDL.h"

pArea Area::create (std::string areaName)
{
    pArea area(new Area(areaName));
    return area;
}
Area::Area (std::string areaName)
    :WorldObject("area")
{
    setPath(Paths::area(areaName));
    setXmlPath(Paths::areaXml(areaName));
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(getPath(), "FileSystem", "areas." + areaName);
    //Ogre::ResourceGroupManager::getSingleton().addResourceLocation(getPath() + "skybox.zip", "Zip", "areas."+areaName);
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("areas." + areaName);
    XmlTag * tag = new XmlTag (getXmlPath());
    construct (tag);
    delete tag;
}

Area::~Area ()
{
}

void Area::readCustomDataTag(XmlTag * tag)
{
    Vector3d checkpointPosition (0, 0, 0);
    double checkpointRadius = 5;
    std::string mesh = "none";

    setName (     tag->getAttribute("name"));
    description = tag->getAttribute("description");
    author =      tag->getAttribute("author");
    contact =     tag->getAttribute("contact");
    license =     tag->getAttribute("license");
    checkpointPosition = Vector3d (tag->getAttribute("checkpointPosition"));
    checkpointRadius = stod(tag->getAttribute("checkpointRadius"));
    XmlTag * t = tag->getTag(0); for (int i = 0; i < tag->nTags(); t = tag->getTag(++i))
    {
        if (t->getName() == "ground")
        {
            pOgreObjectData ogreData(new OgreObjectData);
            ogreData->meshPath = "";
            //create main mesh
            ogreData->meshPath = getPath() + ogreData->meshPath;
            pOgreObject ogreObject (new OgreObject(this, ogreData, getId(), false));
            ogreObjects[ogreObject->getId()] = ogreObject;
            //create child meshes
            XmlTag * u = t->getTag(0); for (int i = 0; i < t->nTags(); u = t->getTag(++i))
            {
                if (u->getName() == "mesh")
                {
                    //creating graphics mesh
                    log->__format (LOG_CCREATOR, "Creating an ogre mesh for the ground");
                    pOgreObjectData childData (new OgreObjectData);
                    childData->meshPath = getPath() + u->getAttribute("file");
                    Vector3d posDiff (u->getAttribute("position"));
                    Vector3d scale (u->getAttribute("scale"));
                    Quaternion rotDiff (u->getAttribute("rotation"));
                    pOgreObject ogreChild (new OgreObject(this, childData, getId()));
                    ogreObjects[ogreChild->getId()] = ogreChild;
                    ogreChild->setOgreReference(ogreObjects[ogreObject->getId()], rotDiff, posDiff, scale);
                    System::get()->ogreWindow->update ();
                    
                    //create physics mesh
                    pMeshOdeData odeData (new MeshOdeData());
                    odeData->ogreChild = ogreChild;
                    pOdeObject tmp (new OdeObject(this, odeData , getId()));
                    odeObjects[name+tmp->getId()] = tmp;
                }
            }
        }
        if (t->getName() == "parts")
        {
            Vector3d position(0,0,0);
            Quaternion rotation(1,0,0,0);
            XmlTag * u = t->getTag(0); for (int j = 0; j < t->nTags(); u = t->getTag(++j))
            {
                position = Vector3d (u->getAttribute("position"));
                rotation = Quaternion (u->getAttribute("rotation"));
                pPart tmp = Part::create (u->getName());
                tmp->setPosition (position);
                tmp->setRotation (rotation);
                objects[tmp->getName()] = tmp;
                tmp->stepGraphics();
                System::get()->ogreWindow->update ();
            }
        }
        if (t->getName() == "camera")
        {
            pCamera tmp = Camera::create (t);
            objects[tmp->getName()] = tmp;
        }
    }
    log->loadscreen (LOG_CCREATOR, "Creating the area ground");

    if (odeObjects.begin() == odeObjects.end()) 
    {
        pOdeObject tmp (new OdeObject(this, getId()));
        odeObjects[name] = tmp;
    }
    positionCameras(odeObjects.begin()->second->getBodyID());

    System::get()->ogreWindow->update ();

    // create the checkpoint sphere
    dGeomID checkpointID = dCreateSphere (World::get()->spaceID, checkpointRadius);
    dGeomSetBody (checkpointID, 0);
    dGeomSetPosition (checkpointID, checkpointPosition.x, checkpointPosition.y, checkpointPosition.z); 
}
void Area::construct (XmlTag * tag)
{
    constructFromTag(tag);
}

void Area::setCastShadows(bool castShadows)
{
    //UNCOMMENT ME
    //if (areaEntity != 0) areaEntity->setCastShadows(castShadows);
}

void Area::setRenderDetail(int renderMode)
{
    Ogre::PolygonMode mode;
    switch (renderMode)
    {
    case 1:
        mode = Ogre::PM_POINTS;
        break;
    case 2:
        mode = Ogre::PM_WIREFRAME;
        break;
    case 3:
        mode = Ogre::PM_SOLID;
        break;
    }
    WorldObjectsIt i = objects.begin();
    for(;i != objects.end(); i++)
    {
        if (pCamera tmp = boost::dynamic_pointer_cast<Camera>(i->second))
        {
            tmp->ogreCamera->setPolygonMode(mode);
        }
    }
}

pCamera Area::getClosestCamera(Vector3d location)
{
    log->__format(LOG_DEVELOPER, "Finding closest camera");
    double closestDistance = 99999999999.0;
    WorldObjectsIt i = objects.begin();
    pCamera closestCam;
    bool first = false;
    for (;i != objects.end(); i++)
    {
        if (pCamera tmp = boost::dynamic_pointer_cast<Camera>(i->second))
        {
            if (first == false)
            {
                closestCam = tmp;
                first = true;
            }
            log->__format(LOG_DEVELOPER, "Checking cam id: %s", tmp->getName().c_str());
            Ogre::Vector3 p = tmp->ogreCamera->getPosition();
            Vector3d cPos (p.x, p.y, p.z);
            double distance = cPos.distance(location);
            if (distance < closestDistance)
            {
                closestDistance = distance;
                closestCam = tmp;
            }
        }
    }
    activeCamera = closestCam;
    return activeCamera;
}
void Area::setPosition (Vector3d position)
{
    //WorldObject::setPosition(position);
    Vector3d posDiff =  position - getPosition();
    log->__format (LOG_DEVELOPER,"Difference in area position: (%f, %f, %f).", posDiff.x, posDiff.y, posDiff.z);
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        i->second->setPosition ( i->second->getPosition() + posDiff );
    }
    OdeObjectsIt j = odeObjects.begin();
    for(;j != odeObjects.end(); j++)
    {
        j->second->setPosition( j->second->getPosition() + posDiff);
    }
}
void Area::applyRotation (Quaternion rotation)
{
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        i->second->applyRotation (rotation);
    }
    OdeObjectsIt j = odeObjects.begin();
    for(;j != odeObjects.end(); j++)
    {
        j->second->setPosition ( rotation.rotateObject(j->second->getPosition()) );
        Quaternion finalRotation = rotation * j->second->getRotation();
        j->second->setRotation (finalRotation);
    }
}
