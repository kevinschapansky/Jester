#include "Controller.h"
#include "PassThroughFuser.h"


jester::Controller::Controller(DataFusionModule::FusionAlgorithm algorithm) {
	kStartClock = std::clock();
	kScene = NULL;
	kAlgorithm = algorithm;
}

jester::Controller::~Controller() {
	delete kScene;
	delete kFusionModule;
}

void jester::Controller::init() {
	if (kScene != NULL) {
		delete kScene;
	}

	switch (kAlgorithm) {
	case DataFusionModule::PASS_THROUGH :
		kFusionModule = new PassThroughFuser();
		kScene = new Scene(kFusionModule);
		break;
	default :
		printf("SHITS BROKE\n");
	};
}

jester::Scene* jester::Controller::getScene() {
	return kScene;
}

long int jester::Controller::getTimestamp() {
	float clocks;

	clocks = (float) (std::clock() - kStartClock);
	return (long int) (clocks / CLOCKS_PER_SEC);
}

void jester::Controller::suggestBoneInfo(Sensor *sensor, std::vector<BoneFusionData *> data) {
	kFusionModule->newData(sensor, data);
}
		
void jester::Controller::suggestJointInfo(Sensor *sensor, std::vector<JointFusionData *> data) {
	kFusionModule->newData(sensor, data);
}