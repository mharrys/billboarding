Billboarding
============
Billboarding is a technique that orients a object (usually a texture-mapped
quadrilateral) such that it is always perpendicular to the camera reglardless
of how the camera is transformed. It can for example be used to display
complex geometries at a distance when detail is not required.

There are at least two methods to achieve this effect, this program implements
one "cheap" and one "true" method as described by Lighthouse 3D. However,
the methods have been modified by me to use quaternions instead and without
the fixed OpenGL pipeline.

The cheap method is easy and fast to implement but never modifies the object
orientation state, only during render is the technique put in place. It works
by replacing the rotation section in the model-view matrix with the identity
matrix. Because the matrix is in view space, the identity matrix will keep the
objects front direction towards the camera. If scaling is desired the rotation
must be replaced with the scale values of the sprite.

The true method is a bit more involved since it actually modifies the object
orientation state while also being a bit more expensive relative to the cheap
method. It works by simply reorienting the object in world space to look at the
cameras current position in world space without rolling unless moving right
above it.

Note that the billboarding effect from each method is different, the cheap
method gives the illusion of following both the camera position and its
orientation while the true method gives the illusion to only follow the camera
position.

Having the object facing the camera on all rotation axes is not always desired.
The program features two effects, "spherical" which rotates the sprites about
all axes and "cylindrical" which only rotates about the y-axis.

The cheap method can be easily implemented in the vertex shader as described
by Geeks 3D.

+ Press `F1` to toggle billboarding.
+ Press `F2` to toggle adjusting the model-view matrix for cheap method on the CPU or GPU.
+ Press `F3` to toggle between spherical and cylindrical billboarding.
+ Press `F4` to toggle between cheap and true method.
+ Press `F5` to toggle controls.
+ Press `w`, `a`, `d`, `s` to fly forward, left, right or back.
+ Press `r`, `q` to fly up and down.

License
-------
Licensed under GNU GPL v3.0.

How-to
------
You will need a C++11 compiler, GLM, GLEW and SDL2. Consult SConstruct for
details.

Build and run

    $ scons
    $ cd bin
    $ ./billboarding

Cleanup

    $ scons -c

References
----------
1. Simple Billboarding Vertex Shader (GLSL). [downloaded 2014-08-17]. Available from http://www.geeks3d.com/20140807/billboarding-vertex-shader-glsl/.
2. Billboarding Tutorial. [downloaded 2014-08-17]. Available from http://www.lighthouse3d.com/opengl/billboarding/index.php3?billInt.
