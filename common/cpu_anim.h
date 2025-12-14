#ifndef __CPU_ANIM_H__
#define __CPU_ANIM_H__

#include <iostream>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef struct Vertex
{
  float pos[2];
  float uv[2];
} Vertex;

static const Vertex vertices[6] = {
    {{1.f, 1.f}, {1.0f, 0.0f}},   // top right
    {{1.f, -1.f}, {1.0f, 1.f}},   // bottom right
    {{-1.f, -1.f}, {0.0f, 1.0f}}, // bottom left
    {{-1.f, -1.f}, {0.0f, 1.0f}}, // bottom left (duplicate)
    {{-1.f, 1.f}, {0.0f, 0.0f}},  // top left
    {{1.f, 1.f}, {1.0f, 0.0f}}    // top right (duplicate)
};

static const char *vertex_shader_text =
    "#version 330\n"
    "uniform mat4 MVP;\n"
    "in vec2 vUv;\n"
    "in vec2 vPos;\n"
    "out vec2 uv;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
    "    uv = vUv;\n"
    "}\n";

static const char *fragment_shader_text =
    "#version 330\n"
    "in vec2 uv;\n"
    "out vec4 fragment;\n"
    "uniform sampler2D texSampler;\n"
    "void main()\n"
    "{\n"
    "  //fragment = vec4(uv, 0.0, 1.0);\n"
    "  fragment = texture(texSampler, uv);\n"
    "}\n";

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
}

void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, true);
  }
}

struct CPUAnimBitmap {
  std::unique_ptr<unsigned char[]> pixels;
  int width, height;
  void *dataBlock;
  void (*fAnim)(void *, int);
  void (*animExit)(void *);
  void (*clickDrag)(void *, int, int, int, int);
  int dragStartX, dragStartY;

  // OpenGL resources
  GLuint vertex_buffer = 0;
  GLuint vertex_shader = 0;
  GLuint fragment_shader = 0;
  GLuint program = 0;
  GLuint texture = 0;

  CPUAnimBitmap(int w, int h, void *d = NULL) {
    width = w;
    height = h;
    pixels.reset(new unsigned char[width * height * 4]);
    dataBlock = d;
    clickDrag = NULL;
  }

  ~CPUAnimBitmap() {
    glDeleteBuffers(1, &vertex_buffer);
    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteProgram(program);
    glDeleteTextures(1, &texture);
  }

  unsigned char *get_ptr(void) const { return pixels.get(); }
  long image_size(void) const { return width * height * 4; }

  void click_drag(void (*f)(void *, int, int, int, int)) {
    clickDrag = f;
  }

  void anim_and_exit(void (*f)(void *, int), void (*e)(void *)) {
    CPUAnimBitmap **bitmap = get_bitmap_ptr();
    *bitmap = this;
    fAnim = f;
    animExit = e;
    // a bug in the Windows GLUT implementation prevents us from
    // passing zero arguments to glutInit()
    int c = 1;

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
    GLFWwindow *window = glfwCreateWindow(width, height, "bitmap", NULL, NULL);
    if (window == NULL) {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Note: You must initialize an OpenGL context (e.g., with GLFW) BEFORE calling glewInit
    GLenum err = glewInit();
    if (GLEW_OK != err) {
      std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
      return;
    }
    // std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    const GLint mvp_location = glGetUniformLocation(program, "MVP");
    const GLint vpos_location = glGetAttribLocation(program, "vPos");
    const GLint vuv_location = glGetAttribLocation(program, "vUv");
    const GLint uniformLocation = glGetUniformLocation(program, "texSampler");

    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void *)offsetof(Vertex, pos));
    glEnableVertexAttribArray(vuv_location);
    glVertexAttribPointer(vuv_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void *)offsetof(Vertex, uv));

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, get_ptr());

    float mvp[16] = {
        1.f,
        0.f,
        0.f,
        0.f,
        0.f,
        1.f,
        0.f,
        0.f,
        0.f,
        0.f,
        1.f,
        0.f,
        0.f,
        0.f,
        0.f,
        1.f,
    };

    static int ticks = 1;

    while (!glfwWindowShouldClose(window)) {
      // input
      processInput(window);

      // compute new frame
      fAnim(dataBlock, ticks++);
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                      GL_RGBA, GL_UNSIGNED_BYTE, get_ptr());

      // render
      glViewport(0, 0, width, height);
      glClear(GL_COLOR_BUFFER_BIT);

      glUseProgram(program);
      glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat *)&mvp);
      glBindVertexArray(vertex_array);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture);
      glUniform1i(uniformLocation, 0);
      glDrawArrays(GL_TRIANGLES, 0, 6);

      // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
      glfwSwapBuffers(window);
      glfwPollEvents();
    }

    animExit(dataBlock);
    glfwTerminate();
  }

  // static method used for glut callbacks
  static CPUAnimBitmap **get_bitmap_ptr(void)
  {
    static CPUAnimBitmap *gBitmap;
    return &gBitmap;
  }

  // static method used for glut callbacks
  static void mouse_func(int button, int state,
                         int mx, int my)
  {
    // if (button == GLUT_LEFT_BUTTON)
    // {
    //   CPUAnimBitmap *bitmap = *(get_bitmap_ptr());
    //   if (state == GLUT_DOWN)
    //   {
    //     bitmap->dragStartX = mx;
    //     bitmap->dragStartY = my;
    //   }
    //   else if (state == GLUT_UP)
    //   {
    //     bitmap->clickDrag(bitmap->dataBlock,
    //                       bitmap->dragStartX,
    //                       bitmap->dragStartY,
    //                       mx, my);
    //   }
    // }
  }

};

#endif // __CPU_ANIM_H__
