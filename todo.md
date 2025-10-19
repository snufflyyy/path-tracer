# todo list

 - use a faster library for writing png files or use a fast zlib compress function (see std_image_write.h lines 17-21 for more info)
 - add more hittables (such as planes, or triangles)
 - if i add a triangle hittable, try to add meshes using BVH or something like that
 - direct light sampling possibly
 - maybe rewrite ray_cast in camera.c to NOT use recursion and instead use a for loop
 - add unit testing for essential functions to prevent important shit from stop working (prolly just use the google library for unit testing)

# version 0.5

 - add scene system (saving and loading, maybe json?)
 - bug hunt, clean up, and refactor all code that is shitty
 - add error checking to all pthread function calls
