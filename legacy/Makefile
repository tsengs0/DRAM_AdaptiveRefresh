# Name of the generated binary.
BINARY := RefreshCounter

SRC_DIR=src
INC_DIR=inc

# Identifies the source files and derives name of object files.
SOURCES := $(SRC_DIR)/main.cpp $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(INC_DIR)/*.h)
OBJECTS := ${SOURCES:.cpp=.o}
DEPENDENCIES := ${SOURCES:.cpp=.d}

##########################################
# Compiler settings
##########################################

# State what compiler we use.
CXX := g++-5.4

# Optimization flags. Usually you should not optimize until you have finished
# debugging, except when you want to detect dead code.
OPTCXXFLAGS =

# Debugging flags.
DBGCXXFLAGS = #-g

# Common warning flags shared by both C and C++.
WARNFLAGS := -std=c++11 -W -Wall 

# Sum up the flags.
CXXFLAGS := -O #${WARNFLAGS} ${DBGCXXFLAGS} ${OPTCXXFLAGS}

# Linker flags.
LDFLAGS := -Wall

##########################################
# Targets
##########################################

$(BINARY): ${OBJECTS}
	$(CXX) $(LDFLAGS) -o $@ $^ $(XERCES_LDFLAGS)

# From .cpp to .o. Dependency files are generated here
${OBJECTS}: %.o: %.cc
	$(CXX) ${CXXFLAGS} -MMD -MF $(subst .o,.d,$@) -o $@ -c $<


all: ${BINARY}

run:
	./RefreshCounter access_pattern.csv

clean:
	$(RM) $(OBJECTS) $(DEPENDENCIES) $(BINARY)

.PHONY: clean

-include $(DEPENDENCIES)
