###############################################################################
#
# Copyright (C) 2004 Martin Fido (martinfido@users.sourceforge.co.uk)
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
###############################################################################

include common.mk
#optional overrides
-include platform.mk

###############################################################################
#
# Linker options
#
###############################################################################

TARGET := motorsport$(EXE_EXT)

###############################################################################
#
# Sources
#
###############################################################################

LIBRARIES_WIN32 := mingw32
LIBRARIES_LINUX :=

LIBRARIES := $(LIBRARIES) \
    $(LIBRARIES_$(PLATFORM)) \
    SDLmain \
    SDL \
    ode \
    pthread

SOURCES := $(SOURCES) \
    log/logEngine.cpp \
    data/dataEngine.cpp \
    graphics/graphicsEngine.cpp \
    physics/physicsEngine.cpp \
    input/inputEngine.cpp \
    system.cpp \
    world.cpp \
    main.cpp

OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))

###############################################################################
#
# Rules
#
###############################################################################
