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

jester::FusionBone::FusionBone(SceneGraphNode *parent, BoneId type) : Bone(parent, type) {

}
