/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "clutch.hpp"
#include "world.hpp"
#include "system.hpp"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"
#include "SDL/SDL_keysym.h"


void Clutch::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    torqueTransfer = 0.0;
    revTorqueTransfer = 0.0;
    lockedParam = 1;
     
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
            if (attribute == "coeffStaticFriction")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the clutch static coefficient of friction: %s", attribute.c_str() );
                coeffStaticFriction = stod (attribute);
            }
            if (attribute == "coeffDynamicFriction")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the clutch dynamic coefficient of friction: %s", attribute.c_str() );
                coeffDynamicFriction = stod (attribute);
            }
            if (attribute == "maxTorqueTransfer")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the clutch maximum torque transfer: %s", attribute.c_str() );
                maxTorqueTransfer = stod (attribute);
            }
            attribute.clear();
        }
    }
}

void Clutch::stopPhysics ()
{
}

void Clutch::stepPhysics ()
{
    if(SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_g)]->getValue()) {
        torqueTransfer = 0*(inputDrive->getRevAngularVel()-outputDrive->getAngularVel());
        revTorqueTransfer = torqueTransfer;
    }
    else {
        torqueTransfer = lockedParam*(inputDrive->getRevAngularVel()-outputDrive->getAngularVel());
        log->format(LOG_TRACE, "torqueTransfer=%f", torqueTransfer);

        if(torqueTransfer > maxTorqueTransfer) {
                torqueTransfer = maxTorqueTransfer;
        }
        else if(torqueTransfer < -1*maxTorqueTransfer) {
                torqueTransfer = -1 * maxTorqueTransfer;
        }
        revTorqueTransfer = torqueTransfer;
    }       

 //   rotationalVelocity = pOutTorque->getAngularVel();
    log->format(LOG_TRACE, "inputTorque=%f outputTorque=%f inputVel=%f outputVel=%f", revTorqueTransfer, torqueTransfer,inputDrive->getRevAngularVel(),outputDrive->getAngularVel());
}

