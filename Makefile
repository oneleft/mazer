############################################################################
# mazer2016 - an implementation of the assignment 1 specification for
# CPT323 - Object Oriented Programming Using C++.
#
# Developed by Paul Miller. This solution can be used as a starting point for
# Assignment 2.
############################################################################

#object files created as part of compilation
OBJECTS=pathfind/pathfind.o main.o  generators/generator.o data/maze.o args/arg_processor.o args/action.o \
#header files included in various files.
HEADERS=data/maze.h pathfind/pathfind.h generators/recursivegen.h args/action.h \
	args/arg_processor.h constants/constants.h \
	generators/recursivegen_stack.h
CXXFLAGS=-Wall -pedantic -std=c++14
LFLAGS=-pg
#how do we create the binary for execution
all: $(OBJECTS) TAGS
	g++ $(LFLAGS) $(OBJECTS) -o mazer

#how do we make each individual object file? 
#%.o: %.cpp $(HEADERS)
#	g++ $(CXXFLAGS) -c $< -o $@

TAGS:
	ctags -e -R .

#how do we do cleanup so that we only have the source files?
.PHONY:clean
clean:
	rm -f $(OBJECTS) mazer TAGS

debug:CXXFLAGS+=-DDEBUG -g
debug:clean all

.PHONY:svgclean
svgclean : clean
	rm -f *.svg
