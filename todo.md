# todo list

 - use a faster library for writing png files or use a fast zlib compress function (see std_image_write.h lines 17-21 for more info)
 - add more hittables (such as planes, or triangles)
 - if i add a triangle hittable, try to add meshes using BVH or something like that
 - simd optimized math functions maybe? (do some digging to see if the compiler is doing a good enough job simd optimizing)
 - direct light sampling
 - it might be worth to switch all function error handling to use gotos
 - might be time to ditch this material system and just create a principle bsdf like blender has, it is a pain in the ass with the ui and the scene saving and loading
 - make all local variables that are declared but not created need to equal a base value (aka. pointer = NULL)

# version 0.5

 - add unit testing for essential functions to prevent important shit from breaking again (prolly just use the google library for unit testing)
 - bug hunt, clean up, and refactor all code that is shitty
 - add error checking to all pthread function calls
