#include "SceneGraphNode.h"
#include "Controller.h"
#include "PrimeSenseCarmineFactory.h"
#include "DataFusionModule.h"

#include <thread>
#include <chrono>

int main(int argc, char **argv) {
	jester::Controller *cont;
	jester::Scene* sceneRoot;
	jester::Sensor *carmine;

	cont = new jester::Controller(jester::DataFusionModule::FusionAlgorithm::PASS_THROUGH);

	cont->init();
	sceneRoot = cont->getScene();

	//traverse scene graph
	//add sensors to scene
	carmine = jester::PrimeSenseCarmineFactory::CreateCarmineSensor(sceneRoot, cont);
	carmine->start();
	while (1);
	std::this_thread::sleep_for(std::chrono::seconds(1));
}