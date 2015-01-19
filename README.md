Billboarding
============
Billboarding is a technique that orients a object (usually a texture-mapped
quadrilateral) such that it is always perpendicular to the camera regardless
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
+ Press `F2` to toggle between spherical and cylindrical billboarding.
+ Press `F3` to toggle between cheap and true method.
+ Press `w`, `a`, `d`, `s` to fly forward, left, right or back.
+ Press `r`, `q` to fly up and down.

Screenshot
----------
![scrot](https://github.com/mharrys/billboarding/raw/master/scrot.png "Screenshot")

How-to
------
This project depends on [Gust](https://github.com/mharrys/gust), see its
project page for details on dependencies.

Recursive clone required

    $ git clone --recursive <repository>

Build and run

    $ scons
    $ cd bin
    $ ./arcball

Cleanup

    $ scons -c

References
----------
1. Simple Billboarding Vertex Shader (GLSL). [downloaded 2014-08-17]. Available from http://www.geeks3d.com/20140807/billboarding-vertex-shader-glsl/.
2. Billboarding Tutorial. [downloaded 2014-08-17]. Available from http://www.lighthouse3d.com/opengl/billboarding/index.php3?billInt.
