// COS 426, Spring 2007, Thomas Funkhouser
// Assignment 3: Ray Tracing



// Include files
#include "R3/R3.h"
#include "R3Scene.h"
#include "raytrace.h"



// Program arguments

static char options[] =
"  -help\n"
"  -width <int:image width>\n"
"  -height <int:image height>\n"
"  -max_depth <int:max recursion depth>\n"
"  -antialias <int:num_primary_rays_per_pixel>\n"
"  -distribute <int:num_distributed_rays_per_intersection>\n";



static void 
ShowUsage(void) 
{
  // Print usage message and exit
  fprintf(stderr,  "Usage: raypro input_scene output_image [  -option [arg ...] ...]\n");
  fprintf(stderr, "%s", options);
  exit(EXIT_FAILURE);
}



static void 
CheckOption(char *option, int argc, int minargc)
{
  // Check if there are enough remaining arguments for option
  if (argc < minargc)  {
    fprintf(stderr, "Too few arguments for %s\n", option);
    ShowUsage();
    exit(-1);
  }
}


static R3Scene *
ReadScene(const char *filename, int width, int height)
{
  // Allocate scene
  R3Scene *scene = new R3Scene();
  if (!scene) {
    fprintf(stderr, "Unable to allocate scene\n");
    return NULL;
  }

  // Read scene
  if (!scene->Read(filename)) {
    fprintf(stderr, "Unable to read scene from %s\n", filename);
    return NULL;
  }

  // Adjust camera vertical field of view to match aspect ratio of image
  scene->camera.yfov = atan(tan(scene->camera.xfov) * (double) height / (double) width); 

  // Return scene
  return scene;
}



int 
main(int argc, char **argv)
{
  // Look for help
  for (int i = 0; i < argc; i++) {
    if (!strcmp(argv[i], "-help")) {
      ShowUsage();
    }
  }

  // Read input and output filenames
  if (argc < 3)  ShowUsage();
  argv++, argc--; // First argument is program name
  char *input_scene_name = *argv; argv++, argc--; 
  char *output_image_name = *argv; argv++, argc--; 

  // Initialize arguments to default values
  int width = 256;
  int height = 256;
  int max_depth = 0;
  int num_distributed_rays_per_intersection = 0;
  int num_primary_rays_per_pixel = 1;

  // Parse arguments 
  while (argc > 0) {
    if (!strcmp(*argv, "-width")) {
      CheckOption(*argv, argc, 2);
      width = atoi(argv[1]);
      argv += 2, argc -= 2;
    }
    else if (!strcmp(*argv, "-height")) {
      CheckOption(*argv, argc, 2);
      height = atoi(argv[1]);
      argv += 2, argc -= 2;
    }
    else if (!strcmp(*argv, "-max_depth")) {
      CheckOption(*argv, argc, 2);
      max_depth = atoi(argv[1]);
      argv += 2, argc -= 2;
    }
    else if (!strcmp(*argv, "-antialias")) {
      CheckOption(*argv, argc, 2);
      num_primary_rays_per_pixel = atoi(argv[1]);
      argv += 2, argc -= 2;
    }
    else if (!strcmp(*argv, "-distribute")) {
      CheckOption(*argv, argc, 2);
      num_distributed_rays_per_intersection = atoi(argv[1]);
      argv += 2, argc -= 2;
    }
    else {
      // Unrecognized program argument
      fprintf(stderr,  "meshpro: invalid option: %s\n", *argv);
      ShowUsage();
    }
  }

  // Read scene
  R3Scene *scene = ReadScene(input_scene_name, width, height);
  if (!scene) {
    fprintf(stderr, "Unable to read scene from %s\n", input_scene_name);
    exit(-1);
  }

  // Render image
  R2Image *image = RenderImage(scene, width, height, max_depth, 
    num_primary_rays_per_pixel, num_distributed_rays_per_intersection);
  if (!image) {
    fprintf(stderr, "Did not render image from scene\n");
    exit(-1);
  }

  // Transfer the image to sRGB color space: for Windows + Linux and Mac OS X
  // 10.6+ (for earlier MAC OS X it will look slightly too bright, but not as
  // much as it would be too dark otherwise. This function also clamps the 
  // image values; however, it does not scale the brightness and also does not
  // perform any more complicated tone mapping
  image->TosRGB();

  // Write output image
  if (!image->Write(output_image_name)) {
    fprintf(stderr, "Did not write image to %s\n", output_image_name);
    exit(-1);
  }

  // Delete everything
  delete scene;
  delete image;

  // Return success
  return EXIT_SUCCESS;
}



