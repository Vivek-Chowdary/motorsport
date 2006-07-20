/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "gui.hpp"
#include "SDL/SDL.h"
#include "system.hpp"
#include "logEngine.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "xmlTag.hpp"
using namespace Ogre;

pGui Gui::gui;

pGui Gui::get()
{
    if (!gui)
    {
        new Gui();
    }
    return (gui);
}
void Gui::destroy()
{
    if (!gui)
    {
        std::cout << "WARNING: There's no gui to be destroyed!" << std::endl;
    }
    else
    {
        gui.reset();
    }
}

Gui::Gui ()
{
    if (gui)
    {
        gui->log->__format(LOG_ERROR, "Tried to create another gui. This shouldn't have happened.");
    } else {
        pGui tmp(this);
        gui = tmp;
#ifdef MACOSX
        XmlTag * tag = new XmlTag ("motorsport.app/Contents/Resources/guiConfig.xml");
#else
        XmlTag * tag = new XmlTag ("../cfg/guiConfig.xml");
#endif
        showStatistics = true;
        log = LogEngine::create (LOG_DEVELOPER, "Gui");

        if (tag->getName() == "guiConfig")
        {
            showStatistics = stob(tag->getAttribute("showStatistics"));
            telemetryLines = stoi(tag->getAttribute("telemetryLines"));
        }
        delete tag;

        // updating singleton pointer
        tmpOgreCamera = 0;
        loadscreenText = "";
        lapTime = "Lap Time: n/a";
        time = "Time: n/a";
    }
}

int Gui::computeStep (void)
//makes the graphics engine draw one frame
{
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

    if (System::get()->axisMap[getIDKeyboardKey(SDLK_g)]->getValue() == 1)
    {
        log->__format (LOG_ENDUSER, "Showing/hiding statistics on screen.");
        showStatistics = !showStatistics;
        System::get()->axisMap[getIDKeyboardKey(SDLK_g)]->setNewRawValue(0); //no setRawValues should be out of the input engine; this must be done via filters that convert axis variations into 'events' FIXME
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

void Gui::showLoadscreen ()
{
    OverlayElement *loadscreenHeader = OverlayManager::getSingleton ().getOverlayElement ("loadscreen/header");
    loadscreenHeader->setCaption ("Loading contents and engines. Please wait...");
        
    tmpOgreCamera = System::get()->ogreSceneManager->createCamera ("Loadscreen camera");
    tmpOgreCamera->setFixedYawAxis (true, Ogre::Vector3 (0, 0, 1));
    tmpOgreCamera->setPosition (Ogre::Vector3 (-3, 3, 1));
    tmpOgreCamera->lookAt (Ogre::Vector3 (0, 0, 0));
    tmpOgreCamera->setNearClipDistance (0.100);
    System::get()->ogreWindow->removeAllViewports ();
    System::get()->ogreWindow->addViewport (tmpOgreCamera, -1);

    Overlay *loadscreenOverlay = (Overlay *) OverlayManager::getSingleton ().getByName ("loadscreen");
    if (!loadscreenOverlay)
    {
        Exception (Exception::ERR_ITEM_NOT_FOUND, "Could not find loadscreen overlay", "statusPanel");
    }
    loadscreenOverlay->show();
}
void Gui::hideLoadscreen ()
{
    Overlay *loadscreenOverlay = (Overlay *) OverlayManager::getSingleton ().getByName ("loadscreen");
    if (!loadscreenOverlay)
    {
        Exception (Exception::ERR_ITEM_NOT_FOUND, "Could not find loadscreen overlay", "statusPanel");
    }
    loadscreenOverlay->hide();
    delete tmpOgreCamera;
    tmpOgreCamera = NULL;
    System::get()->ogreWindow->removeViewport (-1);
}

void Gui::updateStatistics (void)
{
    OverlayElement *guiAvg = OverlayManager::getSingleton ().getOverlayElement ("gui/AverageFps");
    OverlayElement *guiCurr = OverlayManager::getSingleton ().getOverlayElement ("gui/CurrFps");
    OverlayElement *guiScaledPhysics = OverlayManager::getSingleton ().getOverlayElement ("gui/ScaledPhysicsRate");
    OverlayElement *guiPhysics = OverlayManager::getSingleton ().getOverlayElement ("gui/PhysicsRate");
    const RenderTarget::FrameStats & stats = System::get()->ogreWindow->getStatistics ();
    guiAvg->setCaption ("Average Graphics Rate: " + StringConverter::toString (stats.avgFPS) + " fps");
    guiCurr->setCaption ("Current Graphics Rate: " + StringConverter::toString ((float)System::get()->graphicsFrequency) + " fps");
    guiScaledPhysics->setCaption ("Current Physics Rate: " + StringConverter::toString (float(System::get()->getCurrentPhysicsFrequency())) + " Hz (" + StringConverter::toString (float(System::get()->getCurrentPhysicsTimestep() * 1000.0)) + " ms)");
    guiPhysics->setCaption ("Desired Physics Rate: "       + StringConverter::toString (float(System::get()->getDesiredPhysicsFrequency())) + " Hz (" + StringConverter::toString (float(System::get()->getDesiredPhysicsTimestep() * 1000.0)) + " ms)" );
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

void Gui::addTelemetryLine (const std::string & line)
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

void Gui::addLoadscreenLine (const std::string & line)
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
    System::get()->ogreWindow->update ();
}
void Gui::updateLapTime (double time)
{
    OverlayElement *guiLapTime = OverlayManager::getSingleton ().getOverlayElement ("gui/LapTime");
    double tmpTime = time;
    int minutes = int (tmpTime / 60);
    tmpTime = tmpTime - (minutes * 60);
    guiLapTime->setCaption ("Lap Time: " + StringConverter::toString(float(time)) + " s (" + StringConverter::toString(minutes) + "' " + StringConverter::toString(float(tmpTime)) + "\")");
}
void Gui::updateTime (double time)
{
    OverlayElement *guiLapTime = OverlayManager::getSingleton ().getOverlayElement ("gui/Time");
    guiLapTime->setCaption ("Time: " + StringConverter::toString (float(time)) + " s");
}


Gui::~Gui (void)
{
}
