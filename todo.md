# todo list

 - use a faster library for writing png files or use a fast zlib compress function (see std_image_write.h lines 17-21 for more info)
 - add more hittables (such as planes, or triangles)
 - if i add a triangle hittable, try to add meshes using BVH or something like that
 - simd optimized math functions maybe? (do some digging to see if the compiler is doing a good enough job simd optimizing)
 - add support for higher bit color exporting

# version 0.5

 - add scene system (saving and loading, maybe json files?)
 - add unit testing for essential functions to prevent important shit from breaking again (prolly just use the google library for unit testing)
 - bug hunt, clean up, and refactor all code that is shitty
 - add error checking to all pthread function calls
 - do more research on direct light sampling and fix the cast_direct() function in camera.c
