/*
 *  GLSL_helper.h
 *  CSC473
 *
 *  Many useful helper functions for GLSL shaders - gleaned from various sources including orange book
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

#ifdef _WIN32
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <string>
#include <iostream>


int printOglError(char const * file, int line);
#define printOpenGLError() printOglError(__FILE__, __LINE__)

void checkGlErrors();
void getGLversion();

void printProgramInfoLog(GLuint program);
void printShaderInfoLog(GLuint shader);

char * textFileRead(std::string const & fileName);


inline GLint getUniformLoc(GLuint const program, char const * const name)
{
    GLint loc;
    
    loc = glGetUniformLocation(program, name);
    
    if (loc ==1) {
        printf("No such uniform named %s\n", name);
    }
    
    printOpenGLError();
    return loc;
}


inline GLint safe_glGetAttribLocation(const GLuint program, const char varname[])
{
    GLint r = glGetAttribLocation(program, varname);
    if (r < 0)
        std::cerr << "WARN: "<< varname << " cannot be bound (it either doesn't exist or has been optimized away). safe_glAttrib calls will silently ignore it.\n" << std::endl;
    return r;
}

inline GLint safe_glGetUniformLocation(const GLuint program, const char varname[])
{
    GLint r = glGetUniformLocation(program, varname);
    if (r < 0)
        std::cerr << "WARN: "<< varname << " cannot be bound (it either doesn't exist or has been optimized away). safe_glUniform calls will silently ignore it.\n" << std::endl;
    return r;
}


inline void safe_glEnableVertexAttribArray(const GLint handle)
{
    if (handle >= 0)
        glEnableVertexAttribArray(handle);
}

inline void safe_glDisableVertexAttribArray(const GLint handle)
{
    if (handle >= 0)
        glDisableVertexAttribArray(handle);
}

inline void safe_glVertexAttribPointer(const GLint handle, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer)
{
    if (handle >= 0)
        glVertexAttribPointer(handle, size, type, normalized, stride, pointer);
}

inline void safe_glUniformMatrix4fv(const GLint handle, const GLfloat data[])
{
    if (handle >= 0)
        glUniformMatrix4fv(handle, 1, GL_FALSE, data);
}

inline void safe_glUniform1i(const GLint handle, const GLint a)
{
    if (handle >= 0)
        glUniform1i(handle, a);
}

inline void safe_glUniform2i(const GLint handle, const GLint a, const GLint b)
{
    if (handle >= 0)
        glUniform2i(handle, a, b);
}

inline void safe_glUniform3i(const GLint handle, const GLint a, const GLint b, const GLint c)
{
    if (handle >= 0)
        glUniform3i(handle, a, b, c);
}

inline void safe_glUniform4i(const GLint handle, const GLint a, const GLint b, const GLint c, const GLint d)
{
    if (handle >= 0)
        glUniform4i(handle, a, b, c, d);
}

inline void safe_glUniform1f(const GLint handle, const GLfloat a) {
    if (handle >= 0)
        glUniform1f(handle, a);
}
