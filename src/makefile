###########################################
#Find out current OS
ifdef SHELL
PLATFORM := LINUX
endif
ifdef windir
PLATFORM := WIN32
endif
###########################################





###########################################
#Compiler options
ifeq ("$(PLATFORM)", "WIN32")
TYPE := MINGW
EXE_EXT := .exe
BASE_INCLUDE_DIR := $(PWD)../3pl/windows/include
BASE_LIBRARY_DIR := $(PWD)../3pl/windows/lib
PLATFORM_CFLAGS := -D__WIN32__ -DOGRE_NONCLIENT_BUILD -D_GLIBCPP_USE_C99_DYNAMIC -DOGRE_GL_USE_MULTITEXTURING -DYY_NO_UNPUT -D__ogreMinGW -Dmain=SDL_main
LD_FLAGS:= -mno-mingw -mwindows -Wl --enable-runtime-pseudo-reloc
endif
ifeq ("$(PLATFORM)", "LINUX")
TYPE :=
EXE_EXT := 
BASE_INCLUDE_DIR := /usr/include
BASE_LIBRARY_DIR := /usr/lib
PLATFORM_CFLAGS :=
LD_FLAGS:=
endif
DEFINES := $(PLATFORM) $(TYPE)
TARGET := motorsport$(EXE_EXT)
LD := g++
CC := gcc
CXX := g++
CFLAGS := -c -Wall -g -O0 -DGCC_3_1 -DEXT_HASH $(PLATFORM_CFLAGS)
CXXFLAGS := $(CFLAGS)
###########################################





###########################################
#Unit Test Programs (see also: RUN_TESTS)
TEST_EXE := TestLogEngine
###########################################





###########################################
#Paths to libs and header files
LD_LIBRARY_PATH := $(BASE_LIBRARY_DIR)
INCDIRS := $(BASE_LIBRARY_DIR)
###########################################





###########################################
#Paths to include dirs
#Windows specific paths...
ifeq ("$(PLATFORM)", "WIN32")
PLATFORM_INCLUDE_DIRS := \
	$(BASE_INCLUDE_DIR)/OgreMain \
	$(BASE_INCLUDE_DIR)/PlatformManager_Win32 \
	$(BASE_INCLUDE_DIR)/ReferenceApplication/Common \
	$(BASE_INCLUDE_DIR)/ReferenceApplication/ReferenceAppLayer \
	$(BASE_INCLUDE_DIR)/PlugIns/BSPSceneManager \
	$(BASE_INCLUDE_DIR)/PlugIns/CgProgramManager \
	$(BASE_INCLUDE_DIR)/PlugIns/FileSystem \
	$(BASE_INCLUDE_DIR)/PlugIns/GuiFilesystem \
	$(BASE_INCLUDE_DIR)/PlugIns/NatureSceneManager \
	$(BASE_INCLUDE_DIR)/PlugIns/OctreeSceneManager \
	$(BASE_INCLUDE_DIR)/PlugIns/ParticleFX \
	$(BASE_INCLUDE_DIR)/deps/DevIL/include \
	$(BASE_INCLUDE_DIR)/deps/libode \
	$(BASE_INCLUDE_DIR)/deps/libopcode \
	$(BASE_INCLUDE_DIR)/deps/libopcode/Ice \
	$(BASE_INCLUDE_DIR)/deps/libz \
	$(BASE_INCLUDE_DIR)/deps/NVIDIA_Cg/include
endif
#Linux specific paths...
ifeq ("$(PLATFORM)", "LINUX")
PLATFORM_INCLUDE_DIRS := \
	/usr/local/include/OGRE \
	/usr/local/include/SDL
endif

#All paths...	
INCLUDE_DIRS := \
	$(PLATFORM_INCLUDE_DIRS) \
	$(BASE_INCLUDE_DIR)/ \
	$(BASE_INCLUDE_DIR)/ode \
	$(BASE_INCLUDE_DIR)/freetype2 \
	$(BASE_INCLUDE_DIR)/SDL \
	$(BASE_INCLUDE_DIR)/OGRE \
	$(BASE_INCLUDE_DIR)/xercesc \
	input \
	graphics \
	physics \
	gui \
	data \
	log \
	./
###########################################





###########################################
#Paths to library dirs
#Windows specific paths...
ifeq ("$(PLATFORM)", "WIN32")
PLATFORM_LIBRARY_DIRS := \
	/dev-cpp/lib
