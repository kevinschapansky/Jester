/*
 *  GLSL_helper.h
 *  CSC473
 *
 *  Many useful helper functions for GLSL shaders - gleaned from various sources including orange book
 *  The 'safe' versions of the shader calls are from Harvard's CSC 175
 *  Created by zwood on 2/21/10.
 *
 */

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OPENGL/gl.h>
#endif

#ifdef __unix__
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdexcept>

int printOglError (const char *file, int line);

/* Print out the information log for a program object */
void printProgramInfoLog (GLuint program);

void printShaderInfoLog (GLuint shader);

#define printOpenGLError() printOglError(__FILE__, __LINE__)

//A helper routine to make it easier to set uniform variables in the shader
GLint getUniLoc(GLuint program, const GLchar *name);

void getGLversion();

int textFileWrite(char *fn, char *s);
char *textFileRead(char *fn);

inline GLint safe_glGetAttribLocation(const GLuint program, const char varname[]) {
  GLint r = glGetAttribLocation(program, varname);
  if (r < 0)
    std::cerr << "WARN: "<< varname << " cannot be bound (it either doesn't exist or has been optimized away). safe_glAttrib calls will silently ignore it.\n" << std::endl;
  return r;
}

inline void safe_glEnableVertexAttribArray(const GLint handle) {
  if (handle >= 0)
    glEnableVertexAttribArray(handle);
}

inline void safe_glDisableVertexAttribArray(const GLint handle) {
  if (handle >= 0)
    glDisableVertexAttribArray(handle);
}

inline void safe_glVertexAttribPointer(const GLint handle, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer) {
  if (handle >= 0)
    glVertexAttribPointer(handle, size, type, normalized, stride, pointer);
}
