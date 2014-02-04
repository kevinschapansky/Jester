#include "SceneGraphNode.h"
#include "Controller.h"

int main(int argc, char **argv) {
	jester::Controller cont;
	jester::Scene* sceneRoot;

	cont.init();
	sceneRoot = cont.getScene();

	//traverse scene graph
	//add sensors to scene

}