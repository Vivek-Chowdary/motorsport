/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "guiEngine.hpp"
#include "xmlParser.hpp"
#include "SDL/SDL.h"
#include "system.hpp"
#include "logEngine.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
using namespace Ogre;

GuiEngine::GuiEngine ()
{
    XmlFile * xmlFile = new XmlFile ("guiConfig.xml");
    processXmlRootNode (xmlFile->getRootNode());
    delete xmlFile;
    
    // get the direction of the graphics data
    log->put (LOG_INFO, "Setting up data pointers...");
    systemData = SystemData::getSystemDataPointer ();
}

int GuiEngine::computeStep (void)
//makes the graphics engine draw one frame
{
    log->put (LOG_VERBOSE, "Doing an step...");
    static Uint32 lastRefreshTime = 0;
    if (SDL_GetTicks () - lastRefreshTime >= 1000)
    {
        updateStatistics ();
        lastRefreshTime += 1000;
    }

    Overlay *overlay = (Overlay *) OverlayManager::getSingleton ().getByName ("gui");
    if (!overlay)
    {
        Except (Exception::ERR_ITEM_NOT_FOUND, "Could not find gui overlay", "statusPanel");
    }

    if (systemData->axisMap[getIDKeyboardKey(SDLK_f)]->getValue() == 1)
    {
        log->put (LOG_INFO, "Showing/hiding statistics on screen.");
        showStatistics = !showStatistics;
        systemData->axisMap[getIDKeyboardKey(SDLK_f)]->setNewRawValue(0); //no setRawValues should be out of the input engine; this must be done via filters that convert axis variations into 'events' FIXME
    }
    if (showStatistics)
        overlay->show ();
    else
        overlay->hide ();

    return (0);
}


void GuiEngine::updateStatistics (void)
{
    // update stats when necessary
    GuiElement *guiAvg = GuiManager::getSingleton ().getGuiElement ("gui/AverageFps");
    GuiElement *guiCurr = GuiManager::getSingleton ().getGuiElement ("gui/CurrFps");
    GuiElement *guiBest = GuiManager::getSingleton ().getGuiElement ("gui/BestFps");
    GuiElement *guiPhysics = GuiManager::getSingleton ().getGuiElement ("gui/PhysicsRate");
    const RenderTarget::FrameStats & stats = systemData->ogreWindow->getStatistics ();
    guiAvg->setCaption ("Average FPS: " + StringConverter::toString (stats.avgFPS));
    guiCurr->setCaption ("Current FPS: " + StringConverter::toString (systemData->graphicsFrequency));
    guiBest->setCaption ("Best FPS: " + StringConverter::toString (stats.bestFPS) + "FPS " + StringConverter::toString (stats.bestFrameTime) + " ms");
    guiPhysics->setCaption ("Physics Rate: " + StringConverter::toString (systemData->physicsFrequency) + "Hz " + StringConverter::toString (systemData->physicsTimeStep) + " ms");
    GuiElement *guiTris = GuiManager::getSingleton ().getGuiElement ("gui/NumTris");
    guiTris->setCaption ("Triangle Count: " + StringConverter::toString (stats.triangleCount));
    GuiElement *guiDbg = GuiManager::getSingleton ().getGuiElement ("gui/DebugText");
    guiDbg->setCaption (systemData->ogreWindow->getDebugText ());
}

GuiEngine::~GuiEngine (void)
{
    // finally stop the log engine
    delete log;
}

void GuiEngine::processXmlRootNode (DOMNode * n)
{
    LOG_LEVEL localLogLevel = LOG_TRACE;
    std::string localLogName = "GUI";
    showStatistics = true;
    LogEngine * tmpLog = new LogEngine (LOG_TRACE, "XML");
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string name;
            assignXmlString (name, n->getNodeName());
            tmpLog->format (LOG_INFO, "Name: %s", name.c_str());
            if (name == "guiConfig")
            {
                tmpLog->put (LOG_INFO, "Found the gui engine config element.");
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
                        if (attribute == "localLogLevel")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            localLogLevel = stologlevel (attribute);
                            tmpLog->format (LOG_INFO, "Found the local log level: %s", attribute.c_str());
                        }

                        if (attribute == "localLogName")
                        {
                            localLogName.clear();
                            assignXmlString (localLogName, attNode->getValue());
                            tmpLog->format (LOG_INFO, "Found the log name: %s", localLogName.c_str());

                        }
                        if (name == "showStatistics")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue ());
                            showStatistics = stob (attribute);
                            tmpLog->format (LOG_INFO, "Found whether to show the statistics or not: %s", attribute.c_str());
                        }
                        attribute.clear();
                    }
                }
            }
            name.clear();
        }
    }
    delete tmpLog;

    log = new LogEngine (localLogLevel, localLogName.c_str());
    localLogName.clear();
    log->put (LOG_INFO, "All config has been parsed");
}
