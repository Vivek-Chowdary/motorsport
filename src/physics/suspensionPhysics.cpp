/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "suspension.hpp"
#include "world.hpp"
#include "system.hpp"
#include "xmlParser.hpp"
#include "ode.h"
#include "log/logEngine.hpp"
#include "vector3d.hpp"
#include "vehicle.hpp"
#include "body.hpp"
#include "wheel.hpp"


void Suspension::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    springConstant = 0;
    dampingConstant = 0;
    position = new Vector3d (0, 0, 0);
    rotation = new Vector3d (0, 0, 0);
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
            if (attribute == "position")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the position: %s", attribute.c_str());
                *position = stov3d (attribute);
            }
            if (attribute == "rotation")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the rotation: %s", attribute.c_str());
                *rotation = stov3d (attribute);
            }
            if (attribute == "springConstant")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the suspension spring constant: %s", attribute.c_str() );
                springConstant = stod (attribute);
            }
            if (attribute == "dampingConstant")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the suspension damping constant: %s", attribute.c_str() );
                dampingConstant = stod (attribute);
            }
            attribute.clear();
        }
    }
    
    jointID = dJointCreateHinge (World::getWorldPointer()->worldID, 0);
//    jointID = dJointCreateHinge2 (World::getWorldPointer()->worldID, 0);
    dJointAttach (jointID, 0, 0);
    rotation->degreesToRadians();
}

void Suspension::attach (Wheel & wheel, Vehicle & vehicle)
{
    Vector3d position = *(this->position); //getPosition();
    Vector3d rotation = *(this->rotation); //getRotation();
    log->format (LOG_TRACE, "Attaching a wheel to this suspension (p=%f,%f,%f;r=%f,%f,%f)", position.x, position.y, position.z, rotation.x, rotation.y, rotation.z);
    wheel.setPosition (Vector3d (0, 0, 0));
    wheel.setRotation (rotation);
    wheel.setPosition (position);
    wheel.setSuspJoint (jointID);
//    dJointSetHinge2Param (jointID, dParamHiStop + dParamGroup * (1-1), 0.001);
//    dJointSetHinge2Param (jointID, dParamLoStop, 0);
    dJointAttach (jointID, wheel.wheelID, vehicle.body->bodyID);
 
//    dVector3 wheelAxisVector;
//    dJointGetHinge2Axis2 (jointID, wheelAxisVector);
//    dBodySetFiniteRotationAxis (wheel.wheelID, wheelAxisVector[0], wheelAxisVector[1], wheelAxisVector[2]);

//    double h = SystemData::getSystemDataPointer()->physicsTimeStep / 1000 ;
//    dJointSetHinge2Param (jointID, dParamSuspensionERP, h * springConstant / (h * springConstant + dampingConstant));
//    dJointSetHinge2Param (jointID, dParamSuspensionCFM, 1 / (h * springConstant + dampingConstant));
    setPosition(position);
    setRotation(rotation);
}

void Suspension::setPosition (Vector3d position)
{
//    dJointSetHinge2Anchor (jointID, position.x, position.y, position.z);
    dJointSetHingeAnchor (jointID, position.x, position.y, position.z);

}
void Suspension::setRotation (Vector3d rotation)
{
//    dJointSetHinge2Axis1 (jointID, 0, 0, 1);
    //dJointSetHinge2Axis2 (jointID, rotation.x, rotation.y, rotation.z);
//    dJointSetHinge2Axis2 (jointID, 0, 1, 0);
    dJointSetHingeAxis (jointID, rotation.x, rotation.y, rotation.z);
    dJointSetHingeAxis (jointID, 0,1,0);
}
Vector3d Suspension::getPosition ()
{
    const dReal *temp = dBodyGetRotation (dJointGetBody (jointID, 0));
    return Vector3d (temp[0], temp[1], temp[2]);
}
Vector3d Suspension::getRotation ()
{
    const dReal *temp = dBodyGetQuaternion (dJointGetBody (jointID, 0));
    return Vector3d (temp[0], temp[1], temp[2], temp[3]);
}

void Suspension::stopPhysics ()
{
    dJointDestroy (jointID);
}

void Suspension::stepPhysics ()
{
}
