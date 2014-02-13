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

int main(int argc, char **argv) {
	SkeletonVisualizer vis();
	// while (1) {
	// 	jester::Bone *head = sceneRoot->getBone(jester::Bone::BoneId::HAND_R);
	// 	glm::vec3 headPos = head->getWorldPosition();
	// 	printf("%5f %5f %5f\n",headPos.x, headPos.y, headPos.z);
	// }
}

