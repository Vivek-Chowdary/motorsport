
/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright © 2000-2002 The OGRE Team
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
Filename:    ExampleFrameListener.h
Description: Defines an example frame listener which responds to frame events.
             This frame listener just moves a specified camera around based on
             keyboard and mousex movements.
             Mouse:    Freelook
             W or Up:  Forward
             S or Down:Backward
             A:           Step left
             D:        Step right
             PgUp:     Move upwards
             PgDown:   Move downwards
             O/P:       Yaw the root scene node (and it's children)
             I/K:       Pitch the root scene node (and it's children)
             F:           Toggle frame rate stats on/off
             R:        Render mode
             T:        Cycle texture filtering
                       Bilinear, Trilinear, Anisotropic(8)
-----------------------------------------------------------------------------
*/

#ifndef SIM_FRAMELISTENER_HPP
#    define SIM_FRAMELISTENER_HPP

#    include "Ogre.h"
#    include "OgreKeyEvent.h"
#    include "OgreEventListeners.h"
#    include "OgreStringConverter.h"
#    include "OgreException.h"

using namespace Ogre;

class SimFrameListener:public FrameListener,
      public KeyListener
{
  private:
    int mSceneDetailIndex;

    void updateStats ( void );

  public:
    // Constructor takes a RenderWindow because it uses that to determine input context

         SimFrameListener ( RenderWindow * win,
                            Camera * cam,
                            bool useBufferedInputKeys,
                            bool useBufferedInputMouse );
         virtual ~ SimFrameListener (  );
    virtual bool processUnbufferedKeyInput ( const FrameEvent & evt );
    bool processUnbufferedMouseInput ( const FrameEvent & evt );
    void moveCamera (  );
    void showDebugOverlay ( bool show );
    // Override frameStarted event to process that (don't care about frameEnded)
    bool frameStarted ( const FrameEvent & evt );
    bool frameEnded ( const FrameEvent & evt );
    void switchMouseMode (  );
    void switchKeyMode (  );
    void keyClicked ( KeyEvent * e );
    void keyPressed ( KeyEvent * e );
    void keyReleased ( KeyEvent * e );

  protected:
         EventProcessor * mEventProcessor;
    InputReader *mInputDevice;
    Camera *mCamera;

    Vector3 mTranslateVector;
    RenderWindow *mWindow;
    bool mStatsOn;
    bool mUseBufferedInputKeys,
         mUseBufferedInputMouse,
         mInputTypeSwitchingOn;
    unsigned int mNumScreenShots;
    float mMoveScale;
    float mRotScale;

    // just to stop toggles flipping too fast
    Real mTimeUntilNextToggle;
    float mRotX,
          mRotY;
    TextureFilterOptions mFiltering;
    int mAniso;

};

#endif
