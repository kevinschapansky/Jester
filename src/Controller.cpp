#include "Controller.h"
#include "PassThroughFuser.h"


jester::Controller::Controller(DataFusionModule *fusionModule) {
	kStartClock = std::clock();
	kScene = NULL;
	kFusionModule = fusionModule;
}

jester::Controller::~Controller() {
	delete kScene;
	delete kFusionModule;
}

void jester::Controller::init() {
	if (kScene != NULL) {
		delete kScene;
	}

	kScene = new Scene(kFusionModule);
}

jester::Scene* jester::Controller::getScene() {
	return kScene;
}

long int jester::Controller::getTimestamp() {
	float clocks;

	clocks = (float) (std::clock() - kStartClock);
	return (long int) (clocks / CLOCKS_PER_SEC);
}

void jester::Controller::suggestBoneInfo(Sensor *sensor, BoneFusionData data[Bone::BONE_COUNT]) {
	kFusionModule->newData(sensor, data);
}
		
void jester::Controller::suggestJointInfo(Sensor *sensor, JointFusionData data[Bone::JOINT_COUNT]) {
	kFusionModule->newData(sensor, data);
}
