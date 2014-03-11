#include "DataFusionModule.h"

void jester::DataFusionModule::setDefaultSkeleton() {
	SceneGraphNode *sceneRoot = kBones.find(Bone::ROOT)->second->getParent();//Bone::DefaultPositions[Bone::ROOT]->getParent();

	//setSkeletonFromJoints(sceneRoot, Bone::DefaultPositions);
}

void jester::DataFusionModule::setSkeletonFromJoints(SceneGraphNode *positionParent, JointFusionData joints[Bone::JOINT_COUNT]) {
	//root
	glm::vec3 hipsMidpoint = *(joints[Bone::JointId::HIP_L].position) - *(joints[Bone::JointId::HIP_R].position);
	float hipMidpointConfidence = std::min(joints[Bone::JointId::HIP_L].confidence, joints[Bone::JointId::HIP_R].confidence);
	setBoneDataFromEndpoints(positionParent, positionParent, kBones.find(Bone::ROOT)->second, 
		hipsMidpoint, hipsMidpoint - glm::vec3(0, 0.2, 0), 
		hipMidpointConfidence);

	//hips
	setBoneDataFromEndpoints(positionParent, positionParent, kBones.find(Bone::PELVIS_L)->second, 
		hipsMidpoint, *(joints[Bone::JointId::HIP_L].position),
		std::min(hipMidpointConfidence, joints[Bone::JointId::HIP_L].confidence));
	setBoneDataFromEndpoints(positionParent, positionParent, kBones.find(Bone::PELVIS_R)->second, 
		hipsMidpoint, *(joints[Bone::JointId::HIP_R].position),
		std::min(hipMidpointConfidence, joints[Bone::JointId::HIP_R].confidence));

	//femurs
	setBoneDataFromEndpoints(positionParent, positionParent, kBones.find(Bone::FEMUR_L)->second, 
		*(joints[Bone::JointId::HIP_L].position), *(joints[Bone::JointId::KNEE_L].position),
		std::min(joints[Bone::JointId::HIP_L].confidence, joints[Bone::JointId::KNEE_L].confidence));
	setBoneDataFromEndpoints(positionParent, positionParent, kBones.find(Bone::FEMUR_R)->second, 
		*(joints[Bone::JointId::HIP_R].position), *(joints[Bone::JointId::KNEE_R].position),
		std::min(joints[Bone::JointId::HIP_R].confidence, joints[Bone::JointId::KNEE_R].confidence));

	//tibias
	setBoneDataFromEndpoints(positionParent, positionParent, kBones.find(Bone::TIBIA_L)->second, 
		*(joints[Bone::JointId::KNEE_L].position), *(joints[Bone::JointId::ANKLE_L].position),
		std::min(joints[Bone::JointId::KNEE_L].confidence, joints[Bone::JointId::ANKLE_L].confidence));
	setBoneDataFromEndpoints(positionParent, positionParent, kBones.find(Bone::TIBIA_R)->second, 
		*(joints[Bone::JointId::KNEE_R].position), *(joints[Bone::JointId::ANKLE_R].position),
		std::min(joints[Bone::JointId::KNEE_R].confidence, joints[Bone::JointId::ANKLE_R].confidence));

	//spinal column.skull
	setBoneDataFromEndpoints(positionParent, positionParent, kBones.find(Bone::SPINE)->second, 
		hipsMidpoint, *(joints[Bone::JointId::C7].position),
		std::min(hipMidpointConfidence, joints[Bone::JointId::C7].confidence));
	setBoneDataFromEndpoints(positionParent, positionParent, kBones.find(Bone::NECK)->second, 
		*(joints[Bone::JointId::C7].position), *(joints[Bone::JointId::HEAD].position),
		std::min(joints[Bone::JointId::C7].confidence, joints[Bone::JointId::HEAD].confidence));
	setBoneDataFromEndpoints(positionParent, positionParent, kBones.find(Bone::SKULL)->second, 
		*(joints[Bone::JointId::HEAD].position), *(joints[Bone::JointId::HEAD].position) + glm::vec3(0, 0.2, 0),
		joints[Bone::JointId::HEAD].confidence);

	//collars
	setBoneDataFromEndpoints(positionParent, positionParent, kBones.find(Bone::COLLAR_L)->second, 
		*(joints[Bone::JointId::C7].position), *(joints[Bone::JointId::SHOULDER_L].position),
		std::min(joints[Bone::JointId::C7].confidence, joints[Bone::JointId::SHOULDER_L].confidence));
	setBoneDataFromEndpoints(positionParent, positionParent, kBones.find(Bone::COLLAR_R)->second, 
		*(joints[Bone::JointId::C7].position), *(joints[Bone::JointId::SHOULDER_R].position),
		std::min(joints[Bone::JointId::C7].confidence, joints[Bone::JointId::SHOULDER_R].confidence));

	//Humerus's
	setBoneDataFromEndpoints(positionParent, positionParent, kBones.find(Bone::HUMERUS_L)->second, 
		*(joints[Bone::JointId::SHOULDER_L].position), *(joints[Bone::JointId::ELBOW_L].position),
		std::min(joints[Bone::JointId::SHOULDER_L].confidence, joints[Bone::JointId::ELBOW_L].confidence));
	setBoneDataFromEndpoints(positionParent, positionParent, kBones.find(Bone::HUMERUS_R)->second, 
		*(joints[Bone::JointId::SHOULDER_R].position), *(joints[Bone::JointId::ELBOW_R].position),
		std::min(joints[Bone::JointId::SHOULDER_R].confidence, joints[Bone::JointId::ELBOW_R].confidence));

	//Radii
	setBoneDataFromEndpoints(positionParent, positionParent, kBones.find(Bone::RADIUS_L)->second, 
		*(joints[Bone::JointId::ELBOW_L].position), *(joints[Bone::JointId::WRIST_L].position),
		std::min(joints[Bone::JointId::ELBOW_L].confidence, joints[Bone::JointId::WRIST_L].confidence));
	setBoneDataFromEndpoints(positionParent, positionParent, kBones.find(Bone::RADIUS_R)->second, 
		*(joints[Bone::JointId::ELBOW_R].position), *(joints[Bone::JointId::WRIST_R].position),
		std::min(joints[Bone::JointId::ELBOW_R].confidence, joints[Bone::JointId::WRIST_R].confidence));
}

