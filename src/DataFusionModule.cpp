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

	//populate the default endpoints for when bones are set with unknown joint positions
	for (int i = 0; i < Bone::BONE_COUNT; i++) {
		FusionBone *bone = kBones.find(Bone::intToBoneId(i))->second;
		std::pair<Bone::JointId, Bone::JointId> jointIds = Bone::BoneToJointsMap.find(Bone::intToBoneId(i))->second;
		glm::mat4 invParentTransform = glm::inverse(bone->getParent()->getWorldTransform());
		glm::vec3 parentSpaceStart = bone->getPosition();
		glm::vec3 parentSpaceEnd = glm::vec3(invParentTransform * glm::vec4(Bone::DefaultPositions[jointIds.second], 1));
		glm::vec3 parentBoneEndpoint = glm::vec3(0, 0, ((Bone*) bone->getParent())->getLength());

		bone->setDefaultEndpoints(parentSpaceStart - parentBoneEndpoint, parentSpaceEnd - parentBoneEndpoint);
	}

	for (int i = 0; i < Bone::JOINT_COUNT; i++)
		delete data[i].position;
}

void jester::DataFusionModule::setSkeletonFromJoints(SceneGraphNode *positionParent, JointFusionData joints[Bone::JOINT_COUNT]) {

	for (unsigned int i = 0; i < Bone::BONE_COUNT; i++) {
		Bone::BoneId id = Bone::intToBoneId(i);
		std::pair<Bone::JointId, Bone::JointId> jointIds = Bone::BoneToJointsMap.find(id)->second;
		glm::vec3 *startPos = joints[jointIds.first].position;
		glm::vec3 *endPos = joints[jointIds.second].position;
		float confidence = std::min(joints[jointIds.first].confidence, joints[jointIds.second].confidence);
		FusionBone *bone = kBones.find(id)->second;


		if (startPos == NULL && endPos == NULL) {
			setBoneDataFromEndpoints(bone->getParent(), bone->getParent(), bone,
					bone->getDefaultJointPositions().first + glm::vec3(0, 0, ((Bone*) bone->getParent())->getLength()),
					bone->getDefaultJointPositions().second + glm::vec3(0, 0, ((Bone*) bone->getParent())->getLength()), 0);
		} else if (startPos == NULL) {
			setBoneDataFromEndpoints(bone->getParent(), positionParent, bone,
					bone->getDefaultJointPositions().first + glm::vec3(0, 0, ((Bone*) bone->getParent())->getLength()),
					*endPos, 0);
		} else if (endPos == NULL) {
			setBoneDataFromEndpoints(positionParent, bone, bone,
					*startPos,
					bone->getDefaultJointPositions().second + glm::vec3(0, 0, ((Bone*) bone->getParent())->getLength()), 0);
		} else {
			setBoneDataFromEndpoints(positionParent, positionParent, bone, *startPos, *endPos, confidence);
		}
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

jester::DataFusionModule::~DataFusionModule() {}