endif 
#Linux specific paths...
ifeq ("$(PLATFORM)", "LINUX")
PLATFORM_LIBRARY_DIRS := \
	/usr/local/lib \
	$(BASE_LIBRARY_DIR)/OGRE \
	$(BASE_LIBRARY_DIR)/ode
endif
#All paths...	
LIBRARY_DIRS := \
	$(PLATFORM_LIBRARY_DIRS) \
	$(BASE_LIBRARY_DIR)
###########################################





###########################################
#Libraries
#Windows specific libraries...
ifeq ("$(PLATFORM)", "WIN32")
PLATFORM_LIBRARIES := \
	xerces-c_2_5_0 \
	pthreadGC \
	ReferenceAppLayer \
	mingw32
endif 
#Linux specific libraries...
ifeq ("$(PLATFORM)", "LINUX")
PLATFORM_LIBRARIES := \
	xerces-c \
	pthread
endif
#All libraries...	
LIBRARIES := \
	$(PLATFORM_LIBRARIES) \
	SDLmain \
	SDL \
	ode \
	freetype \
	OgreMain
###########################################





###########################################
#Sources
SOURCES := \
	main.cpp \
	system.cpp \
	world.cpp \
	log/logEngine.cpp \
	data/dataEngine.cpp \
	physics/physicsEngine.cpp \
	graphics/graphicsEngine.cpp \
	input/inputEngine.cpp \
	gui/guiEngine.cpp \
	data/domParser.cpp \
	data/mainParser.cpp \
	data/logParser.cpp \
	data/dataParser.cpp \
	data/cubeDataParser.cpp \
	data/graphicsParser.cpp \
	data/physicsParser.cpp \
	data/inputParser.cpp \
	data/guiParser.cpp \
	data/cubeData.cpp \
	data/cameraData.cpp \
	physics/cubePhysics.cpp \
	graphics/cubeGraphics.cpp \
	graphics/cameraGraphics.cpp \
	input/cubeInput.cpp \
	input/cameraInput.cpp
###########################################





###########################################
#Targets
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))

#Compiling instructions
%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(patsubst %,-D%,$(DEFINES)) $(patsubst %,-I%,$(INCLUDE_DIRS)) -o $@ $<

%.o : %.c
	$(CC) $(CFLAGS) $(patsubst %,-D%,$(DEFINES)) $(patsubst %,-I%,$(INCLUDE_DIRS)) -o $@ $<


#Phony targets definitions
.PHONY: all
all: 	$(TARGET)

.PHONY: configure
configure:
	@echo    "---Configure...-------------------------------"
	@ echo "there is no configure command at this point - just type 'make'"
	@echo    "----------------------------------------------"

.PHONY: clean
clean:
	@echo    "---Clean...-----------------------------------"
	rm -f $(TARGET) $(OBJECTS) $(TEST_EXE)
	@echo    "----------------------------------------------"

.PHONY: clobber
clobber : clean
	@echo    "---Clobber...---------------------------------"
	rm -f *~ */*~ .*[hc]pp.swp */.*[hc]pp.swp *.log
	@echo    "----------------------------------------------"

.PHONY: run
run:
	@echo    "---Run...-------------------------------------"
	./motorsport 2>/dev/null
	@echo    "----------------------------------------------"

.PHONY: tags
tags:
	ctags --c-types=+px **/*.[ch]pp

$(TARGET) :$(OBJECTS)
	$(LD) $(LD_FLAGS) $(patsubst %,-L%,$(LIBRARY_DIRS)) -o $@ $^ $(patsubst %,-l%,$(LIBRARIES))
	@chmod +x $@
	@echo    "---Exec...------------------------------------"
	@echo -n "Motorsport successfully compiled!:     "
	@du --si $(TARGET) |cut -c -4
	@echo    "If you have problems running $(TARGET),"
	@echo    "read the doc/developer/README file."
	@echo    "----------------------------------------------"

#compiling the unit test programs
TestLogEngine: -lxerces-c -lpthread log/testLogEngine.o log/logEngine.o log/logParser.o data/domParser.o
	$(LD)  -o $@ $^
	@chmod +x $@

.PHONY: test
test:	$(TEST_EXE)
	$(foreach test, $(TEST_EXE), ./$(test);)
###########################################