void jester::DataFusionModule::setSkeletonBones(FusionBone *bones[Bone::JOINT_COUNT]) {
	for (int i = 0; i < Bone::BONE_COUNT; i++) {
		kBones.insert(std::pair<Bone::BoneId, FusionBone *>(bones[i]->getType(), bones[i]));
	}
}

void jester::DataFusionModule::setBoneDataFromEndpoints(SceneGraphNode *curStartParent, SceneGraphNode *curEndParent,
			FusionBone *bone, glm::vec3 startPos, glm::vec3 endPos, float confidence) {
	glm::vec3 worldStartPosition = glm::vec3(curStartParent->getWorldTransform() * glm::vec4(startPos, 1.f));
	glm::vec3 worldEndPosition = glm::vec3(curEndParent->getWorldTransform() * glm::vec4(startPos, 1.f));
	glm::mat4 parentTransform = bone->getParent()->getWorldTransform();
	glm::mat4 invParentWorldTransform = glm::inverse(parentTransform);

	glm::vec3 parentSpaceBoneVector = glm::vec3(invParentWorldTransform * glm::vec4(worldEndPosition - worldStartPosition, 0.f));
	glm::vec3 parentVector(0, 0 , 1);
	glm::vec3 normvector = glm::normalize(parentSpaceBoneVector);
	float angle = glm::dot(parentVector, normvector);
	glm::vec3 axis = glm::cross(parentVector, normvector);

	bone->setPosition(glm::vec3(invParentWorldTransform * glm::vec4(worldStartPosition, 1)), confidence);
	bone->setLength(glm::distance(worldStartPosition, worldEndPosition));
	bone->setOrientation(glm::rotate(glm::quat(glm::vec3(0, 0, 0)), angle, axis), confidence);
}