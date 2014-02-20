#define GLM_FORCE_RADIANS

#include "SceneGraphNode.h"
#include "Controller.h"
#include "PrimeSenseCarmineFactory.h"
#include "DataFusionModule.h"
#include "Bone.h"
#include "GLSL_helper.h"
#include "MStackHelp.h"
#include "GeometryCreator.h"

#include <stdio.h>
#include <glm/vec3.hpp>
#include "glm/gtc/type_ptr.hpp"

extern "C" void ReshapeCamera(int width, int height);
extern "C" void DrawSkeleton();

class SkeletonVisualizer {
public:
	typedef struct RenderData {
		GLuint aPosition;
		GLuint aNormal;

		GLuint uColor;
		GLuint uModelMatrix;
		GLuint uViewMatrix;
		GLuint uProjMatrix;
		GLuint uLightColor;
		GLuint uLightPos;
		GLuint uMaterial;

		GLuint shadeProg;

		RenderingHelper modelTrans;
		int windowWidth;
		int windowHeight;
	} RenderData;

	static const int NUM_BONES = 16;

	SkeletonVisualizer(int *argc, char **argv) {
		kController = new jester::Controller(jester::DataFusionModule::FusionAlgorithm::PASS_THROUGH);

		kController->init();
		kScene = kController->getScene();
		kCarmine = jester::PrimeSenseCarmineFactory::CreateCarmineSensor(kScene, kController);

		kRenderData.windowHeight = 600;
		kRenderData.windowWidth = 600;

		glutInit(argc, argv);
		glutInitWindowPosition(20, 20);
		glutInitWindowSize(kRenderData.windowWidth, kRenderData.windowHeight);
		glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
		glutCreateWindow("Jester Skeleton Visualizer");
		glutReshapeFunc(ReshapeCamera);
	   	glutDisplayFunc(DrawSkeleton);

	   	initialize();
	   	getGLversion();

	   	kDegToRad = 0.0174532925;

	   	// Shader Setup
	    if (!installShader("Phong.vert", "Phong.frag"))
	    {
	        printf("Error installing shader!\n");
	        exit(1);
	    }
	}

	void start() {
		if (!kCarmine->start()) {
		 	printf("Sensor start failed\n");
		 	exit(1);
		}

		glutMainLoop();
	}

	void reshapeGL (int width, int height) {
		kRenderData.windowWidth = width;
		kRenderData.windowHeight = height;
		glViewport (0, 0, (GLsizei)(width), (GLsizei)(height));				
	}

