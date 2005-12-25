/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
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
#ifdef MACOSX
        XmlFile *xmlFile = new XmlFile ("motorsport.app/Contents/Resources/guiConfig.xml");
#else
        XmlFile *xmlFile = new XmlFile ("../cfg/guiConfig.xml");
#endif
        processXmlRootNode (xmlFile->getRootNode());
        delete xmlFile;

        // get the direction of the graphics data
        log->__format (LOG_DEVELOPER, "Setting up data pointers...");
        systemData = SystemData::getSystemDataPointer ();

        // updating singleton pointer
        guiEnginePointer = this;
        tmpOgreCamera = 0;
        loadscreenText = "";
        lapTime = "Lap Time: n/a";
        time = "Time: n/a";
    }
}

int GuiEngine::computeStep (void)
//makes the graphics engine draw one frame
{
    log->__format (LOG_DEVELOPER, "Doing an step...");
    //update all statistics every second
    static double lastRefreshTime = 0;
    if (SDL_GetTicks ()/1000.0 - lastRefreshTime >= 1)
    {
        updateStatistics ();
        lastRefreshTime += 1;
    }
    //update last telemetry line every frame
    OverlayElement *telText = OverlayManager::getSingleton ().getOverlayElement ("telemetry/text");
    telText->setCaption (telemetryText + tempLine);

    Overlay *overlay = (Overlay *) OverlayManager::getSingleton ().getByName ("gui");
    if (!overlay)
    {
        OGRE_EXCEPT (Exception::ERR_ITEM_NOT_FOUND, "Could not find gui overlay", "statusPanel");
    }
    Overlay *telemetryOverlay = (Overlay *) OverlayManager::getSingleton ().getByName ("telemetry");
    if (!telemetryOverlay)
    {
        OGRE_EXCEPT (Exception::ERR_ITEM_NOT_FOUND, "Could not find telemetry overlay", "statusPanel");
    }

    if (systemData->axisMap[getIDKeyboardKey(SDLK_g)]->getValue() == 1)
    {
        log->__format (LOG_ENDUSER, "Showing/hiding statistics on screen.");
        showStatistics = !showStatistics;
        systemData->axisMap[getIDKeyboardKey(SDLK_g)]->setNewRawValue(0); //no setRawValues should be out of the input engine; this must be done via filters that convert axis variations into 'events' FIXME
    }
    if (showStatistics)
    {
        if (telemetryText.size() != 0)
        {
            telemetryOverlay->show();
        }
        overlay->show ();
    } else {
        overlay->hide ();
        telemetryOverlay->hide ();
    }
    return (0);
}

void GuiEngine::showLoadscreen ()
{
    OverlayElement *loadscreenHeader = OverlayManager::getSingleton ().getOverlayElement ("loadscreen/header");
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
        Exception (Exception::ERR_ITEM_NOT_FOUND, "Could not find loadscreen overlay", "statusPanel");
    }
    loadscreenOverlay->show();
}
void GuiEngine::hideLoadscreen ()
{
    Overlay *loadscreenOverlay = (Overlay *) OverlayManager::getSingleton ().getByName ("loadscreen");
    if (!loadscreenOverlay)
    {
        Exception (Exception::ERR_ITEM_NOT_FOUND, "Could not find loadscreen overlay", "statusPanel");
    }
    loadscreenOverlay->hide();
    delete tmpOgreCamera;
    tmpOgreCamera = NULL;
    SystemData::getSystemDataPointer()->ogreWindow->removeViewport (-1);
}

