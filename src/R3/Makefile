#
# Linux makefile for assignment libR3.a
#



#
# List of files
#

CCSRCS=R3Distance.cpp R3Matrix.cpp R3Mesh.cpp R3Sphere.cpp R3Cone.cpp R3Cylinder.cpp R3Box.cpp R3Plane.cpp R3Segment.cpp R3Ray.cpp R3Line.cpp R3Vector.cpp R3Point.cpp R3Block.cpp R3Item.cpp R3Character.cpp 
OBJS=$(CCSRCS:.cpp=.o)
INCS=$(HSRCS) $(CCSRCS:.cpp=.h)



#
# Options
#

CC=g++
CPPFLAGS=-Wall -I. -I.. -g
LDFLAGS=-g



#
# Output library name
#

LIB=./libR3.a



#
# Make targets
#

$(LIB): $(OBJS)
	    - rm -f $(LIB)
	    ar ur $(LIB) $(OBJS) $(USER_OBJS)

clean:
	    -  rm -f *~ *.o *.obj *.pdb $(LIB)

