########################################################################
# This Makefile can be used instead of runme to produce images
# of scenes.  It has the advantage that it only creates the images
# that are missing or out of date.  Thus, it should be a bit more
# convenient to run incrementally.
########################################################################


########################################################################
# Default commands
########################################################################

DISTRIBUTED_SIZE=-width 256 -height 128

EXE=src/raypro

output/%.jpg: input/%.scn 
	$(EXE) $< $@

output/%.0.jpg: input/%.scn 
	$(EXE) $< $@ -max_depth 0

output/%.1.jpg: input/%.scn 
	$(EXE) $< $@ -max_depth 1

output/%.2.jpg: input/%.scn 
	$(EXE) $< $@ -max_depth 2


########################################################################
# List of target categories
########################################################################

all: \
  $(EXE) \
  ray-primitive \
  ray-scene \
  illumination \
  transformation \
  specular \
  shadow \
  specular \
  transmission \
  refraction \
  antialias \
  distributed 



########################################################################
# Compile src/raypro
########################################################################

$(EXE): src
	cd src; make


########################################################################
# List of targets within each category
########################################################################

ray-primitive: \
  output/sphere.jpg  \
  output/tri.jpg  \
  output/box1.jpg  \
  output/box2.jpg  \
  output/ico.jpg \
  output/teapot.jpg \
  output/cylinder1.jpg  \
  output/cylinder2.jpg  \
  output/cone1.jpg \
  output/cone2.jpg 

ray-scene: \
  output/fourspheres.jpg \
  output/sixtriangles.jpg \
  output/mixedprimitives1.jpg \
  output/mixedprimitives2.jpg 

illumination: \
  output/diffuse.jpg \
  output/specular.jpg \
  output/shininess.jpg \
  output/dirlight1.jpg \
  output/pointlight1.jpg \
  output/spotlight1.jpg \
  output/dirlight2.jpg \
  output/pointlight2.jpg \
  output/spotlight2.jpg \
  output/texture.jpg

shadow: \
  output/hardshadow.jpg \
  output/softshadow.jpg

specular: \
  output/specular.0.jpg \
  output/specular.1.jpg \
  output/specular.2.jpg \
  output/shininess.0.jpg \
  output/shininess.1.jpg \
  output/shininess.2.jpg \
  output/stilllife.0.jpg \
  output/stilllife.1.jpg \
  output/stilllife.2.jpg

transmission: \
  output/transmission.0.jpg \
  output/transmission.1.jpg \
  output/transmission.2.jpg

refraction: \
  output/refraction.0.jpg \
  output/refraction.1.jpg \
  output/refraction.2.jpg

transformation: \
  output/mixedprimitives3.jpg \
  output/transform.jpg \
  output/stack.jpg 

antialias: \
  output/antialias1.jpg \
  output/antialias4.jpg \
  output/antialias16.jpg \
  output/antialias256.jpg

distributed: \
  output/distributed0_0.jpg \
  output/distributed1_0.jpg \
  output/distributed2_0.jpg \
  output/distributed0_1.jpg \
  output/distributed0_4.jpg \
  output/distributed1_1.jpg \
  output/distributed0_16.jpg \
  output/distributed1_4.jpg \
  output/distributed2_1.jpg \
  output/distributed0_256.jpg \
  output/distributed0_64.jpg \
  output/distributed1_16.jpg \
  output/distributed2_4.jpg \
  output/distributed1_64.jpg \
  output/distributed2_16.jpg \
  output/distributed1_256.jpg \
  output/distributed2_64.jpg \
  output/distributed2_256.jpg 


########################################################################
# Targets with specific program arguments
########################################################################

output/antialias1.jpg: input/sphere.scn
	$(EXE) $< $@ -antialias 1 -width 64 -height 64

output/antialias4.jpg: input/sphere.scn
	$(EXE) $< $@ -antialias 4 -width 64 -height 64

output/antialias16.jpg: input/sphere.scn
	$(EXE) $< $@ -antialias 16 -width 64 -height 64

output/antialias256.jpg: input/sphere.scn
	$(EXE) $< $@ -antialias 256 -width 64 -height 64

output/distributed0_%.jpg: input/distributed.scn 
	touch $@.begun
	$(EXE) $< $@ -distribute $* -max_depth 0 $(DISTRIBUTED_SIZE)
	touch $@.finished
	echo $@ done.
output/distributed1_%.jpg: input/distributed.scn 
	touch $@.begun
	$(EXE) $< $@ -distribute $* -max_depth 1 $(DISTRIBUTED_SIZE)
	touch $@.finished
	echo $@ done.
output/distributed2_%.jpg: input/distributed.scn 
	touch $@.begun
	$(EXE) $< $@ -distribute $* -max_depth 2 $(DISTRIBUTED_SIZE)
	touch $@.finished
	echo $@ done.
output/distributed3_%.jpg: input/distributed.scn 
	touch $@.begun
	$(EXE) $< $@ -distribute $* -max_depth 3 $(DISTRIBUTED_SIZE)
	touch $@.finished
	echo $@ done.
