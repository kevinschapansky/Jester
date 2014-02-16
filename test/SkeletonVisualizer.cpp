#include "SceneGraphNode.h"
#include "Controller.h"
#include "PrimeSenseCarmineFactory.h"
#include "DataFusionModule.h"
#include "Bone.h"
#include "GLSL_helper.h"

#include <stdio.h>
#include <glm/vec3.hpp>

class SkeletonVisualizer {
public:
	SkeletonVisualizer() {
		kController = new jester::Controller(jester::DataFusionModule::FusionAlgorithm::PASS_THROUGH);

		kController->init();
		kScene = kController->getScene();
		kCarmine = jester::PrimeSenseCarmineFactory::CreateCarmineSensor(kScene, kController);

		if (!kCarmine->start()) {
		 	printf("Sensor start failed\n");
		 	exit(1);
		 }
	}
private:
	jester::Controller *kController;
	jester::Scene *kScene;
	jester::Sensor *kCarmine;
};

// Variable Handles
GLuint aPosition;
GLuint aNormal;

GLuint uColor;
GLuint uModelMatrix;
GLuint uViewMatrix;
GLuint uProjMatrix;
GLuint uLightColor;
GLuint uLightPos;
GLuint uMaterial;

GLuint ShadeProg;

/* Main display function */
void Draw (void) {
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				
	//Start our shader	
 	glUseProgram(ShadeProg);
/*
	//data set up to access the vertices and color
  	safe_glEnableVertexAttribArray(h_aPosition);
 	glBindBuffer(GL_ARRAY_BUFFER, triBuffObj);
	safe_glVertexAttribPointer(h_aPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
  	safe_glEnableVertexAttribArray(h_aColor);
 	glBindBuffer(GL_ARRAY_BUFFER, colBuffObj);
	safe_glVertexAttribPointer(h_aColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//actually draw the data
	glDrawArrays(GL_TRIANGLES, 0, 3);	
								*/					
	//clean up 
	safe_glDisableVertexAttribArray(aPosition);
	safe_glDisableVertexAttribArray(uColor);
	//disable the shader
	glUseProgram(0);	
	glutSwapBuffers();
}

bool InstallShader(std::string const & vShaderName, std::string const & fShaderName)
{
    GLuint VS; // handles to shader object
    GLuint FS; // handles to frag shader object
    GLint vCompiled, fCompiled, linked; // status of shader

    VS = glCreateShader(GL_VERTEX_SHADER);
    FS = glCreateShader(GL_FRAGMENT_SHADER);

    // load the source
    char const * vSource = textFileRead(vShaderName);
    char const * fSource = textFileRead(fShaderName);
    glShaderSource(VS, 1, & vSource, NULL);
    glShaderSource(FS, 1, & fSource, NULL);

    // compile shader and print log
    glCompileShader(VS);
    printOpenGLError();
    glGetShaderiv(VS, GL_COMPILE_STATUS, & vCompiled);
    printShaderInfoLog(VS);

    // compile shader and print log
    glCompileShader(FS);
    printOpenGLError();
    glGetShaderiv(FS, GL_COMPILE_STATUS, & fCompiled);
    printShaderInfoLog(FS);

    if (! vCompiled || ! fCompiled)
    {
        std::cerr << "Error compiling either shader " << vShaderName << " or " << fShaderName << std::endl;
        return false;
    }

    // create a program object and attach the compiled shader
    ShadeProg = glCreateProgram();
    glAttachShader(ShadeProg, VS);
    glAttachShader(ShadeProg, FS);

    glLinkProgram(ShadeProg);

    // check shader status requires helper functions
    printOpenGLError();
    glGetProgramiv(ShadeProg, GL_LINK_STATUS, &linked);
    printProgramInfoLog(ShadeProg);

    glUseProgram(ShadeProg);

    // get handles to attribute data
    aPosition   = safe_glGetAttribLocation(ShadeProg, "aPosition");
    aNormal     = safe_glGetAttribLocation(ShadeProg, "aNormal");
    
    uColor          = safe_glGetUniformLocation(ShadeProg, "uColor");
    uProjMatrix     = safe_glGetUniformLocation(ShadeProg, "uProjMatrix");
    uViewMatrix     = safe_glGetUniformLocation(ShadeProg, "uViewMatrix");
    uModelMatrix    = safe_glGetUniformLocation(ShadeProg, "uModelMatrix");
    uMaterial       = safe_glGetUniformLocation(ShadeProg, "uMaterial");
    uLightColor     = safe_glGetUniformLocation(ShadeProg, "uLightColor");
    uLightPos       = safe_glGetUniformLocation(ShadeProg, "uLightPos");

    std::cout << "Sucessfully installed shader " << ShadeProg << std::endl;
    return true;
}

/* Some OpenGL initialization */
void Initialize ()	{				// Any GL Init Code 
	// Start Of User Initialization
	glClearColor (1.0f, 1.0f, 1.0f, 1.0f);								
	// Black Background
 	glClearDepth (1.0f);		// Depth Buffer Setup
 	glDepthFunc (GL_LEQUAL);	// The Type Of Depth Testing
	glEnable (GL_DEPTH_TEST);	// Enable Depth Testing
	glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

/* Reshape */
void ReshapeGL (int width, int height) {
	glViewport (0, 0, (GLsizei)(width), (GLsizei)(height));				
}

int main(int argc, char **argv) {
	//SkeletonVisualizer *vis = new SkeletonVisualizer();

	glutInit(&argc, argv);
	glutInitWindowPosition(20, 20);
	glutInitWindowSize(600, 600);
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
	glutCreateWindow("Jester Skeleton Visualizer");
	glutReshapeFunc( ReshapeGL );
   	glutDisplayFunc( Draw );

   	Initialize();
   	getGLversion();

   	// Shader Setup
    if (! InstallShader("Phong.vert", "Phong.frag"))
    {
        printf("Error installing shader!\n");
        return 1;
    }
	
	glutMainLoop();
	// while (1) {
	// 	jester::Bone *head = sceneRoot->getBone(jester::Bone::BoneId::HAND_R);
	// 	glm::vec3 headPos = head->getWorldPosition();
	// 	printf("%5f %5f %5f\n",headPos.x, headPos.y, headPos.z);
	// }
}

