#include "SceneGraphNode.h"
#include "Controller.h"
#include "PrimeSenseCarmine.h"

int main(int argc, char **argv) {
	jester::Controller cont;
	jester::Scene* sceneRoot;
	jester::PrimeSenseCarmine *carmine;

	cont.init();
	sceneRoot = cont.getScene();

	//traverse scene graph
	//add sensors to scene
	carmine = new jester::PrimeSenseCarmine(sceneRoot);
	
}