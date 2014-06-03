#include "LeapCarmineFuser.h"

const float jester::LeapCarmineFuser::SwitchDelta = 0.25f;
const int jester::LeapCarmineFuser::BonesPerHand = 5;

void jester::LeapCarmineFuser::setCarmine(Sensor *carmine, std::map<jester::Bone::BoneId, double> carmineConfMap) {
	kCarmine = carmine;
	BasicDataFuser::addSensor(carmine, carmineConfMap);
}

void jester::LeapCarmineFuser::setLeap(Sensor *leap, std::map<jester::Bone::BoneId, double> leapConfMap) {
	kLeap = leap;
	BasicDataFuser::addSensor(leap, leapConfMap);
}

void jester::LeapCarmineFuser::handleSwaps(std::map<Sensor *, std::map<Bone::BoneId, BoneFusionData>> *dataset) {
	std::map<Bone::BoneId, BoneFusionData> leapData = dataset->find(kLeap)->second;
	std::map<Bone::BoneId, BoneFusionData> carmineData = dataset->find(kCarmine)->second;

	std::map<Bone::BoneId, BoneFusionData>::iterator LLIt = leapData.find(Bone::RADIUS_L);
	std::map<Bone::BoneId, BoneFusionData>::iterator LRIt = leapData.find(Bone::RADIUS_R);
	std::map<Bone::BoneId, BoneFusionData>::iterator CLIt = carmineData.find(Bone::RADIUS_L);
	std::map<Bone::BoneId, BoneFusionData>::iterator CRIt = carmineData.find(Bone::RADIUS_R);

	glm::vec3 leapLeft, leapRight, carmineLeft, carmineRight;

	//get worldspace endpoints if they exist
	if (LLIt != leapData.end())
		leapLeft = glm::vec3(kLeap->getWorldTransform() * glm::vec4(getEndpointFromBoneData(LLIt->second), 1.0));
	if (LRIt != leapData.end())
		leapRight = glm::vec3(kLeap->getWorldTransform() * glm::vec4(getEndpointFromBoneData(LRIt->second), 1.0));
	if (CLIt != carmineData.end())
		carmineLeft = glm::vec3(kCarmine->getWorldTransform() * glm::vec4(getEndpointFromBoneData(CLIt->second), 1.0));
	if (CRIt != carmineData.end())
		carmineRight = glm::vec3(kCarmine->getWorldTransform() * glm::vec4(getEndpointFromBoneData(CRIt->second), 1.0));

	//get position deltas
	float lNo, lYes, rNo, rYes;
	lNo = lYes = rNo = rYes = FLT_MAX;

	if (LLIt != leapData.end() && CLIt != carmineData.end()) {
		lNo = glm::distance(leapLeft, carmineLeft);
	}
	if (LLIt != leapData.end() && CRIt != carmineData.end()) {
		lYes = glm::distance(leapLeft, carmineRight);
	}
	if (LRIt != leapData.end() && CRIt != carmineData.end()) {
		rNo = glm::distance(leapRight, carmineRight);
	}
	if (LRIt != leapData.end() && CLIt != carmineData.end()) {
		rYes = glm::distance(leapRight, carmineLeft);
	}

	bool swapEnabled = false;

	//see if a swap is super helpful
	if (CLIt != carmineData.end() && CRIt != carmineData.end()) {
		if (LLIt != leapData.end() && LRIt == leapData.end()) {
			if (lNo > SwitchDelta && lYes < SwitchDelta) {
				swapEnabled = true;
			}
		} else if (LRIt != leapData.end() && LLIt == leapData.end()) {
			if (rNo > SwitchDelta && rYes < SwitchDelta) {
				swapEnabled = true;
			}
		} else if (LLIt != leapData.end() && LRIt != leapData.end()) {
			if (lYes < lNo && rYes < rNo) {
				swapEnabled = true;
			}
		}
	}


	if (swapEnabled) {
		for (int a = Bone::PHALANX_L_1, b = Bone::PHALANX_R_1; a <= Bone::PHALANX_L_5; a++, b++) {
			swapBone(Bone::intToBoneId(a), Bone::intToBoneId(b), &leapData);
		}
		swapBone(Bone::RADIUS_L, Bone::RADIUS_R, &leapData);
	}
	dataset->operator[](kLeap) = leapData;
	dataset->operator[](kCarmine) = carmineData;
}

void jester::LeapCarmineFuser::swapBone(Bone::BoneId a, Bone::BoneId b, std::map<Bone::BoneId, BoneFusionData> *set) {
	std::map<Bone::BoneId, BoneFusionData>::iterator aIt = set->find(a);
	std::map<Bone::BoneId, BoneFusionData>::iterator bIt = set->find(b);

	BoneFusionData tempA;

	if (aIt != set->end()) {
		tempA = set->operator[](a);
	}
	if (bIt != set->end()) {
		set->operator[](a) = set->operator[](b);
	} else if (aIt != set->end()){
		set->erase(aIt);
	}
	if (aIt != set->end()) {
		set->operator[](b) = tempA;
	} else if (bIt != set->end()){
		set->erase(bIt);
	}
}

jester::LeapCarmineFuser::LeapCarmineFuser() {
	kCarmine = kLeap = NULL;
}

jester::LeapCarmineFuser::~LeapCarmineFuser() {

}

jester::DataFusionModule* jester::LeapCarmineFuserFactory::CreateFusionModule() {
	return new LeapCarmineFuser();
}
