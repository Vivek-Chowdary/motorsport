
############################################################
# license etc ...
############################################################


############################################################
# tools
############################################################

CC := g++
LD := g++


############################################################
# options
############################################################

FILENAME := motorsport
INCLUDE_DIRECTORIES := data log

LD_FLAGS := $(patsubst %, -l%, $(LIBRARIES))
CC_FLAGS := -c $(patsubst %, -I%, $(INCLUDE_DIRECTORIES))

ifeq ("$(OS)", "Windows_NT")
LD_FLAGS += -mno-cygwin
TARGET := $(FILENAME).exe
else
TARGET := $(FILENAME)
endif


############################################################
# sources
############################################################

SOURCES := \
    log/logEngine.cpp \
    data/dataEngine.cpp \
    main.cpp

OBJECTS := $(patsubst %.cpp, %.o, $(SOURCES))


############################################################
# dependencies
############################################################

# to be done later ...


############################################################
# rules
############################################################

.PHONY: all
all : $(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: clobber
clobber : clean
	rm -f log*.txt

$(OBJECTS) : %.o : %.cpp
	$(CC) $(CC_FLAGS) -o $@ $<

$(TARGET) : $(OBJECTS)
	$(LD) $(LD_FLAGS) -o $@ $^ $(LB_LIBS)
	chmod +x $@
	@echo "build completed successfully"
