#include "SceneGraphNode.h"
#include "Controller.h"
#include "PrimeSenseCarmineFactory.h"
#include "DataFusionModule.h"
#include "Bone.h"

#include <stdio.h>
#include <glm/vec3.hpp>

int main(int argc, char **argv) {
	jester::Controller *cont;
	jester::Scene* sceneRoot;
	jester::Sensor *carmine;

	cont = new jester::Controller(jester::DataFusionModule::FusionAlgorithm::PASS_THROUGH);

	cont->init();
	sceneRoot = cont->getScene();

	carmine = jester::PrimeSenseCarmineFactory::CreateCarmineSensor(sceneRoot, cont);
	 if (!carmine->start()) {
	 	printf("Sensor start failed\n");
	 	exit(1);
	 }
	while (1) {
		jester::Bone *head = sceneRoot->getBone(jester::Bone::BoneId::HAND_R);
		glm::vec3 headPos = head->getWorldPosition();
		printf("%5f %5f %5f\n",headPos.x, headPos.y, headPos.z);
	}
}