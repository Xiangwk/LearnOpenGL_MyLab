# LearnOpenGL_MyLab

This is my OpenGL learn notebook, in this repository I will share the source code of JoeyDeVries's LearnOpenGL course!

To make these code work we need some 3rdParty library like glfw, glad etc.

My opinion is compiling them yourself to make sure everything work well!

To get these library's source code you can visit:

**glfw:** <http://www.glfw.org/> This is a library to help you create an opengl context and window which to draw something in it!

**glad:** <http://glad.dav1d.de/> This is a library to help you get the address of opengl's functions to use them.

**stb:** <https://github.com/nothings/stb> This is a very popular single header image loading library by Sean Barrett that is able to load most popular file formats and is easy to integrate in your project(s).

To load an image we just need stb_image.h. Add it to your project as stb_image.h and create an additional C++ file with the following code:

```cpp
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
```

If you still don't understand how to use this header file, please reference to my section III: Texture

For more information about the 3rdParty library, visit https://learnopengl.com/#!Getting-started/Creating-a-window

***

# I. Create a GLFW window

In this section we create a window! It is our canvas for painting!

1. Create GLFW window and OpenGL context;
2. Init GLAD to get address of the OpenGL functions;
3. Create a viewport;
4. Game Loop;

# II. Hellow Triangle

In this section we draw a triangle and a quad on the canvas we create in the last section.

It use some significant concept: VAO, VBO and EBO.

1. Create VAO to store the triangle's vertex attribute;
2. Create VBO to store the vertex data;
3. (Optional)Create EBO to store the indices of triangle;
4. Set vertex attribute;
5. Create and use shader;
6. Draw;

# III. Texture

In this section we attach two textures to our quad.

To create a texture object we need to:

1. Generate texture id;
2. Bind id to a texture target;
3. Set wrapping/filtering option;
4. load image;
5. Generate texture using the image data;

<img src="http://ovi8mw7d8.bkt.clouddn.com/simpleTexture.PNG" width="400" />

# IV. Camera

In this section I wrote two camera class, one is FreeCamera and another is FPSCamera.
The single header file is in the include/ folder. Example in the src/ folder.
The image's used to generate textures is now move to the LearnOpenGL_MyNotebook/image/ folder.

Operation:

W ------- camera move forward

S ------- camera move backward

A ------- camera strafe left

D ------- camera strafe right

mouse move   ------ rotate the len

mouse scroll ------ len zoom

# V. Lighting

In this section I create a base scene with lighting, and in the source code you can see how troublesome to set a light.

Our scene just have 1 direcional light, 4 point lights and a spot light, 10 same boxes just in different position.

To set the uniform value there are nearly 50 lines of code, if more objects in our scene it must be a horrible work.

In next section I will optimize the implementation.

<img src="http://ovi8mw7d8.bkt.clouddn.com/lighting.png" width="400" />

# VI. Uniform Buffer Object

In the Lighting section I said I will optimize the lighting implementation. So you can see this section.

With UBO(Uniform Buffer Object), the same uniform value in different shaders only need to set once.

The Lights in the scene is a good example to use UBO.

I also create a UBO for store the view matrix and projection matrix!

# VII. Load Model and build a LabScene

In this section I add a model loader and abstract texture to a class. This work could make my life a little comfortable by getting rid of creating and binding VAO, VBO and so on.

I will continue to optimize the implementation. Welcome to have try.

<img src="http://ovi8mw7d8.bkt.clouddn.com/LabScene.png" width="400" />

# VIII. Blending

In this section I add some grass on the ground. Actually draw some grass not use blending, it just discard the fragments which are no need to draw.

I will implement the Order Independent Transparency(OIT) in the next or next next or ... section, it is really a complicated algorithm to make blending.

But the process of implementation is worthy! We have no need to sort the transparent object in our scene and will learn more advance features of OpenGL.

Note that the grass's texture must set wrap as clamp_to_edge, not repeat. The effect show in two pictures.

<img src="http://ovi8mw7d8.bkt.clouddn.com/repeat.png" width="400" /><img src="http://ovi8mw7d8.bkt.clouddn.com/clamp_to_edge.png" width="400" />

# IX. Spaceport

In this section I replace the floor to a tranparent plane and add a skybox!

I also update the nanosuit, add some reflect maps. The new resource can be downloaded here:<https://learnopengl-cn.github.io/data/nanosuit_reflection.zip>

The skybox can be found in this web:<http://www.custommapmakers.org/skyboxes.php>, note that you may need to convert the format to .jpg!

Note that many files are updated, like the shader of nanosuit, the Model.h and Mesh.h and so on. The header files now all store in the folder include, you can find it in the root directory.

This two pictures show the difference:

<img src="http://ovi8mw7d8.bkt.clouddn.com/show_noreflect.png" width="400" /><img src="http://ovi8mw7d8.bkt.clouddn.com/show_reflect.png" width="400" />

This picture show the new scene:

<img src="http://ovi8mw7d8.bkt.clouddn.com/space_scene.png" width="400" />
