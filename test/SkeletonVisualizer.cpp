#define GLM_FORCE_RADIANS

#include "SceneGraphNode.h"
#include "Controller.h"
#include "PrimeSenseCarmineFactory.h"
#include "LeapMotionFactory.h"
#include "LeapCarmineFuser.h"
#include "Bone.h"
#include "GLSL_helper.h"
#include "GeometryCreator.h"

#include <stdio.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

extern "C" void ReshapeCamera(int width, int height);
extern "C" void DrawSkeleton();
extern "C" void KeyDownUpdate(int param);
extern "C" void KeyDown(unsigned char key, int x, int y);
extern "C" void KeyUp(unsigned char key, int x, int y);
extern "C" void MousePosition(int x, int y);

//#define NO_CARMINE
//#define NO_LEAP

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

		glm::mat4 cameraTrans;
		int windowWidth;
		int windowHeight;
	} RenderData;

	typedef struct KeyData {
		bool w;
		bool a;
		bool s;
		bool d;
		int highlightedBone;

		int mouseOffsetX;
		int mouseOffsetY;
		float x;
		float z;
		float pitch;
		float yaw;
		bool captureMouse;
	} KeyData;

	static const int NUM_BONES = 16;

	SkeletonVisualizer(int *argc, char **argv) {
		kFuser = new jester::LeapCarmineFuser();
		kController = new jester::Controller(kFuser);

		kController->init();
		kScene = kController->getScene();
		kFuser->setSceneRoot(kScene);

		#ifndef NO_CARMINE
			kCarmine = jester::PrimeSenseCarmineFactory::CreateCarmineSensor(kScene, kController);
			glm::quat carmineOrientation = glm::angleAxis(3.14f, glm::vec3(0, 1, 0));
			carmineOrientation = glm::rotate(carmineOrientation, 3.14f/4.f, glm::vec3(1, 0, 0));
			kCarmine->setOrientation(carmineOrientation);
			kCarmine->setPosition(glm::vec3(0, .9144f, .7112f));
			kFuser->setCarmine(kCarmine);
		#endif 

		#ifndef NO_LEAP
			kLeap = jester::LeapMotionFactory::CreateLeapSensor(kScene, kController);
			kLeap->setOrientation(glm::angleAxis(3.14f, glm::vec3(0, 1, 0)));
			kLeap->setPosition(glm::vec3(0, .2286f, 0.9398f));
			kFuser->setLeap(kLeap);
		#endif

		glutInit(argc, argv);
	   	initialize();
	   	getGLversion();

	   	kDegToRad = 0.0174532925;

	   	// Shader Setup
	    if (!installShader("../test/Phong.vert", "../test/Phong.frag"))
	    {
	        printf("Error installing shader!\n");
	        exit(1);
	    }
	}

	void start() {
		#ifndef NO_CARMINE
			if (!kCarmine->start()) {
			 	printf("Carmine sensor start failed\n");
			 	exit(1);
			}
		#endif

		#ifndef NO_LEAP
			if (!kLeap->start()) {
			 	printf("Leap sensor start failed\n");
			 	exit(1);
			}
		#endif

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
	    SetView(kRenderData.cameraTrans);

	    glUniform3f(kRenderData.uLightPos, 0.f, 10.f, 5.0f );
	    glUniform3f(kRenderData.uLightColor, .7f, .7f, .7f);
		
		drawJoints();
		drawBones();
		//disable the shader
		glUseProgram(0);	
		glutSwapBuffers();
		glutPostRedisplay();
    	printOpenGLError();
	}

	void keyDownUpdate(int param) {
		glutTimerFunc(10, KeyDownUpdate, 10);

		kKeyData.yaw += kKeyData.mouseOffsetX / 500.f;
		kKeyData.mouseOffsetX = 0;

		kKeyData.pitch += kKeyData.mouseOffsetY / 500.f;
		kKeyData.mouseOffsetY = 0;

		if (kKeyData.w) {
			kKeyData.z -= 0.1f;
		}
		if (kKeyData.s) {
			kKeyData.z += 0.1f;
		}
		if (kKeyData.a) {
			kKeyData.x -= 0.1f;
		}
		if (kKeyData.d) {
			kKeyData.x += 0.1f;
		}
		
		kRenderData.cameraTrans = glm::rotate(glm::mat4(1), kKeyData.yaw, glm::vec3(0.f, 1.f, 0.f));
		kRenderData.cameraTrans = glm::rotate(kRenderData.cameraTrans, kKeyData.pitch, glm::vec3(1.f, 0.f, 0.f));
		kRenderData.cameraTrans = glm::translate(kRenderData.cameraTrans, glm::vec3(kKeyData.x, 1.f, kKeyData.z));

		if (kKeyData.captureMouse)
			glutWarpPointer(kRenderData.windowWidth / 2, kRenderData.windowHeight / 2);
	}

	void keyDown(unsigned char key) {
		switch (key) {
        case 'w':
            kKeyData.w = true;
            break;
        case 'a':
            kKeyData.a = true;
            break;
        case 's':
            kKeyData.s = true;
            break;
        case 'd':
            kKeyData.d = true;
            break;
        case 'm':
        	kKeyData.captureMouse = !kKeyData.captureMouse;
        	break;
        case 'q': case 'Q' :
            exit( EXIT_SUCCESS );
            break;
    	}
	}

	void keyUp(unsigned char key) {
		switch (key) {
        case 'w':
            kKeyData.w = false;
            break;
        case 'a':
            kKeyData.a = false;
            break;
        case 's':
            kKeyData.s = false;
            break;
        case 'd':
            kKeyData.d = false;
            break;
        case 'b':
        	kKeyData.highlightedBone = (kKeyData.highlightedBone + 1) % jester::Bone::BONE_COUNT;
        	printf("Highlighting Bone: %d\n", kKeyData.highlightedBone);
        	break;
        }
	}

	void mousePosition(int x, int y) {
		int xOffset = kRenderData.windowWidth / 2 - x;
		int yOffset = kRenderData.windowHeight / 2 - y;

		if (abs(xOffset) > 2)
			kKeyData.mouseOffsetX += xOffset;
		if (abs(yOffset) > 2)
			kKeyData.mouseOffsetY += yOffset;
	}

