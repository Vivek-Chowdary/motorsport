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

GuiEngine *GuiEngine::guiEnginePointer = NULL;

GuiEngine *GuiEngine::getGuiEnginePointer ()
{
    return (guiEnginePointer);
}

GuiEngine::GuiEngine ()
{
    if (guiEnginePointer != 0)
    {
        delete this;
    } else {
        XmlFile * xmlFile = new XmlFile ("guiConfig.xml");
        processXmlRootNode (xmlFile->getRootNode());
        delete xmlFile;

        // get the direction of the graphics data
        log->put (LOG_INFO, "Setting up data pointers...");
        systemData = SystemData::getSystemDataPointer ();

        // updating singleton pointer
        guiEnginePointer = this;
        tmpOgreCamera = 0;
        loadscreenText = "";
    }
}

int GuiEngine::computeStep (void)
//makes the graphics engine draw one frame
{
    log->put (LOG_VERBOSE, "Doing an step...");
    //update all statistics every second
    static Uint32 lastRefreshTime = 0;
    if (SDL_GetTicks () - lastRefreshTime >= 1000)
    {
        updateStatistics ();
        lastRefreshTime += 1000;
    }
    //update last telemetry line every frame
    GuiElement *telText = GuiManager::getSingleton ().getGuiElement ("telemetry/text");
    telText->setCaption (telemetryText + tempLine);

    Overlay *overlay = (Overlay *) OverlayManager::getSingleton ().getByName ("gui");
    if (!overlay)
    {
        Except (Exception::ERR_ITEM_NOT_FOUND, "Could not find gui overlay", "statusPanel");
    }
    Overlay *telemetryOverlay = (Overlay *) OverlayManager::getSingleton ().getByName ("telemetry");
    if (!telemetryOverlay)
    {
        Except (Exception::ERR_ITEM_NOT_FOUND, "Could not find telemetry overlay", "statusPanel");
    }

    if (systemData->axisMap[getIDKeyboardKey(SDLK_f)]->getValue() == 1)
    {
        log->put (LOG_INFO, "Showing/hiding statistics on screen.");
        showStatistics = !showStatistics;
        systemData->axisMap[getIDKeyboardKey(SDLK_f)]->setNewRawValue(0); //no setRawValues should be out of the input engine; this must be done via filters that convert axis variations into 'events' FIXME
    }
    if (showStatistics)
    {
        overlay->show ();
        telemetryOverlay->show ();
    } else {
        overlay->hide ();
        telemetryOverlay->hide ();
    }
    return (0);
}

void GuiEngine::showLoadscreen ()
{
    GuiElement *loadscreenHeader = GuiManager::getSingleton ().getGuiElement ("loadscreen/header");
    loadscreenHeader->setCaption ("Loading contents and engines. Please wait...");
        
    tmpOgreCamera = SystemData::getSystemDataPointer ()->ogreSceneManager->createCamera ("Loadscreen camera");
    tmpOgreCamera->setFixedYawAxis (true, Ogre::Vector3 (0, 0, 1));
    tmpOgreCamera->setPosition (Ogre::Vector3 (30, 30, 10));
    tmpOgreCamera->lookAt (Ogre::Vector3 (0, 0, 0));
    tmpOgreCamera->setNearClipDistance (0.100);
    SystemData::getSystemDataPointer()->ogreWindow->removeAllViewports ();
    SystemData::getSystemDataPointer()->ogreWindow->addViewport (tmpOgreCamera, -1);

    Overlay *loadscreenOverlay = (Overlay *) OverlayManager::getSingleton ().getByName ("loadscreen");
    if (!loadscreenOverlay)
    {
        Except (Exception::ERR_ITEM_NOT_FOUND, "Could not find loadscreen overlay", "statusPanel");
    }
    loadscreenOverlay->show();
}
void GuiEngine::hideLoadscreen ()
{
    Overlay *loadscreenOverlay = (Overlay *) OverlayManager::getSingleton ().getByName ("loadscreen");
    if (!loadscreenOverlay)
    {
        Except (Exception::ERR_ITEM_NOT_FOUND, "Could not find loadscreen overlay", "statusPanel");
    }
    loadscreenOverlay->hide();
    delete tmpOgreCamera;
    tmpOgreCamera = NULL;
    SystemData::getSystemDataPointer()->ogreWindow->removeViewport (-1);
}

