#include "FusionBone.h"

void jester::FusionBone::setPosition(glm::vec3 position) {
	kPosition = position;
}

jester::FusionBone::FusionBone(SceneGraphNode *parent, BoneId type) : Bone(parent, type) {
	
}