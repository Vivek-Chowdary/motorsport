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

###############################################################################
#
# Platform specific
#
###############################################################################

ifeq ("$(OS)", "Windows_NT")

PLATFORM := WIN32
TYPE := CYGWIN
EXE_EXT := .exe

else

PLATFOM := LINUX
TYPE :=
EXE_EXT :=

endif


###############################################################################
#
# Environment check
#
###############################################################################

ifeq ("$(PLATFORM)", "WIN32")

ifeq ("$(MOTORSPORT_INCLUDE_DIRS)", "")
$(error $(PLATFORM) environment not set - see INSTALL for info)
endif

ifeq ("$(MOTORSPORT_LIBRARY_DIRS)", "")
$(error $(PLATFORM) environment not set - see INSTALL for info)
endif

endif


###############################################################################
#
# Compiler options
#
###############################################################################

CC := gcc
CXX := g++
CFLAGS := -c -Wall -g -O0
CXXFLAGS := $(CFLAGS)

INCLUDE_DIRS_WIN32 := $(MOTORSPORT_INCLUDE_DIRS)
INCLUDE_DIRS_LINUX := /usr/include

INCLUDE_DIRS := \
    input \
    graphics \
    physics \
    data \
    log \
    $(INCLUDE_DIRS_$(PLATFORM))

DEFINES := $(PLATFORM) $(TYPE)


###############################################################################
#
# Linker options
#
###############################################################################

TARGET := motorsport$(EXE_EXT)

LD := g++

LD_FLAGS_WIN32 := -mno-cygwin -mwindows
LD_FLAGS_LINUX :=

LD_FLAGS := $(LD_FLAGS_$(PLATFORM))

LIBRARY_DIRS_WIN32 := $(MOTORSPORT_LIBRARY_DIRS)
LIBRARY_DIRS_LINUX := /usr/lib

LIBRARY_DIRS := $(LIBRARY_DIRS_$(PLATFORM))


###############################################################################
#
# Sources
#
###############################################################################

LIBRARIES_WIN32 := mingw32
LIBRARIES_LINUX :=

LIBRARIES := \
    $(LIBRARIES_$(PLATFORM)) \
    SDLmain \
    SDL \
    ode

SOURCES := \
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
# Dependencies
#
###############################################################################

%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(patsubst %,-D%,$(DEFINES)) $(patsubst %,-I%,$(INCLUDE_DIRS)) -o $@ $<

%.o : %.c
	$(CC) $(CFLAGS) $(patsubst %,-D%,$(DEFINES)) $(patsubst %,-I%,$(INCLUDE_DIRS)) -o $@ $<


###############################################################################
#
# Rules
#
###############################################################################

.PHONY: all
all : $(TARGET)

.PHONY: configure
configure:
	@ echo "there is no configure command at this point - just type 'make'"

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: clobber
clobber : clean
	rm -f log*.txt

.PHONY: tags
tags:
	ctags --c-types=+px **/*.[ch]pp

$(TARGET) : $(OBJECTS)
	$(LD) $(LD_FLAGS) $(patsubst %,-L%,$(LIBRARY_DIRS)) -o $@ $^ $(patsubst %,-l%,$(LIBRARIES))
	chmod +x $@
	@echo "build completed successfully"
