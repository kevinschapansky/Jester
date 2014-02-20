#include "FusionBone.h"

void jester::FusionBone::setPosition(glm::vec3 position, float confidence) {
	kPosition = position;
	kConfidence = confidence;
}

jester::FusionBone::FusionBone(SceneGraphNode *parent, BoneId type) : Bone(parent, type) {

}