	void drawSkeleton() {
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				
		//Start our shader	
	 	glUseProgram(kRenderData.shadeProg);

	 	SetProjectionMatrix();
	    SetView();

	    glUniform3f(kRenderData.uLightPos, 0.f, 10.f, 5.0f );
	    glUniform3f(kRenderData.uLightColor, .7f, .7f, .7f);

	    safe_glDisableVertexAttribArray(kRenderData.aPosition);
		safe_glDisableVertexAttribArray(kRenderData.aNormal);

/*
	    //attach bone mesh data to shader handles
		safe_glEnableVertexAttribArray(kRenderData.aPosition);
        glBindBuffer(GL_ARRAY_BUFFER, kBoneMesh->PositionHandle);
        safe_glVertexAttribPointer(kRenderData.aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
        
        safe_glEnableVertexAttribArray(kRenderData.aNormal);
        glBindBuffer(GL_ARRAY_BUFFER, kBoneMesh->NormalHandle);
        safe_glVertexAttribPointer(kRenderData.aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, kBoneMesh->IndexHandle);
        glUniform3f(kRenderData.uColor, 0.409f, 0.409f, 0.409f);
        glUniform1f(kRenderData.uMaterial, 1.f);

        kRenderData.modelTrans.pushMatrix();
		kRenderData.modelTrans.loadIdentity();
		kRenderData.modelTrans.rotate(3.14/2.0, glm::vec3(0, 1, 0));

		//actually draw the data
		SetModel();
		glDrawElements(GL_TRIANGLES, kBoneMesh->IndexBufferLength, GL_UNSIGNED_SHORT, 0);
		kRenderData.modelTrans.popMatrix();
*/

		safe_glDisableVertexAttribArray(kRenderData.aPosition);
		safe_glDisableVertexAttribArray(kRenderData.aNormal);

		//attach bone mesh data to shader handles
		safe_glEnableVertexAttribArray(kRenderData.aPosition);
        glBindBuffer(GL_ARRAY_BUFFER, kJointMesh->PositionHandle);
        safe_glVertexAttribPointer(kRenderData.aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
        
        safe_glEnableVertexAttribArray(kRenderData.aNormal);
        glBindBuffer(GL_ARRAY_BUFFER, kJointMesh->NormalHandle);
        safe_glVertexAttribPointer(kRenderData.aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, kJointMesh->IndexHandle);
        glUniform1f(kRenderData.uMaterial, 1.f);

		for (int i = 0; i < jester::Bone::BoneId::BONE_COUNT; i++) {
			kRenderData.modelTrans.pushMatrix();
			kRenderData.modelTrans.loadIdentity();

			jester::Bone *curBone = kScene->getBone(jester::Bone::intToBoneId(i));
			kRenderData.modelTrans.translate(curBone->getPosition());

			if (curBone->getConfidence() > 0.5) {
				glUniform3f(kRenderData.uColor, 0.409f, 0.409f, 0.409f);
			} else {
				glUniform3f(kRenderData.uColor, 0.909f, 0.409f, 0.409f);
			}

			SetModel();
			glDrawElements(GL_TRIANGLES, kJointMesh->IndexBufferLength, GL_UNSIGNED_SHORT, 0);
			kRenderData.modelTrans.popMatrix();
		}

		//clean up 
		safe_glDisableVertexAttribArray(kRenderData.aPosition);
		safe_glDisableVertexAttribArray(kRenderData.aNormal);
		//disable the shader
		glUseProgram(0);	
		glutSwapBuffers();
		glutPostRedisplay();
    	printOpenGLError();
	}

private:
	jester::Controller *kController;
	jester::Scene *kScene;
	jester::Sensor *kCarmine;

	Mesh *kBoneMesh;
	Mesh *kJointMesh;
	RenderData kRenderData;
	float kDegToRad;

	/* Some OpenGL initialization */
	void initialize ()	{				// Any GL Init Code 
		// Start Of User Initialization
		glClearColor (1.0f, 1.0f, 1.0f, 1.0f);								
		// Black Background
	 	glClearDepth (1.0f);		// Depth Buffer Setup
	 	glDepthFunc (GL_LEQUAL);	// The Type Of Depth Testing
		glEnable (GL_DEPTH_TEST);	// Enable Depth Testing
		glEnable(GL_CULL_FACE);
	    glCullFace(GL_BACK);
	    kBoneMesh = GeometryCreator::CreateCylinder(0.05, 0.05, 1.0, 15, 1);
	    kJointMesh = GeometryCreator::CreateSphere(glm::vec3(0.075));
		kRenderData.modelTrans.useModelViewMatrix();
		kRenderData.modelTrans.loadIdentity();
	}

	void SetProjectionMatrix() {
	    glm::mat4 Projection = glm::perspective(80.0f, ((float) kRenderData.windowWidth)/ ((float) kRenderData.windowHeight), 0.1f, 100.f);
	    safe_glUniformMatrix4fv(kRenderData.uProjMatrix, glm::value_ptr(Projection));
	}

	void SetView() {
		/*
	    double theta = 0;
	    double phi = 0;
	    vec3 look = vec3(1.0 * cos(phi * kDegToRad) * cos(theta * kDegToRad),
	                1.0 * sin(phi * kDegToRad),
	                1.0 * cos(phi * kDegToRad) * cos((90.0 - theta) * kDegToRad));
	   	vec3 eye(0.0, 0.0, -3.0);
	    glm::mat4 View = glm::lookAt(eye, look + eye, vec3(0.f, 1.f, 0.f));*/
	    glm::mat4 View = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f),glm::vec3(0.f, -1.f, 0.f));
	    safe_glUniformMatrix4fv(kRenderData.uViewMatrix, glm::value_ptr(View));
	}

	void SetModel() {
	    safe_glUniformMatrix4fv(kRenderData.uModelMatrix, glm::value_ptr(kRenderData.modelTrans.modelViewMatrix));
	}

	bool installShader(std::string const & vShaderName, std::string const & fShaderName) {
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
	    kRenderData.shadeProg = glCreateProgram();
	    glAttachShader(kRenderData.shadeProg, VS);
	    glAttachShader(kRenderData.shadeProg, FS);

	    glLinkProgram(kRenderData.shadeProg);

	    // check shader status requires helper functions
	    printOpenGLError();
	    glGetProgramiv(kRenderData.shadeProg, GL_LINK_STATUS, &linked);
	    printProgramInfoLog(kRenderData.shadeProg);

	    glUseProgram(kRenderData.shadeProg);

	    // get handles to attribute data
	    kRenderData.aPosition   = safe_glGetAttribLocation(kRenderData.shadeProg, "aPosition");
	    kRenderData.aNormal     = safe_glGetAttribLocation(kRenderData.shadeProg, "aNormal");
	    
	    kRenderData.uColor          = safe_glGetUniformLocation(kRenderData.shadeProg, "uColor");
	    kRenderData.uProjMatrix     = safe_glGetUniformLocation(kRenderData.shadeProg, "uProjMatrix");
	    kRenderData.uViewMatrix     = safe_glGetUniformLocation(kRenderData.shadeProg, "uViewMatrix");
	    kRenderData.uModelMatrix    = safe_glGetUniformLocation(kRenderData.shadeProg, "uModelMatrix");
	    kRenderData.uMaterial       = safe_glGetUniformLocation(kRenderData.shadeProg, "uMaterial");
	    kRenderData.uLightColor     = safe_glGetUniformLocation(kRenderData.shadeProg, "uLightColor");
	    kRenderData.uLightPos       = safe_glGetUniformLocation(kRenderData.shadeProg, "uLightPos");

	    std::cout << "Sucessfully installed shader " << kRenderData.shadeProg << std::endl;
	    return true;
	}
};

SkeletonVisualizer *globalVis = NULL;

extern "C" void ReshapeCamera(int width, int height) {
	globalVis->reshapeGL(width, height);
}

extern "C" void DrawSkeleton() {
	globalVis->drawSkeleton();
}

int main(int argc, char **argv) {
	globalVis = new SkeletonVisualizer(&argc, argv);

	globalVis->start();
	// while (1) {
	// 	jester::Bone *head = sceneRoot->getBone(jester::Bone::BoneId::HAND_R);
	// 	glm::vec3 headPos = head->getWorldPosition();
	// 	printf("%5f %5f %5f\n",headPos.x, headPos.y, headPos.z);
	// }
}

