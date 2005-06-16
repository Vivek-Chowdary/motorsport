/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "driveJoint.hpp"
#include "world.hpp"
#include "system.hpp"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"
#include "SDL/SDL_keysym.h"


void Clutch::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    outputTorqueTransfer = 0.0;
    inputTorqueTransfer = 0.0;
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
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the clutch static coefficient of friction: %s", attribute.c_str() );
                coeffStaticFriction = stod (attribute);
            }
            if (attribute == "coeffDynamicFriction")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the clutch dynamic coefficient of friction: %s", attribute.c_str() );
                coeffDynamicFriction = stod (attribute);
            }
            if (attribute == "maxTorqueTransfer")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the clutch maximum torque transfer: %s", attribute.c_str() );
                maxTorqueTransfer = stod (attribute);
            }
        }
    }
}

void Clutch::stopPhysics ()
{
}

void Clutch::stepPhysics ()
{
  if(enabled) {
    double clutch = 0;
    clutch = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP7)]->getValue() * 3 / 3;
    if (clutch == 0) {
        clutch = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP4)]->getValue() * 2 / 3;
        if (clutch == 0) {
            clutch = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP1)]->getValue() * 1 / 3;
            if (clutch == 0) {
                clutch = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_q)]->getValue() * 3 / 3;
    }   }   }
    if(clutch) {
        outputTorqueTransfer = 0*(inputDrive->getOutputAngularVel()-outputDrive->getInputAngularVel());
        inputTorqueTransfer = outputTorqueTransfer*-1;
    }
    else {
        outputTorqueTransfer = lockedParam*(inputDrive->getOutputAngularVel()-outputDrive->getInputAngularVel());
        log->format(LOG_DEVELOPER, "torqueTransfer=%f", outputTorqueTransfer);

        if(outputTorqueTransfer > maxTorqueTransfer) {
                outputTorqueTransfer = maxTorqueTransfer;
        }
        else if(outputTorqueTransfer < -1*maxTorqueTransfer) {
                outputTorqueTransfer = -1 * maxTorqueTransfer;
        }
        inputTorqueTransfer = outputTorqueTransfer*-1;
    }       

    inputDrive->addOutputTorque(inputTorqueTransfer);
    outputDrive->addInputTorque(outputTorqueTransfer);   

 //   rotationalVelocity = pOutTorque->getInputAngularVel();
    log->format(LOG_DEVELOPER, "inputTorque=%f outputTorque=%f inputVel=%f outputVel=%f", inputTorqueTransfer, outputTorqueTransfer,inputDrive->getOutputAngularVel(),outputDrive->getInputAngularVel());
  }
}


void Gear::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    outputTorqueTransfer = 0.0;
    inputTorqueTransfer = 0.0;
    ratio = 1.0;
    springConstant = 1000;
    relAngle = 0.0;
      
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
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the clutch static coefficient of friction: %s", attribute.c_str() );
//                coeffStaticFriction = stod (attribute);
            }
            if (attribute == "coeffDynamicFriction")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the clutch dynamic coefficient of friction: %s", attribute.c_str() );
//                coeffDynamicFriction = stod (attribute);
            }
            if (attribute == "maxTorqueTransfer")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the clutch maximum torque transfer: %s", attribute.c_str() );
//                maxTorqueTransfer = stod (attribute);
            }
        }
    }
}

void Gear::stopPhysics ()
{
}

void Gear::stepPhysics ()
{
  if(enabled) {
    double dt;
    dt = SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep();
    prevRelAngle = relAngle;
    prevRelAngularVel = relAngularVel;

    relAngularVel = inputDrive->getOutputAngularVel()/ratio - outputDrive->getInputAngularVel();

    // trapezoidal integration
    relAngle = prevRelAngle + dt / 2 * (prevRelAngularVel + relAngularVel); 

//    relAngle = relAngle + inputDrive->getOutputAngularVel()*dt/ratio - outputDrive->getInputAngularVel()*dt;
    
    outputTorqueTransfer = springConstant*relAngle+dampConstant*relAngularVel;
        
    inputTorqueTransfer = -1*outputTorqueTransfer/ratio;
    
    inputDrive->addOutputTorque(inputTorqueTransfer);
    outputDrive->addInputTorque(outputTorqueTransfer);   
             
    log->format(LOG_DEVELOPER, "inTorque=%f outTorque=%f relVel=%f relAngle=%f", inputTorqueTransfer, outputTorqueTransfer,relAngularVel,relAngle);
  }
  else {
    relAngle = 0;
  }
}

void LSD::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    outputTorqueTransfer = 0.0;
    inputTorqueTransfer = 0.0;
    ratio = 1.0;
    springConstant = 1000;
    relAngle = 0.0;
      
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
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the clutch static coefficient of friction: %s", attribute.c_str() );
//                coeffStaticFriction = stod (attribute);
            }
            if (attribute == "coeffDynamicFriction")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the clutch dynamic coefficient of friction: %s", attribute.c_str() );
//                coeffDynamicFriction = stod (attribute);
            }
            if (attribute == "maxTorqueTransfer")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the clutch maximum torque transfer: %s", attribute.c_str() );
//                maxTorqueTransfer = stod (attribute);
            }
        }
    }
}

void LSD::stopPhysics ()
{
}

void LSD::stepPhysics ()
{
  if(enabled) {
    double dt;
    dt = SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep();
    prevRelAngle = relAngle;
    prevRelAngularVel = relAngularVel;

    relAngularVel = inputDrive->getOutputAngularVel()/ratio - (outputDrive->getInputAngularVel()+outputDrive2->getInputAngularVel())/2;

    // trapezoidal integration
    relAngle = prevRelAngle + dt / 2 * (prevRelAngularVel + relAngularVel); 

//    relAngle = relAngle + inputDrive->getOutputAngularVel()*dt/ratio - outputDrive->getInputAngularVel()*dt;
    
    outputTorqueTransfer = springConstant*relAngle+dampConstant*relAngularVel;
        
    inputTorqueTransfer = -1*outputTorqueTransfer/ratio;
    
    inputDrive->addOutputTorque(inputTorqueTransfer);
    outputDrive->addInputTorque(outputTorqueTransfer/2);
    outputDrive2->addInputTorque(outputTorqueTransfer/2);   
             
    log->format(LOG_DEVELOPER, "inTorque=%f outTorque=%f relVel=%f relAngle=%f", inputTorqueTransfer, outputTorqueTransfer,relAngularVel,relAngle);
  }
  else {
    relAngle = 0;
  }
}
