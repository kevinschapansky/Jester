/*
 *  GLSL_helper.cpp
 *  CSC473
 *
 *  Many useful helper functions for GLSL shaders - gleaned from various sources including orange book
 *  Created by zwood on 2/21/10.
 *
 */
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include "GLSL_helper.h"

using namespace std;

void checkGlErrors()
{
	const GLenum errCode = glGetError();

	if (errCode != GL_NO_ERROR)
	{
		string error("GL Error: ");
		error += reinterpret_cast<char const*>(gluErrorString(errCode));
		cerr << error << endl;
		throw runtime_error(error);
	}
}


int printOglError (const char *file, int line) {
	/* Returns 1 if an OpenGL error occurred, 0 otherwise. */
	GLenum glErr;
	int    retCode = 0;
	
	glErr = glGetError ();
	while (glErr != GL_NO_ERROR)
    {
		printf ("glError in file %s @ line %d: %s\n", file, line, gluErrorString (glErr));
		retCode = 1;
		glErr = glGetError ();
    }
	return retCode;
}

void printShaderInfoLog(GLuint shader)
{
	GLint     infologLength = 0;
	GLint     charsWritten  = 0;
	GLchar *infoLog;
	
	printOpenGLError ();  // Check for OpenGL errors
	glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &infologLength);
	printOpenGLError ();  // Check for OpenGL errors
	
	if (infologLength > 0)
    {
		infoLog = (GLchar *)malloc (infologLength);
		if (infoLog == NULL)
        {
			puts ("ERROR: Could not allocate InfoLog buffer");
			exit (1);
        }
		glGetShaderInfoLog (shader, infologLength, &charsWritten, infoLog);
		printf ("Shader InfoLog:\n%s", infoLog);
		free (infoLog);
    }
	printOpenGLError();  // Check for OpenGL errors
}

/* Print out the information log for a program object */
void printProgramInfoLog(GLuint program)
{
	GLint     infologLength = 0;
	GLint     charsWritten  = 0;
	GLchar *infoLog;
	
	printOpenGLError ();  // Check for OpenGL errors
	glGetProgramiv (program, GL_INFO_LOG_LENGTH, &infologLength);
	printOpenGLError ();  // Check for OpenGL errors
	
	if (infologLength > 0)
    	{
		infoLog = (GLchar *)malloc (infologLength);
		if (infoLog == NULL)
        	{
			puts ("ERROR: Could not allocate InfoLog buffer");
			exit (1);
        	}
		glGetProgramInfoLog (program, infologLength, &charsWritten, infoLog);
		printf ("Program InfoLog:\n%s", infoLog);
		free (infoLog);
    	}
	printOpenGLError ();  // Check for OpenGL errors
}

void getGLversion()
{
	int major, minor;
	
	major = minor =0;
	const char *verstr = (const char *)glGetString(GL_VERSION);
	
	if ((verstr == NULL) || (sscanf(verstr, "%d.%d", &major, &minor) !=2)) {
		printf("Invalid GL_VERSION format %d %d\n", major, minor);
	}
	if( major <2) {
		printf("This shader example will not work due to opengl version, which is %d %d\n", major, minor);
		exit(0);
	}
}

char * textFileRead(std::string const & fileName)
{
	FILE * fp;
	char * content = NULL;
	
	int count = 0;
	
	fp = fopen(fileName.c_str(),"rt");
	
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		count = ftell(fp);
		rewind(fp);
		
		if (count > 0)
		{
			content = (char *)malloc(sizeof(char) * (count+1));
			count = fread(content,sizeof(char),count,fp);
			content[count] = '\0';
		}
		fclose(fp);
	}

	return content;
}
