#OS-specific options
ifeq ("$(OS)", "Windows_NT")

PLATFORM := WIN32
TYPE := CYGWIN
EXE_EXT := .exe
BASE_INCLUDE_DIR := $(PWD)../3pl/windows/include
BASE_LIBRARY_DIR := $(PWD)../3pl/windows/lib
LD_FLAGS:= -mno-cygwin -mwindows
LIBRARIES_WIN32 := mingw32
else
#endif
#ifeq ("$(OSTYPE)", "linux-gnu")
PLATFORM := LINUX
TYPE :=
EXE_EXT := 
BASE_INCLUDE_DIR := /usr/include
BASE_LIBRARY_DIR := /usr/lib
LD_FLAGS:=
LIBRARIES_LINUX :=
endif


#Compiler options
DEFINES := $(PLATFORM) $(TYPE)
TARGET := motorsport$(EXE_EXT)
LD := g++
CC := gcc
CXX := g++
CFLAGS := -c -Wall -g -O0 -DGCC_3_1 -DEXT_HASH
CXXFLAGS := $(CFLAGS)

#Unit Test Programs (see also: RUN_TESTS)
TEST_EXE := TestLogEngine

#Paths to libs and header files
LD_LIBRARY_PATH := $(BASE_LIB_DIR)
INCDIRS := $(BASE_LIBRARY_DIR)

INCLUDE_DIRS := \
	./ \
	input \
	graphics \
	physics \
	gui \
	data \
	log \
	$(BASE_INCLUDE_DIR)/ode \
	$(BASE_INCLUDE_DIR)/freetype2 \
	$(BASE_INCLUDE_DIR)/SDL \
	$(BASE_INCLUDE_DIR)/OGRE \
	$(BASE_INCLUDE_DIR)/xercesc \
	$(BASE_INCLUDE_DIR)/../local/include/OGRE \
	$(BASE_INCLUDE_DIR)/../local/include/SDL
	
LIBRARY_DIRS := \
	$(BASE_LIBRARY_DIR) \
	$(BASE_LIBRARY_DIR)/../local/lib \
#	$(BASE_LIBRARY_DIR)/OGRE \
#       $(BASE_LIBRARY_DIR)/ode

LIBRARIES := \
	SDLmain \
	SDL \
	ode \
	pthread \
	freetype \
	OgreMain \
	xerces-c

SOURCES := \
	log/logEngine.cpp \
        log/logParser.cpp \
        data/cubeData.cpp \
        data/cameraData.cpp \
        data/domParser.cpp \
	data/dataEngine.cpp \
        graphics/cubeGraphics.cpp \
        graphics/cameraGraphics.cpp \
	graphics/graphicsEngine.cpp \
        graphics/graphicsParser.cpp \
	gui/guiEngine.cpp \
        physics/cubePhysics.cpp \
	physics/physicsEngine.cpp \
        physics/physicsParser.cpp \
        input/cubeInput.cpp \
        input/cameraInput.cpp \
	input/inputEngine.cpp \
	system.cpp \
	world.cpp \
	main.cpp
				
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))

#compiling the unit test programs
TestLogEngine: log/testLogEngine.o log/logEngine.o
	$(LD)  -o $@ $^
	@chmod +x $@


#Compiling instructions
%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(patsubst %,-D%,$(DEFINES)) $(patsubst %,-I%,$(INCLUDE_DIRS)) -o $@ $<

%.o : %.c
	$(CC) $(CFLAGS) $(patsubst %,-D%,$(DEFINES)) $(patsubst %,-I%,$(INCLUDE_DIRS)) -o $@ $<


#Phony targets definitions
.PHONY: all
all: 	$(TARGET)

.PHONY: test
test:	$(TEST_EXE)
	$(foreach test, $(TEST_EXE), ./$(test);)

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
	ldd motorsport
	./motorsport
	@echo    "----------------------------------------------"

.PHONY: tags
tags:
	ctags --c-types=+px **/*.[ch]pp

$(TARGET) :$(OBJECTS)
	$(LD) $(LD_FLAGS) $(patsubst %,-L%,$(LIBRARY_DIRS)) -o $@ $^ $(patsubst %,-l%,$(LIBRARIES))
	@chmod +x $@
	@echo    "---Dirs...------------------------------------"
	@echo    "Base_include_dir: $(BASE_INCLUDE_DIR)"
	@echo    "Base_library_dir: $(BASE_LIBRARY_DIR)"
	@echo    "Include_dirs:     $(INCLUDE_DIRS)"
	@echo    "Libraries_dirs:   $(LIBRARIES_DIRS)"
	@echo    "Libraries:        $(LIBRARIES)"
	@echo    "Sources:          $(SOURCES)"
	@echo    "---Exec...------------------------------------"
	@echo -n "Motorsport successfully compiled!:     "
	@du --si $(TARGET) |cut -c -4
	@echo    "If you have problems running motorsport,"
	@echo    "read the doc/developer/README file."
	@echo    "----------------------------------------------"