void GuiEngine::updateStatistics (void)
{
    GuiElement *guiAvg = GuiManager::getSingleton ().getGuiElement ("gui/AverageFps");
    GuiElement *guiCurr = GuiManager::getSingleton ().getGuiElement ("gui/CurrFps");
    GuiElement *guiScaledPhysics = GuiManager::getSingleton ().getGuiElement ("gui/ScaledPhysicsRate");
    GuiElement *guiPhysics = GuiManager::getSingleton ().getGuiElement ("gui/PhysicsRate");
    const RenderTarget::FrameStats & stats = systemData->ogreWindow->getStatistics ();
    guiAvg->setCaption ("Average Graphics Rate: " + StringConverter::toString (stats.avgFPS) + " fps");
    guiCurr->setCaption ("Current Graphics Rate: " + StringConverter::toString (systemData->graphicsFrequency) + " fps");
    guiScaledPhysics->setCaption ("Scaled Physics Rate: " + StringConverter::toString (systemData->physicsFrequency) + " Hz (" + StringConverter::toString (1000 / float(systemData->physicsFrequency)) + " ms)");
    guiPhysics->setCaption ("Simulated Physics Rate: " + StringConverter::toString (1000 / float(systemData->physicsTimeStep)) + "Hz (" + StringConverter::toString (systemData->physicsTimeStep) + " ms)" );
    GuiElement *guiTris = GuiManager::getSingleton ().getGuiElement ("gui/NumTris");
    guiTris->setCaption ("Triangle Count: " + StringConverter::toString (stats.triangleCount));
    
    GuiElement *telText = GuiManager::getSingleton ().getGuiElement ("telemetry/text");
    telemetryText.append (tempLine);
    if (telemetryText.size() == 0)
    {
        telText->setCaption ("Telemetry not active.");
    } else {
        telemetryText.append ("\n");
        int num = 0;
        int lastLineEnd = telemetryText.rfind ('\n', telemetryText.size()-1);
        while (lastLineEnd >= 0)
        {
            num++;
            if (num > telemetryLines-1)
            {
                telemetryText.erase (0, lastLineEnd+1);
                lastLineEnd = -1;
            } else {
                lastLineEnd = telemetryText.rfind ('\n', lastLineEnd-1);
            }
        }
        telText->setCaption (telemetryText + tempLine);
    }
}

void GuiEngine::addTelemetryLine (char * line)
{
    if (telemetryText.size() == 0)
    {
        GuiElement *telHeader = GuiManager::getSingleton ().getGuiElement ("telemetry/header");
        telHeader->setCaption (line);
        telemetryText.append (line);
    }
    tempLine.assign (line);
}

void GuiEngine::addLoadscreenLine (char * line)
{
    static unsigned lines = 0;
    const unsigned int nchars = 85;
    const unsigned int nlines = 26;
    std::string tmp = loadscreenText;
    loadscreenText.clear();
    std::string tmpline = line;
    while (tmpline.size() > nchars)
    {
        loadscreenText.append (tmpline.substr(0, tmpline.rfind(' ', nchars)));
        loadscreenText.append ("\n  ");
        lines++;
        tmpline = tmpline.substr(tmpline.rfind(' ', nchars));
    }
    loadscreenText.append (tmpline);
    loadscreenText.append ("\n");
    lines++;
    loadscreenText.append (tmp);
    GuiElement *loadText = GuiManager::getSingleton ().getGuiElement ("loadscreen/text");
    loadText->setCaption (loadscreenText);
    while (lines > nlines)
    {
        int lastline = loadscreenText.rfind ('\n');
        loadscreenText = loadscreenText.substr(0, lastline);
        lines--;
    }
    SystemData::getSystemDataPointer()->ogreWindow->update ();
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
                        if (attribute == "showStatistics")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue ());
                            showStatistics = stob (attribute);
                            tmpLog->format (LOG_INFO, "Found whether to show the statistics or not: %s", attribute.c_str());
                        }
                        if (attribute == "telemetryLines")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue ());
                            telemetryLines = stoi (attribute);
                            tmpLog->format (LOG_INFO, "Found the number of telemetry lines: %s", attribute.c_str());
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
