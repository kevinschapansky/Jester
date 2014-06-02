#include "LeapCarmineFuser.h"

const float jester::LeapCarmineFuser::SwitchDelta = 0.1f;
const int jester::LeapCarmineFuser::JointsPerHand = 10;

void jester::LeapCarmineFuser::newData(Sensor *sensor, std::map<Bone::JointId, JointFusionData> data) {

	/*if (data.find(Bone::WRIST_L) != data.end()) {
		glm::vec3 world(sensor->getWorldTransform() * glm::vec4(data.find(Bone::WRIST_L)->second.position, 1));
		if (sensor == kCarmine) {
			printf("Carmine: %2.4f %2.4f %2.4f\n", world.x, world.y, world.z);
		} else if (sensor == kLeap) {
			printf("LeapMot: %2.4f %2.4f %2.4f\n", world.x, world.y, world.z);
		}
	}*/

	BasicDataFuser::newData(sensor, data);
}

void jester::LeapCarmineFuser::setCarmine(Sensor *carmine, std::map<jester::Bone::BoneId, double> carmineConfMap) {
	kCarmine = carmine;
	BasicDataFuser::addSensor(carmine, carmineConfMap);
}

void jester::LeapCarmineFuser::setLeap(Sensor *leap, std::map<jester::Bone::BoneId, double> leapConfMap) {
	kLeap = leap;
	BasicDataFuser::addSensor(leap, leapConfMap);
}

void jester::LeapCarmineFuser::insertBoneDataIntoFrame(int frame, Sensor* sensor, std::map<Bone::BoneId, BoneFusionData> bones) {
	std::map<Bone::BoneId, BoneFusionData>::iterator existingLeft = kBoneHistory[frame].fusedBoneData.find(Bone::RADIUS_L);
	std::map<Bone::BoneId, BoneFusionData>::iterator existingRight = kBoneHistory[frame].fusedBoneData.find(Bone::RADIUS_R);
	std::map<Bone::BoneId, BoneFusionData>::iterator newLeft = bones.find(Bone::RADIUS_L);
	std::map<Bone::BoneId, BoneFusionData>::iterator newRight = bones.find(Bone::RADIUS_R);

	float swapError = FLT_MAX;
	float keepError = FLT_MAX;

	if (sensor == kLeap) {
		if (newLeft != bones.end()) {
			if (existingRight != kBoneHistory[frame].fusedBoneData.end()) {

			} else if (existingLeft != kBoneHistory[frame].fusedBoneData.end()){

			}
		} else if (newRight != bones.end()) {

		}
	} else if (sensor == kCarmine) {

	} else {
		printf("Unknown sensor reporting, this fusion implementation only supports the LeapMotion and the Carmine\n");
		exit(1);
	}

	BasicDataFuser::insertBoneDataIntoFrame(frame, sensor, bones);
}

jester::LeapCarmineFuser::LeapCarmineFuser() {
	kCarmine = kLeap = NULL;
}

jester::LeapCarmineFuser::~LeapCarmineFuser() {

}

jester::DataFusionModule* jester::LeapCarmineFuserFactory::CreateFusionModule() {
	return new LeapCarmineFuser();
}
