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
EXE_EXT := .exe
else
PLATFOM := LINUX
EXE_EXT :=
endif


###############################################################################
#
# Compiler options
#
###############################################################################

CC_PLUS_PLUS := g++
CC := gcc
CC_FLAGS := -c
INCLUDE_DIRS := data log
DEFINES :=


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


###############################################################################
#
# Sources
#
###############################################################################

SOURCES := \
    log/logEngine.cpp \
    data/dataEngine.cpp \
    main.cpp

OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))


###############################################################################
#
# Dependencies
#
###############################################################################

%.o : %.cpp
	$(CC_PLUS_PLUS) $(CC_FLAGS) $(patsubst %,-D%,$(DEFINES)) $(patsubst %,-I%,$(INCLUDE_DIRS)) -o $@ $<

%.o : %.c
	$(CC) $(CC_FLAGS) $(patsubst %,-D%,$(DEFINES)) $(patsubst %,-I%,$(INCLUDE_DIRS)) -o $@ $<


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

$(TARGET) : $(OBJECTS)
	$(LD) $(LD_FLAGS) $(patsubst %,-L%,$(LIB_DIRS)) -o $@ $^ $(patsubst %,-l%,$(LIBS))
	chmod +x $@
	@echo "build completed successfully"