private:
	jester::Controller *kController;
	jester::LeapCarmineFuser *kFuser;
	jester::Scene *kScene;
	jester::Sensor *kCarmine;
	jester::Sensor *kLeap;

	Mesh *kBoneMesh;
	Mesh *kJointMesh;
	RenderData kRenderData;
	KeyData kKeyData;
	float kDegToRad;

	void initialize() {
		kRenderData.windowHeight = 600;
		kRenderData.windowWidth = 600;
		kKeyData.z = 5.0f;
		kKeyData.x = 0.f;
		kKeyData.yaw = 0.f;
		kKeyData.pitch = 0.f;
		kKeyData.highlightedBone = 0;
		kKeyData.captureMouse = true;

		glutInitWindowPosition(20, 20);
		glutInitWindowSize(kRenderData.windowWidth, kRenderData.windowHeight);
		glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
		glutCreateWindow("Jester Skeleton Visualizer");
		glutReshapeFunc(ReshapeCamera);
	   	glutDisplayFunc(DrawSkeleton);
	   	glutKeyboardFunc(KeyDown);
    	glutKeyboardUpFunc(KeyUp);
    	glutSetCursor(GLUT_CURSOR_NONE);
    	glutWarpPointer(kRenderData.windowWidth / 2, kRenderData.windowHeight / 2);
    	glutPassiveMotionFunc(MousePosition);
    	glutTimerFunc(1, KeyDownUpdate, 1);

		// Start Of User Initialization
		glClearColor (1.0f, 1.0f, 1.0f, 1.0f);								
		// Black Background
	 	glClearDepth (1.0f);		// Depth Buffer Setup
	 	glDepthFunc (GL_LEQUAL);	// The Type Of Depth Testing
		glEnable (GL_DEPTH_TEST);	// Enable Depth Testing
		glEnable(GL_CULL_FACE);
	    glCullFace(GL_BACK);

	    kBoneMesh = GeometryCreator::CreateCylinder(0.025, 0.025, 1, 15, 15);
	    kJointMesh = GeometryCreator::CreateSphere(glm::vec3(0.05));

		kKeyData.w = false;
		kKeyData.a = false;
		kKeyData.s = false;
		kKeyData.d = false;
	}

	void SetProjectionMatrix() {
	    glm::mat4 Projection = glm::perspective(3.1415f / 4.0f, ((float) kRenderData.windowWidth)/ ((float) kRenderData.windowHeight), 0.1f, 100.f);
	    safe_glUniformMatrix4fv(kRenderData.uProjMatrix, glm::value_ptr(Projection));
	}

	void SetView(glm::mat4 cameraTrans) {
		glm::vec3 cameraPos(cameraTrans * glm::vec4(0.f, 0.f, 0.f, 1.f));
		glm::vec3 look(cameraTrans * glm::vec4(0.f, 0.f, -1.f, 1.f));
		glm::vec3 up(cameraTrans * glm::vec4(0.f, 1.f, 0.f, 0.f));
	    glm::mat4 View = glm::lookAt(cameraPos, look, up);
	    safe_glUniformMatrix4fv(kRenderData.uViewMatrix, glm::value_ptr(View));
	}

	void SetModel(glm::mat4 modelMat) {
	    safe_glUniformMatrix4fv(kRenderData.uModelMatrix, glm::value_ptr(modelMat));
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

	void drawBones() {
		safe_glDisableVertexAttribArray(kRenderData.aPosition);
		safe_glDisableVertexAttribArray(kRenderData.aNormal);

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

        jester::Bone *rootBone = kScene->getBone(jester::Bone::ROOT);
        glm::vec3 rootTrans = rootBone->getWorldPosition();
        rootTrans.y *= -1;

        for (int i = 0; i < jester::Bone::BoneId::BONE_COUNT; i++) {
        	jester::Bone *curBone = kScene->getBone(jester::Bone::intToBoneId(i));

        	if (i == kKeyData.highlightedBone) {
				glUniform3f(kRenderData.uColor, 0.409f, 0.409f, 0.409f);
			} else {
				glUniform3f(kRenderData.uColor, 0.909f, 0.409f, 0.409f);
			}
			glm::mat4 boneModelMatrix = glm::translate(glm::mat4(1), -rootTrans) * 
				glm::scale(curBone->getWorldTransform(), glm::vec3(curBone->getWidth(), curBone->getWidth(), curBone->getLength()));

			SetModel(boneModelMatrix);
			glDrawElements(GL_TRIANGLES, kBoneMesh->IndexBufferLength, GL_UNSIGNED_SHORT, 0);
        }
	}

	void drawJoints() {
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

        jester::Bone *rootBone = kScene->getBone(jester::Bone::ROOT);
        glm::vec3 rootTrans = rootBone->getWorldPosition();
        rootTrans.y *= -1;

		for (int i = 0; i < jester::Bone::BoneId::BONE_COUNT; i++) {
			jester::Bone *curBone = kScene->getBone(jester::Bone::intToBoneId(i));

			if (curBone->getConfidence() > 0.5) {
				glUniform3f(kRenderData.uColor, 0.409f, 0.409f, 0.409f);
			} else {
				glUniform3f(kRenderData.uColor, 0.909f, 0.409f, 0.409f);
			}

			float scalingFactor = std::min(1.0f, curBone->getWidth());

			SetModel(glm::translate(glm::mat4(1), -rootTrans) * glm::translate(glm::mat4(1), curBone->getWorldPosition()) *
					glm::scale(glm::mat4(1), glm::vec3(scalingFactor, scalingFactor, scalingFactor)));
			glDrawElements(GL_TRIANGLES, kJointMesh->IndexBufferLength, GL_UNSIGNED_SHORT, 0);
		}

		//clean up 
		safe_glDisableVertexAttribArray(kRenderData.aPosition);
		safe_glDisableVertexAttribArray(kRenderData.aNormal);
	}
};

SkeletonVisualizer *globalVis = NULL;

extern "C" void ReshapeCamera(int width, int height) {
	globalVis->reshapeGL(width, height);
}

extern "C" void DrawSkeleton() {
	globalVis->drawSkeleton();
}

extern "C" void KeyDownUpdate(int param) {
	globalVis->keyDownUpdate(param);
}

extern "C" void KeyDown(unsigned char key, int x, int y) {
	globalVis->keyDown(key);
}

extern "C" void KeyUp(unsigned char key, int x, int y) {
	globalVis->keyUp(key);
}

extern "C" void MousePosition(int x, int y) {
	globalVis->mousePosition(x, y);
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

