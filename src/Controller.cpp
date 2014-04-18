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

void jester::Controller::suggestBoneInfo(Sensor *sensor, std::map<Bone::BoneId, BoneFusionData> data) {
	kFusionModule->newData(sensor, data);
}
		
void jester::Controller::suggestJointInfo(Sensor *sensor, std::map<Bone::JointId, JointFusionData> data) {
	kFusionModule->newData(sensor, data);
}
