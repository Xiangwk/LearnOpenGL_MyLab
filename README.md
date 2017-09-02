# LearnOpenGL_MyNotebook

This is my OpenGL learn notebook, in this repository I will share the source code of JoeyDeVries's LearnOpenGL course!

To make these code work we need some 3rdParty library like glfw, glad etc.

My opinion is compiling them yourself to make sure everything work well!

To get these library's source code you can visit:

**glfw:** <http://www.glfw.org/> This is a library to help you create an opengl context and window which to draw something in it!

**glad:** <http://glad.dav1d.de/> This is a library to help you get the address of opengl's functions to use them.

For more information you can visit https://learnopengl.com/#!Getting-started/Creating-a-window

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
