#include "DataFusionModule.h"

void jester::DataFusionModule::setDefaultSkeleton() {
	SceneGraphNode *sceneRoot = kBones.find(Bone::ROOT)->second->getParent();
	std::map<Bone::JointId, JointFusionData> data;

	for (int i = 0; i < Bone::JOINT_COUNT; i++) {
		JointFusionData nextJoint;
		nextJoint.id = Bone::intToJointId(i);
		nextJoint.confidence = 0;
		nextJoint.position = glm::vec3(Bone::DefaultPositions[i]);
		data.insert(std::pair<Bone::JointId, JointFusionData>(Bone::intToJointId(i), nextJoint));
	}

	setSkeletonFromWorldSpaceBones(jointsToWorldSpaceBones(sceneRoot, data));

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
}

std::map<jester::Bone::BoneId, jester::FusionBone> jester::DataFusionModule::jointsToWorldSpaceBones(SceneGraphNode *positionParent,
				std::map<Bone::JointId, JointFusionData> joints) {
	std::map<Bone::BoneId, FusionBone> bones;

	for (unsigned int i = 0; i < Bone::BONE_COUNT; i++) {
		Bone::BoneId id = Bone::intToBoneId(i);
		std::pair<Bone::JointId, Bone::JointId> jointIds = Bone::BoneToJointsMap.find(id)->second;
		bool hasStart = (joints.count(jointIds.first));
		bool hasEnd = (joints.count(jointIds.second));
		glm::vec3 startPos;
		glm::vec3 endPos;
		float confidence = FLT_MAX;
		FusionBone *currentBone = kBones.find(id)->second;
		FusionBone newBone = *currentBone;

		if (hasStart) {
			startPos = joints.find(jointIds.first)->second.position;
			confidence = std::min(confidence, joints.find(jointIds.first)->second.confidence);
		} else
			confidence = 0;

		if (hasEnd) {
			endPos = joints.find(jointIds.second)->second.position;
			confidence = std::min(confidence, joints.find(jointIds.second)->second.confidence);
		} else
			confidence = 0;

		if (!hasStart && !hasEnd) {
			setBoneDataFromEndpoints(currentBone->getParent(), currentBone->getParent(), &newBone,
					currentBone->getDefaultJointPositions().first + glm::vec3(0, 0, ((Bone*) currentBone->getParent())->getLength()),
					currentBone->getDefaultJointPositions().second + glm::vec3(0, 0, ((Bone*) currentBone->getParent())->getLength()), 0);
		} else if (!hasStart) {
			setBoneDataFromEndpoints(currentBone->getParent(), positionParent, &newBone,
					currentBone->getDefaultJointPositions().first + glm::vec3(0, 0, ((Bone*) currentBone->getParent())->getLength()),
					endPos, 0);
		} else if (!hasEnd) {
			setBoneDataFromEndpoints(positionParent, currentBone, &newBone,
					startPos,
					currentBone->getDefaultJointPositions().second + glm::vec3(0, 0, ((Bone*) currentBone->getParent())->getLength()), 0);
		} else {
			setBoneDataFromEndpoints(positionParent, positionParent, &newBone, startPos, endPos, confidence);
		}
		bones.insert(std::pair<Bone::BoneId, FusionBone>(id, newBone));
	}

	return bones;
}

void jester::DataFusionModule::setBoneDataFromEndpoints(SceneGraphNode *curStartParent, SceneGraphNode *curEndParent,
			FusionBone *bone, glm::vec3 startPos, glm::vec3 endPos, float confidence) {
	glm::vec3 worldStartPosition = glm::vec3(curStartParent->getWorldTransform() * glm::vec4(startPos, 1.f));
	glm::vec3 worldEndPosition = glm::vec3(curEndParent->getWorldTransform() * glm::vec4(endPos, 1.f));

	glm::vec3 boneVector = worldEndPosition - worldStartPosition;
	glm::vec3 parentVector(0, 0 , 1);
	glm::vec3 normvector = glm::normalize(boneVector);
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

	bone->setPosition(worldStartPosition, confidence);
	bone->setLength(glm::distance(worldStartPosition, worldEndPosition));
}

