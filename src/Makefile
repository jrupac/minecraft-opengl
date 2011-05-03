#
# Makefile for COS426 assignment #3
#

# 
# List of source files
#

RAYPRO_SRCS=raypro.cpp raytrace.cpp R3Scene.cpp
RAYPRO_OBJS=$(RAYPRO_SRCS:.cpp=.o)

RAYVIEW_SRCS=rayview.cpp raytrace.cpp R3Scene.cpp
RAYVIEW_OBJS=$(RAYVIEW_SRCS:.cpp=.o)

#
# Compile and link options.  You can change the -g to -O to get
# an optimized, rather than debug, build.
#

CXX=g++
CXXFLAGS=-Wall -I. -g -DUSE_JPEG

#
# OpenGL libraries
#
UNAME := $(shell uname)
ifneq (,$(findstring Darwin,$(UNAME)))
	GLLIBS = -framework GLUT -framework OpenGL
else
  ifneq (,$(findstring CYGWIN,$(UNAME)))
	GLLIBS = -lglut32 -lglu32 -lopengl32
  else
	GLLIBS = -lglut -lGLU -lGL
  endif
endif

#
# GNU Make: targets that don't build files
#

.PHONY: all clean distclean

#
# Rules encoding targets and dependencies.  By default, the first of
# these is built, but you can also build any individual target by
# passing it to make - e.g., "make imgpro" or "make clean"
#
# Notice that many of the dependencies are implicit (e.g. a .o depends
# on its corresponding .cpp), as are many of the compilation rules.
#

LIBS=R3/libR3.a R2/libR2.a jpeg/libjpeg.a

all: raypro rayview

R3/libR3.a: 
	    $(MAKE) -C R3

R2/libR2.a: 
	    $(MAKE) -C R2

jpeg/libjpeg.a: 
	    $(MAKE) -C jpeg

raypro: $(RAYPRO_OBJS) $(LIBS)
		rm -f $@ 
	    $(CXX) $(CXXFLAGS) $^ -lm -o $@ $(GLLIBS)

rayview: $(RAYVIEW_OBJS) $(LIBS)
	    rm -f $@ 
	    $(CXX) $(CXXFLAGS) $^ -lm -o $@ $(GLLIBS)
	    
clean:
	    rm -f *.o raypro rayview
		$(MAKE) -C R3 clean
		$(MAKE) -C R2 clean
		$(MAKE) -C jpeg clean


distclean:  clean

