#include "Bone.h"

glm::quat jester::Bone::getWorldOrientation() {
	return glm::quat();
}

glm::vec3 jester::Bone::getWorldPosition() {
	return kPosition;
}

jester::Bone::BoneId jester::Bone::getType() {
	return kType;
}

jester::Bone::Bone(SceneGraphNode *parent, BoneId type) : SceneGraphNode(parent) {
	kType = type;
}

jester::Bone::~Bone() {
	
}