void GuiEngine::updateStatistics (void)
{
    OverlayElement *guiAvg = OverlayManager::getSingleton ().getOverlayElement ("gui/AverageFps");
    OverlayElement *guiCurr = OverlayManager::getSingleton ().getOverlayElement ("gui/CurrFps");
    OverlayElement *guiScaledPhysics = OverlayManager::getSingleton ().getOverlayElement ("gui/ScaledPhysicsRate");
    OverlayElement *guiPhysics = OverlayManager::getSingleton ().getOverlayElement ("gui/PhysicsRate");
    const RenderTarget::FrameStats & stats = systemData->ogreWindow->getStatistics ();
    guiAvg->setCaption ("Average Graphics Rate: " + StringConverter::toString (stats.avgFPS) + " fps");
    guiCurr->setCaption ("Current Graphics Rate: " + StringConverter::toString ((float)systemData->graphicsFrequency) + " fps");
    guiScaledPhysics->setCaption ("Current Physics Rate: " + StringConverter::toString (float(systemData->getCurrentPhysicsFrequency())) + " Hz (" + StringConverter::toString (float(systemData->getCurrentPhysicsTimestep() * 1000.0)) + " ms)");
    guiPhysics->setCaption ("Desired Physics Rate: "       + StringConverter::toString (float(systemData->getDesiredPhysicsFrequency())) + " Hz (" + StringConverter::toString (float(systemData->getDesiredPhysicsTimestep() * 1000.0)) + " ms)" );
    OverlayElement *guiTris = OverlayManager::getSingleton ().getOverlayElement ("gui/NumTris");
    guiTris->setCaption ("Triangle Count: " + StringConverter::toString (stats.triangleCount));
    if (telemetryLines > 0)
    {
        OverlayElement *telText = OverlayManager::getSingleton ().getOverlayElement ("telemetry/text");
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
}

void GuiEngine::addTelemetryLine (const std::string & line)
{
    if (telemetryLines > 0)
    {
        if (telemetryText.size() == 0)
        {
            Overlay *telemetryOverlay = (Overlay *) OverlayManager::getSingleton ().getByName ("telemetry");
            telemetryOverlay->show();
            OverlayElement *telHeader = OverlayManager::getSingleton ().getOverlayElement ("telemetry/header");
            telHeader->setCaption (line);
            telemetryText.append (line);
            telemetryText.append ("\n");
        }
        tempLine.assign (line);
    }
}

void GuiEngine::addLoadscreenLine (const std::string & line)
{
    static unsigned lines = 0;
    const unsigned int nchars = 85;
    const unsigned int nlines = 26;
    std::string tmp = loadscreenText;
    loadscreenText.clear();
    std::string tmpline = line;
    while (tmpline.size() > nchars)
    {
        int cutPosition = tmpline.rfind(' ', nchars);
        if ( cutPosition < 1)
        {
            cutPosition = nchars;
        }
        loadscreenText.append (tmpline.substr(0, cutPosition));
        loadscreenText.append ("\n  ");
        lines++;
        tmpline = tmpline.substr(cutPosition);
    }
    loadscreenText.append (tmpline);
    loadscreenText.append ("\n");
    lines++;
    loadscreenText.append (tmp);
    OverlayElement *loadText = OverlayManager::getSingleton ().getOverlayElement ("loadscreen/text");
    loadText->setCaption (loadscreenText);
    while (lines > nlines)
    {
        int lastline = loadscreenText.rfind ('\n');
        loadscreenText = loadscreenText.substr(0, lastline);
        lines--;
    }
    SystemData::getSystemDataPointer()->ogreWindow->update ();
}
void GuiEngine::updateLapTime (double time)
{
    OverlayElement *guiLapTime = OverlayManager::getSingleton ().getOverlayElement ("gui/LapTime");
    double tmpTime = time;
    int minutes = int (tmpTime / 60);
    tmpTime = tmpTime - (minutes * 60);
    guiLapTime->setCaption ("Lap Time: " + StringConverter::toString(float(time)) + " s (" + StringConverter::toString(minutes) + "' " + StringConverter::toString(float(tmpTime)) + "\")");
}
void GuiEngine::updateTime (double time)
{
    OverlayElement *guiLapTime = OverlayManager::getSingleton ().getOverlayElement ("gui/Time");
    guiLapTime->setCaption ("Time: " + StringConverter::toString (float(time)) + " s");
}


GuiEngine::~GuiEngine (void)
{
    // finally stop the log engine
    delete log;
}

void GuiEngine::processXmlRootNode (DOMNode * n)
{
    LOG_LEVEL localLogLevel = LOG_DEVELOPER;
    std::string localLogName = "GUI";
    showStatistics = true;
    LogEngine * tmpLog = new LogEngine (LOG_DEVELOPER, "XML Parser");
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string name;
            assignXmlString (name, n->getNodeName());
            tmpLog->__format (LOG_DEVELOPER, "Name: %s", name.c_str());
            if (name == "guiConfig")
            {
                tmpLog->__format (LOG_DEVELOPER, "Found the gui engine config element.");
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
                            assignXmlString (attribute, attNode->getValue());
                            localLogLevel = stologlevel (attribute);
                            tmpLog->__format (LOG_ENDUSER, "Found the local log level: %s", attribute.c_str());
                        }

                        if (attribute == "localLogName")
                        {
                            assignXmlString (localLogName, attNode->getValue());
                            tmpLog->__format (LOG_ENDUSER, "Found the log name: %s", localLogName.c_str());

                        }
                        if (attribute == "showStatistics")
                        {
                            assignXmlString (attribute, attNode->getValue ());
                            showStatistics = stob (attribute);
                            tmpLog->__format (LOG_ENDUSER, "Found whether to show the statistics or not: %s", attribute.c_str());
                        }
                        if (attribute == "telemetryLines")
                        {
                            assignXmlString (attribute, attNode->getValue ());
                            telemetryLines = stoi (attribute);
                            tmpLog->__format (LOG_ENDUSER, "Found the number of telemetry lines: %s", attribute.c_str());
                        }
                    }
                }
            }
        }
    }
    delete tmpLog;

    log = new LogEngine (localLogLevel, localLogName.c_str());
    log->__format (LOG_DEVELOPER, "All config has been parsed");
}
