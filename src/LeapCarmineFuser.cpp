#include "LeapCarmineFuser.h"

const float jester::LeapCarmineFuser::SwitchDelta = 0.1f;
const int jester::LeapCarmineFuser::JointsPerHand = 10;

void jester::LeapCarmineFuser::newData(Sensor *sensor, std::map<Bone::BoneId, BoneFusionData> data) {

}

void jester::LeapCarmineFuser::newData(Sensor *sensor, std::map<Bone::JointId, JointFusionData> data) {

}

void jester::LeapCarmineFuser::setCarmine(Sensor *carmine) {
	kCarmine = carmine;
}

void jester::LeapCarmineFuser::setLeap(Sensor *leap) {
	kLeap = leap;
}

jester::LeapCarmineFuser::LeapCarmineFuser() {
	kCarmine = kLeap = NULL;
}

jester::LeapCarmineFuser::~LeapCarmineFuser() {

}

jester::DataFusionModule* jester::LeapCarmineFuserFactory::CreateFusionModule() {
	return new LeapCarmineFuser();
}
