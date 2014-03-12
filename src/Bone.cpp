#include "Bone.h"

const glm::vec3 jester::Bone::DefaultPositions[JointId::JOINT_COUNT] = {
	glm::vec3(0, 1.82, 0),
	glm::vec3(0, 1.52, 0), //C7
	glm::vec3(.30, 1.46, 0),
	glm::vec3(-.30, 1.46, 0),
	glm::vec3(.30, 1.22, 0),
	glm::vec3(-.30, 1.22, 0),
	glm::vec3(.35, .82, 0),
	glm::vec3(-.35, .82, 0),
	glm::vec3(0, 0.92, 0), //Pelvis midpoint
	glm::vec3(.15, .92, 0),
	glm::vec3(-.15, .92 ,0),
	glm::vec3(.15, .51, 0),
	glm::vec3(-.15, .51, 0),
	glm::vec3(.15, 0, 0),
	glm::vec3(-.15, 0, 0),
	glm::vec3(0, 2, 0),
	glm::vec3(0, .8, 0)
};

const std::map<jester::Bone::BoneId, std::pair<jester::Bone::JointId, jester::Bone::JointId>> jester::Bone::JointToBoneMapping = {
	{jester::Bone::BoneId::ROOT, std::make_pair(jester::Bone::JointId::PELVIS_MIDPOINT, jester::Bone::JointId::EXTEND_DOWN)},

	{jester::Bone::BoneId::PELVIS_L, std::make_pair(jester::Bone::JointId::PELVIS_MIDPOINT, jester::Bone::JointId::HIP_L)},
	{jester::Bone::BoneId::PELVIS_R, std::make_pair(jester::Bone::JointId::PELVIS_MIDPOINT, jester::Bone::JointId::HIP_R)},

	{jester::Bone::BoneId::FEMUR_L, std::make_pair(jester::Bone::JointId::HIP_L, jester::Bone::JointId::KNEE_L)},
	{jester::Bone::BoneId::FEMUR_R, std::make_pair(jester::Bone::JointId::HIP_R, jester::Bone::JointId::KNEE_R)},

	{jester::Bone::BoneId::TIBIA_L, std::make_pair(jester::Bone::JointId::KNEE_L, jester::Bone::JointId::ANKLE_L)},
	{jester::Bone::BoneId::TIBIA_R, std::make_pair(jester::Bone::JointId::KNEE_R, jester::Bone::JointId::ANKLE_R)},

	{jester::Bone::BoneId::SPINE, std::make_pair(jester::Bone::JointId::PELVIS_MIDPOINT, jester::Bone::JointId::C7)},
	{jester::Bone::BoneId::NECK, std::make_pair(jester::Bone::JointId::C7, jester::Bone::JointId::HEAD)},
	{jester::Bone::BoneId::SKULL, std::make_pair(jester::Bone::JointId::HEAD, jester::Bone::JointId::EXTEND_UP)},

	{jester::Bone::BoneId::COLLAR_L, std::make_pair(jester::Bone::JointId::C7, jester::Bone::JointId::SHOULDER_L)},
	{jester::Bone::BoneId::COLLAR_R, std::make_pair(jester::Bone::JointId::C7, jester::Bone::JointId::SHOULDER_R)},

	{jester::Bone::BoneId::HUMERUS_L, std::make_pair(jester::Bone::JointId::SHOULDER_L, jester::Bone::JointId::ELBOW_L)},
	{jester::Bone::BoneId::HUMERUS_R, std::make_pair(jester::Bone::JointId::SHOULDER_R, jester::Bone::JointId::ELBOW_R)},

	{jester::Bone::BoneId::RADIUS_L, std::make_pair(jester::Bone::JointId::ELBOW_L, jester::Bone::JointId::WRIST_L)},
	{jester::Bone::BoneId::RADIUS_R, std::make_pair(jester::Bone::JointId::ELBOW_R, jester::Bone::JointId::WRIST_R)}
};

jester::Bone::BoneId jester::Bone::getType() {
	return kType;
}

float jester::Bone::getConfidence() {
	return kConfidence;
}

float jester::Bone::getLength() {
	return kLength;
}

jester::Bone::Bone(SceneGraphNode *parent, BoneId type) : SceneGraphNode(parent) {
	kType = type;
	kPosition = DefaultPositions[kType];
	kConfidence = 0.0;
}

jester::Bone::~Bone() {
	
}