#Opciones OS-specific
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
PLATFOM := LINUX
TYPE :=
EXE_EXT := 
BASE_INCLUDE_DIR := ../3pl/linux/include
BASE_LIBRARY_DIR := ../3pl/linux/lib
LD_FLAGS:=
LIBRARIES_LINUX :=
endif


#Opciones del compilador
DEFINES := $(PLATFORM) $(TYPE)
TARGET := motorsport$(EXE_EXT)
LD := g++
CC := gcc
CXX := g++
CFLAGS := -c -Wall -g -O0 -DGCC_3_1 -DEXT_HASH
CXXFLAGS := $(CFLAGS)

#Definicion de rutas a cabeceras y librerias
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
	$(BASE_INCLUDE_DIR)/paragui \
	$(BASE_INCLUDE_DIR)/OGRE \
	$(BASE_INCLUDE_DIR)

LIBRARY_DIRS := \
	$(BASE_LIBRARY_DIR)
#	$(BASE_LIBRARY_DIR)/OGRE

LIBRARIES := \
	SDLmain \
	SDL \
	ode \
	pthread \
	freetype \
	paragui \
	OgreMain

SOURCES := \
	log/logEngine.cpp \
	data/dataEngine.cpp \
	graphics/graphicsEngine.cpp \
	gui/guiEngine.cpp \
	physics/physicsEngine.cpp \
	input/inputEngine.cpp \
	system.cpp \
	world.cpp \
	main.cpp
				
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))



#Ordenes de compilacion
%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(patsubst %,-D%,$(DEFINES)) $(patsubst %,-I%,$(INCLUDE_DIRS)) -o $@ $<

%.o : %.c
	$(CC) $(CFLAGS) $(patsubst %,-D%,$(DEFINES)) $(patsubst %,-I%,$(INCLUDE_DIRS)) -o $@ $<


#Opciones de make
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
	rm -f $(TARGET) $(OBJECTS)
	@echo    "----------------------------------------------"

.PHONY: clobber
clobber : clean
	@echo    "---Clobber...---------------------------------"
	rm -f log*.txt
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
	@ls -Falh $(TARGET) |cut -c 39-42
	@echo    "----------------------------------------------"

