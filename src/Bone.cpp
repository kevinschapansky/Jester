#include "Bone.h"

//Positions for the default joint positions. Must be in the same order as Bone::JointId
//and be fully complete. Expected to be specified in world space
const glm::vec3 jester::Bone::DefaultPositions[JointId::JOINT_COUNT] = {
	glm::vec3(0, 1.62, 0),
	glm::vec3(0, 1.52, 0), //C7
	glm::vec3(.30, 1.46, 0),
	glm::vec3(-.30, 1.46, 0),
	glm::vec3(.30, 1.22, 0),
	glm::vec3(-.30, 1.22, 0),
	glm::vec3(.35, .82, 0), //wrist L 82
	glm::vec3(-.35, .82, 0),
	glm::vec3(0, 0.92, 0), //Pelvis midpoint
	glm::vec3(.15, .92, 0),
	glm::vec3(-.15, .92 ,0),
	glm::vec3(.15, .51, 0),
	glm::vec3(-.15, .51, 0),
	glm::vec3(.15, 0, 0),
	glm::vec3(-.15, 0, 0),

	glm::vec3(.30, .70, 0), //DL1
	glm::vec3(.32, .64, 0), //DL2
	glm::vec3(.35, .62, 0), //DL3
	glm::vec3(.375, .64, 0), //DL4
	glm::vec3(.40, .66, 0), //DL5

	glm::vec3(-.30, .70, 0), //DR1
	glm::vec3(-.32, .64, 0), //DR2
	glm::vec3(-.35, .62, 0), //DR3
	glm::vec3(-.375, .64, 0), //DR4
	glm::vec3(-.40, .66, 0), //DR5

	glm::vec3(.30, .76, 0), //MCL1
	glm::vec3(.32, .74, 0), //MCL2
	glm::vec3(.35, .72, 0), //MCL3
	glm::vec3(.375, .72, 0), //MCL4
	glm::vec3(.40, .73, 0), //MCL5

	glm::vec3(-.30, .76, 0), //MCR1
	glm::vec3(-.32, .74, 0), //MCR2
	glm::vec3(-.35, .72, 0), //MCR3
	glm::vec3(-.375, .72, 0), //MCR4
	glm::vec3(-.40, .73, 0), //MCR5
	glm::vec3(0, 1.85, 0), //extend up
	glm::vec3(0, .8, 0) //extend down
};

const std::map<jester::Bone::BoneId, std::pair<jester::Bone::JointId, jester::Bone::JointId>> jester::Bone::BoneToJointsMap = {
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
	{jester::Bone::BoneId::RADIUS_R, std::make_pair(jester::Bone::JointId::ELBOW_R, jester::Bone::JointId::WRIST_R)},

	{jester::Bone::BoneId::PHALANX_L_1, std::make_pair(jester::Bone::JointId::METACARPO_L_1, jester::Bone::JointId::DISTAL_L_1)},
	{jester::Bone::BoneId::PHALANX_L_2, std::make_pair(jester::Bone::JointId::METACARPO_L_2, jester::Bone::JointId::DISTAL_L_2)},
	{jester::Bone::BoneId::PHALANX_L_3, std::make_pair(jester::Bone::JointId::METACARPO_L_3, jester::Bone::JointId::DISTAL_L_3)},
	{jester::Bone::BoneId::PHALANX_L_4, std::make_pair(jester::Bone::JointId::METACARPO_L_4, jester::Bone::JointId::DISTAL_L_4)},
	{jester::Bone::BoneId::PHALANX_L_5, std::make_pair(jester::Bone::JointId::METACARPO_L_5, jester::Bone::JointId::DISTAL_L_5)},

	{jester::Bone::BoneId::PHALANX_R_1, std::make_pair(jester::Bone::JointId::METACARPO_R_1, jester::Bone::JointId::DISTAL_R_1)},
	{jester::Bone::BoneId::PHALANX_R_2, std::make_pair(jester::Bone::JointId::METACARPO_R_2, jester::Bone::JointId::DISTAL_R_2)},
	{jester::Bone::BoneId::PHALANX_R_3, std::make_pair(jester::Bone::JointId::METACARPO_R_3, jester::Bone::JointId::DISTAL_R_3)},
	{jester::Bone::BoneId::PHALANX_R_4, std::make_pair(jester::Bone::JointId::METACARPO_R_4, jester::Bone::JointId::DISTAL_R_4)},
	{jester::Bone::BoneId::PHALANX_R_5, std::make_pair(jester::Bone::JointId::METACARPO_R_5, jester::Bone::JointId::DISTAL_R_5)}
};
const float jester::Bone::SkullWidth = 3.0f;
const float jester::Bone::PhalanxWidth = 0.5f;
const float jester::Bone::RootWidth = 0.f;
const float jester::Bone::DefaultWidth = 1.0f;
const float jester::Bone::DefaultLength = 1.0f;
const float jester::Bone::DefaultConfidence = 0.f;

jester::Bone::BoneId jester::Bone::getType() {
	return kType;
}

float jester::Bone::getConfidence() {
	return kConfidence;
}

float jester::Bone::getLength() {
	return kLength;
}

float jester::Bone::getWidth() {
	return kWidth;
}

std::pair<glm::vec3, glm::vec3> jester::Bone::getDefaultJointPositions() {
	if (kDefaultJointPositions == NULL)
		return std::make_pair(glm::vec3(0), glm::vec3(0));
	return *kDefaultJointPositions;
}

jester::Bone::Bone(SceneGraphNode *parent, BoneId type) : SceneGraphNode(parent) {
	kType = type;
	kPosition = DefaultPositions[kType];
	kConfidence = Bone::DefaultConfidence;
	kLength = Bone::DefaultLength;
	kDefaultJointPositions = NULL;

	switch (type) {
	case Bone::BoneId::SKULL:
		kWidth = Bone::SkullWidth;
		break;
	case Bone::BoneId::PHALANX_L_1:
	case Bone::BoneId::PHALANX_L_2:
	case Bone::BoneId::PHALANX_L_3:
	case Bone::BoneId::PHALANX_L_4:
	case Bone::BoneId::PHALANX_L_5:
	case Bone::BoneId::PHALANX_R_1:
	case Bone::BoneId::PHALANX_R_2:
	case Bone::BoneId::PHALANX_R_3:
	case Bone::BoneId::PHALANX_R_4:
	case Bone::BoneId::PHALANX_R_5:
		kWidth = Bone::PhalanxWidth;
		break;
	case Bone::BoneId::ROOT:
		kWidth = Bone::RootWidth;
		break;
	default:
		kWidth = Bone::DefaultWidth;
	};
}

jester::Bone::~Bone() {
	
}
