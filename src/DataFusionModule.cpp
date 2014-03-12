#include "DataFusionModule.h"

void jester::DataFusionModule::setDefaultSkeleton() {
	SceneGraphNode *sceneRoot = kBones.find(Bone::ROOT)->second->getParent();
	JointFusionData data[Bone::JOINT_COUNT];

	for (int i = 0; i < Bone::JOINT_COUNT; i++) {
		data[i].id = Bone::intToJointId(i);
		data[i].confidence = 0;
		data[i].position = new glm::vec3(Bone::DefaultPositions[i]);
	}

	setSkeletonFromJoints(sceneRoot, data);

	for (int i = 0; i < Bone::JOINT_COUNT; i++)
		delete data[i].position;
}

void jester::DataFusionModule::setSkeletonFromJoints(SceneGraphNode *positionParent, JointFusionData joints[Bone::JOINT_COUNT]) {

	for (unsigned int i = 0; i < Bone::BONE_COUNT; i++) {
		Bone::BoneId id = Bone::intToBoneId(i);
		std::pair<Bone::JointId, Bone::JointId> jointIds = Bone::JointToBoneMapping.find(id)->second;
		glm::vec3 *startPos = joints[jointIds.first].position;
		glm::vec3 *endPos = joints[jointIds.second].position;
		float confidence = std::min(joints[jointIds.first].confidence, joints[jointIds.second].confidence);

		if (startPos != NULL && endPos != NULL)
			setBoneDataFromEndpoints(positionParent, positionParent, kBones.find(id)->second, *startPos, *endPos, confidence);
	}
}

void jester::DataFusionModule::setSkeletonBones(FusionBone *bones[Bone::JOINT_COUNT]) {
	for (int i = 0; i < Bone::BONE_COUNT; i++) {
		kBones.insert(std::pair<Bone::BoneId, FusionBone *>(bones[i]->getType(), bones[i]));
	}
}

void jester::DataFusionModule::setBoneDataFromEndpoints(SceneGraphNode *curStartParent, SceneGraphNode *curEndParent,
			FusionBone *bone, glm::vec3 startPos, glm::vec3 endPos, float confidence) {
	glm::vec3 worldStartPosition = glm::vec3(curStartParent->getWorldTransform() * glm::vec4(startPos, 1.f));
	glm::vec3 worldEndPosition = glm::vec3(curEndParent->getWorldTransform() * glm::vec4(endPos, 1.f));
	glm::mat4 parentTransform = bone->getParent()->getWorldTransform();
	glm::mat4 invParentWorldTransform = glm::inverse(parentTransform);

	glm::vec3 parentSpaceBoneVector = glm::vec3(invParentWorldTransform * glm::vec4(worldEndPosition - worldStartPosition, 0.f));
	glm::vec3 parentVector(0, 0 , 1);
	glm::vec3 normvector = glm::normalize(parentSpaceBoneVector);
	float vectorDot = glm::dot(parentVector, normvector);
	float angle = glm::acos(vectorDot);

	if (vectorDot > 0.9999) {
		bone->setOrientation(glm::quat(glm::vec3(0, 0, 0)), confidence);
	} else if (vectorDot < -0.9999) {
		bone->setOrientation(glm::quat(glm::vec3(0, 3.14, 0)), confidence);
	} else {
		glm::vec3 axis = glm::normalize(glm::cross(parentVector, normvector));
		bone->setOrientation(glm::angleAxis(angle, axis), confidence);
	}

	bone->setPosition(glm::vec3(invParentWorldTransform * glm::vec4(worldStartPosition, 1)), confidence);
	bone->setLength(glm::distance(worldStartPosition, worldEndPosition));
}