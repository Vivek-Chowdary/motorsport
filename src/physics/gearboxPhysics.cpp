
/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "gearbox.hpp"
#include "world.hpp"
#include "system.hpp"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"
#include "SDL/SDL_keysym.h"


void Gearbox::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    torqueTransfer = 0.0;
    revTorqueTransfer = 0.0;
    friction = 0.01;
    inertia = 1.0;
    angularVel = 0.0;
    revAngularVel = 0.0;
    angularAcc = 0.0;
    if (n->hasAttributes ())
    {
        DOMNamedNodeMap *attList = n->getAttributes ();
        int nSize = attList->getLength ();
        for (int i = 0; i < nSize; ++i)
        {
            DOMAttr *attNode = (DOMAttr *) attList->item (i);
            std::string attribute;
            assignXmlString (attribute, attNode->getName());
            if (attribute == "gearboxFriction")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the Gearbox Friction: %s", attribute.c_str() );
                friction = stod (attribute);
            }
            if (attribute == "gearboxInertia")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the Gearbox Inertia: %s", attribute.c_str() );
                inertia = stod (attribute);
            }
            if (attribute == "gear1Ratio")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the Gear 1 Ratio: %s", attribute.c_str() );
                gearRatio = stod (attribute);
            }
            attribute.clear();
        }
    }
}

void Gearbox::stopPhysics ()
{
}

void Gearbox::stepPhysics ()
{
    double dtOverJe;
    prevAngularVel = angularVel;

    dtOverJe=(SystemData::getSystemDataPointer()->physicsTimeStep/1000.0)/inertia;

    torqueTransfer = inputJoint->getTorque();
    revTorqueTransfer = outputJoint->getRevTorque()/gearRatio;

    angularVel = (dtOverJe*(inputJoint->getTorque()-outputJoint->getRevTorque())+prevAngularVel)/(1+(dtOverJe*friction));
    angularAcc = (angularVel-prevAngularVel)/SystemData::getSystemDataPointer()->physicsTimeStep/1000.0;
    revAngularVel = angularVel/gearRatio;

    log->format(LOG_TRACE, "inputTorque=%f outputTorque=%f inputVel=%f outputVel=%f", torqueTransfer, revTorqueTransfer, angularVel, revAngularVel);
}

