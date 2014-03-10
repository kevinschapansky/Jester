#include "Bone.h"

const glm::vec3 jester::Bone::DefaultPositions[BoneId::BONE_COUNT] = {
	glm::vec3(0, 1.82, 0),
	glm::vec3(0, 1.52, 0),
	glm::vec3(.30, 1.46, 0),
	glm::vec3(-.30, 1.46, 0),
	glm::vec3(.30, 1.22, 0),
	glm::vec3(-.30, 1.22, 0),
	glm::vec3(.35, .82, 0),
	glm::vec3(-.35, .82, 0),
	glm::vec3(.15, .92, 0),
	glm::vec3(-.15, .92 ,0),
	glm::vec3(.15, .51, 0),
	glm::vec3(-.15, .51, 0),
	glm::vec3(.15, 0, 0),
	glm::vec3(-.15, 0, 0)
};

jester::Bone::BoneId jester::Bone::getType() {
	return kType;
}

float jester::Bone::getConfidence() {
	return kConfidence;
}

jester::Bone::Bone(SceneGraphNode *parent, BoneId type) : SceneGraphNode(parent) {
	kType = type;
	kPosition = DefaultPositions[kType];
	kConfidence = 0.0;
}

jester::Bone::~Bone() {
	
}