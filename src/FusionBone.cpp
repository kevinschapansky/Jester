#include "FusionBone.h"

void jester::FusionBone::setPosition(glm::vec3 position, float confidence) {
	kPosition = position;
	kConfidence = confidence;
}

void jester::FusionBone::setOrientation(glm::quat orientation, float confidence) {
	kOrientation = orientation;
	kConfidence = confidence;
}

void jester::FusionBone::setLength(float length) {
	kLength = length;
}

void jester::FusionBone::setWidth(float width) {
	kWidth = width;
}

void jester::FusionBone::setDefaultEndpoints(glm::vec3 start, glm::vec3 end) {
	if (kDefaultJointPositions == NULL)
		kDefaultJointPositions = new std::pair<glm::vec3, glm::vec3>(glm::vec3(0), glm::vec3(0));
	*kDefaultJointPositions = std::make_pair(start, end);
}

jester::FusionBone::FusionBone(SceneGraphNode *parent, BoneId type) : Bone(parent, type) {

}
