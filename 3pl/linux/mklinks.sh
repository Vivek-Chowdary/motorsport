#!/bin/bash
origin=/usr

cd include
d=$origin/include
    ln -s /home/stenyak$d/OGRE
    ln -s $d/freetype2
    ln -s $d/OGRE
    ln -s $d/SDL
    ln -s $d/xercesc
cd ..

cd lib
d=$origin/lib
    ln -s /home/stenyak$d/OGRE
    ln -s /home/stenyak$d
    for i in /home/stenyak$d/*Ogre*; do ln -s $i; done
    ln -s $d/ode
cd ..