std::map<jester::Bone::BoneId, jester::FusionBone> jester::DataFusionModule::boneDataToWorldSpaceBones(std::map<Bone::BoneId, BoneFusionData> bones) {
	std::map<Bone::BoneId, FusionBone> worldBones;

	for (std::map<Bone::BoneId, BoneFusionData>::iterator it = bones.begin(); it != bones.end(); it++) {
		FusionBone *currentBone = kBones.find(it->first)->second;
		FusionBone newBone = *currentBone;

		newBone.setOrientation(it->second.orientation, it->second.confidence);
		newBone.setPosition(it->second.position, it->second.confidence);
		newBone.setLength(it->second.length);

		worldBones.insert(std::pair<Bone::BoneId, FusionBone>(newBone.getType(), newBone));
	}

	return worldBones;
}

std::map<jester::Bone::BoneId, jester::BoneFusionData> jester::DataFusionModule::jointDataToBoneData(SceneGraphNode *positionParent, std::map<Bone::JointId, JointFusionData> joints) {
	std::map<jester::Bone::BoneId, jester::BoneFusionData> boneMap;
	std::map<Bone::BoneId, FusionBone> bones = jointsToWorldSpaceBones(positionParent, joints);

	for (std::map<Bone::BoneId, FusionBone>::iterator it = bones.begin(); it != bones.end(); it++) {
		Bone curBone = it->second;

		BoneFusionData newData;
		newData.confidence = curBone.getConfidence();
		newData.id = curBone.getType();
		newData.length = curBone.getLength();
		newData.orientation = curBone.getOrientation();
		newData.position = curBone.getPosition();

		boneMap.insert(std::pair<Bone::BoneId, BoneFusionData>(newData.id, newData));
	}
	return boneMap;
}

void jester::DataFusionModule::setSkeletonBones(FusionBone *bones[Bone::JOINT_COUNT]) {
	for (int i = 0; i < Bone::BONE_COUNT; i++) {
		kBones.insert(std::pair<Bone::BoneId, FusionBone *>(bones[i]->getType(), bones[i]));
	}
}

void jester::DataFusionModule::setSkeletonFromBoneData(std::map<Bone::BoneId, BoneFusionData> data) {
	setSkeletonFromWorldSpaceBones(boneDataToWorldSpaceBones(data));
}

void jester::DataFusionModule::setSkeletonFromWorldSpaceBones(std::map<Bone::BoneId, FusionBone> bones) {
	for (int i = 0; i < Bone::BONE_COUNT; i++) {
		std::map<Bone::BoneId, FusionBone>::iterator curBone = bones.find(Bone::intToBoneId(i));

		if (curBone != bones.end()) {
			FusionBone *skeletonBone = kBones.find(Bone::intToBoneId(i))->second;
			glm::mat4 boneWorldTransform = skeletonBone->getParent()->getWorldTransform();
			glm::mat4 invBoneWorldTransform = glm::inverse(boneWorldTransform);

			skeletonBone->setPosition(glm::vec3(invBoneWorldTransform * glm::vec4(curBone->second.getPosition(), 1.0)),
					curBone->second.getConfidence());
			skeletonBone->setOrientation(glm::quat_cast(invBoneWorldTransform * glm::mat4_cast(curBone->second.getOrientation())),
					curBone->second.getConfidence());
			skeletonBone->setLength(curBone->second.getLength());
		}
	}
}

void jester::DataFusionModule::setSceneRoot(SceneGraphNode *root) {
	kSceneRoot = root;
}

jester::DataFusionModule::~DataFusionModule() {}

jester::DataFusionModuleFactory::~DataFusionModuleFactory() {}
