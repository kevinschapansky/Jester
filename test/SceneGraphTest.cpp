#include "SceneGraphNode.h"
#include "Controller.h"

int main(int argc, char **argv) {
	jester::Controller cont;
	jester::SceneGraphNode* sceneRoot;

	cont.init();
	sceneRoot = cont.getScene();

	//traverse scene graph
	//add sensors to scene

}