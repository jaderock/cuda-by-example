#ifndef __CPU_BITMAP_H__
#define __CPU_BITMAP_H__

#include <iostream>
#include <memory>

#include "glfw_helper.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

struct CPUBitmap {
  std::unique_ptr<unsigned char> pixels;
  int x = 0, y = 0;
  void *dataBlock;
  void (*bitmapExit)(void*);

  CPUBitmap(int width, int height, void *d = NULL) :
    x(width), y(height), dataBlock(d) {
    pixels.reset(new unsigned char[width * height * 4]);
  }

  ~CPUBitmap() = default;

  unsigned char* get_ptr( void ) const   { return pixels.get(); }
  long image_size( void ) const { return x * y * 4; }

  void display_and_exit( void(*e)(void*) = NULL ) {
      /*
          CPUBitmap**   bitmap = get_bitmap_ptr();
          *bitmap = this;
          bitmapExit = e;
          // a bug in the Windows GLUT implementation prevents us from
          // passing zero arguments to glutInit()
          int c=1;
          char* dummy = "";
          glutInit( &c, &dummy );
          glutInitDisplayMode( GLUT_SINGLE | GLUT_RGBA );
          glutInitWindowSize( x, y );
          glutCreateWindow( "bitmap" );
          glutKeyboardFunc(Key);
          glutDisplayFunc(Draw);
          glutMainLoop();
          */
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow *window = glfwCreateWindow(x, y, "bitmap", NULL, NULL);
    if (window == NULL) {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      // Handle initialization failure
      // e.g., std::cout << "Failed to initialize GLAD" << std::endl;
      return;
    }

    while (!glfwWindowShouldClose(window)) {
      // input
      processInput(window);

      // render
      Draw();

      // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  }

  // static method used for glut callbacks
  static CPUBitmap** get_bitmap_ptr( void ) {
    static CPUBitmap *gBitmap;
    return &gBitmap;
  }

  // static method used for glut callbacks
  static void Key(unsigned char key, int x, int y) {
        /*        switch (key) {
                    case 27:
                        CPUBitmap*   bitmap = *(get_bitmap_ptr());
                        if (bitmap->dataBlock != NULL && bitmap->bitmapExit != NULL)
                            bitmap->bitmapExit( bitmap->dataBlock );
                        exit(0);
                }*/
  }

  // static method used for glut callbacks
  static void Draw( void ) {
    //CPUBitmap*   bitmap = *(get_bitmap_ptr());
    glClearColor(1.0, 0.0, 0.0, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT );
    //glDrawPixels( bitmap->x, bitmap->y, GL_RGBA, GL_UNSIGNED_BYTE, bitmap->pixels.get() );
    glFlush();
  }
};

#endif  // __CPU_BITMAP_H__
