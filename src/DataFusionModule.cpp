#include "DataFusionModule.h"

void jester::DataFusionModule::setDefaultSkeleton() {
	SceneGraphNode *sceneRoot = kBones[Bone::ROOT]->getParent();
	std::map<Bone::JointId, JointFusionData> data;

	for (int i = 0; i < Bone::JOINT_COUNT; i++) {
		JointFusionData nextJoint;
		nextJoint.id = Bone::intToJointId(i);
		nextJoint.confidence = 0;
		nextJoint.position = glm::vec3(Bone::DefaultPositions[i]);
		data.insert(std::pair<Bone::JointId, JointFusionData>(Bone::intToJointId(i), nextJoint));
	}

	kDefaultBones = kSceneRoot->buildSkeleton();

	std::map<Bone::BoneId, FusionBone> bones = jointsToParentSpaceBones(sceneRoot, data);
	setSkeletonFromWorldSpaceBones(kDefaultBones, bones);
	setSkeletonFromWorldSpaceBones(kBones, bones);

	//populate the default endpoints for when bones are set with unknown joint positions
	for (int i = 0; i < Bone::BONE_COUNT; i++) {
		FusionBone *bone = kBones[Bone::intToBoneId(i)];
		std::pair<Bone::JointId, Bone::JointId> jointIds = Bone::BoneToJointsMap.find(Bone::intToBoneId(i))->second;
		glm::mat4 invParentTransform = glm::inverse(bone->getParent()->getWorldTransform());
		glm::vec3 parentSpaceStart = bone->getPosition();
		glm::vec3 parentSpaceEnd = glm::vec3(invParentTransform * glm::vec4(Bone::DefaultPositions[jointIds.second], 1));
		glm::vec3 parentBoneEndpoint = glm::vec3(0, 0, ((Bone*) bone->getParent())->getLength());

		bone->setDefaultEndpoints(parentSpaceStart - parentBoneEndpoint, parentSpaceEnd - parentBoneEndpoint);
	}
}

std::map<jester::Bone::BoneId, jester::FusionBone> jester::DataFusionModule::jointsToParentSpaceBones(SceneGraphNode *positionParent,
				const std::map<Bone::JointId, JointFusionData> joints) {
	std::map<Bone::BoneId, FusionBone> bones;

	for (unsigned int i = 0; i < Bone::BONE_COUNT; i++) {
		Bone::BoneId id = Bone::intToBoneId(i);
		std::pair<Bone::JointId, Bone::JointId> jointIds = Bone::BoneToJointsMap.find(id)->second;
		bool hasStart = (joints.count(jointIds.first));
		bool hasEnd = (joints.count(jointIds.second));
		glm::vec3 startPos;
		glm::vec3 endPos;
		float confidence = FLT_MAX;
		FusionBone *currentBone = kBones[id];
		FusionBone *defaultBone = kDefaultBones[id];
		FusionBone newBone = *currentBone;

		if (hasStart) {
			startPos = joints.find(jointIds.first)->second.position;
			confidence = std::min(confidence, joints.find(jointIds.first)->second.confidence);
		} else {
			confidence = 0;
			startPos = glm::vec3(glm::inverse(positionParent->getWorldTransform()) * glm::vec4(defaultBone->getWorldPosition(), 1.0));
		}

		if (hasEnd) {
			endPos = joints.find(jointIds.second)->second.position;
			confidence = std::min(confidence, joints.find(jointIds.second)->second.confidence);
		} else {
			continue;
		}

		setBoneDataFromEndpoints(&newBone, startPos, endPos, confidence);

		bones.insert(std::pair<Bone::BoneId, FusionBone>(id, newBone));
	}

	return bones;
}

void jester::DataFusionModule::setBoneDataFromEndpoints(FusionBone *bone, glm::vec3 startPos, glm::vec3 endPos, float confidence) {
	glm::vec3 boneVector = endPos - startPos;
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

	bone->setPosition(startPos, confidence);
	bone->setLength(glm::distance(startPos, endPos));
}

glm::quat jester::DataFusionModule::getQuaternionFromEndpoints(glm::vec3 startPos, glm::vec3 endPos) {
	glm::quat newQuat;
	glm::vec3 boneVector = endPos - startPos;
	glm::vec3 parentVector(0, 0 , 1);
	glm::vec3 normvector = glm::normalize(boneVector);
	float vectorDot = glm::dot(parentVector, normvector);
	float angle = glm::acos(vectorDot);

	if (vectorDot > 0.9999) {
		newQuat = glm::angleAxis(0.f, glm::vec3(0, 0, 1));
	} else if (vectorDot < -0.9999) {
		newQuat = glm::angleAxis(3.14f, glm::vec3(0, 0, 1));
	} else {
		glm::vec3 axis = glm::normalize(glm::cross(parentVector, normvector));
		newQuat = glm::angleAxis(angle, axis);
	}
	return newQuat;
}

glm::vec3 jester::DataFusionModule::getEndpointFromBoneData(const BoneFusionData bone) {
	return bone.position + glm::normalize(glm::vec3(glm::rotate(bone.orientation, glm::vec4(0, 0, 1, 0)))) * bone.length;
}

std::map<jester::Bone::BoneId, jester::FusionBone> jester::DataFusionModule::boneDataToWorldSpaceBones(const std::map<Bone::BoneId, BoneFusionData> bones) {
	std::map<Bone::BoneId, FusionBone> worldBones;

	for (std::map<Bone::BoneId, BoneFusionData>::const_iterator it = bones.begin(); it != bones.end(); it++) {
		FusionBone *currentBone = kBones[it->first];
		FusionBone newBone = *currentBone;

		newBone.setOrientation(it->second.orientation, it->second.confidence);
		newBone.setPosition(it->second.position, it->second.confidence);
		newBone.setLength(it->second.length);

		worldBones.insert(std::pair<Bone::BoneId, FusionBone>(newBone.getType(), newBone));
	}
	return worldBones;
}

std::map<jester::Bone::BoneId, jester::BoneFusionData> jester::DataFusionModule::jointDataToBoneData(SceneGraphNode *positionParent,
		const std::map<Bone::JointId, JointFusionData> joints) {
	std::map<jester::Bone::BoneId, jester::BoneFusionData> boneMap;
	std::map<Bone::BoneId, FusionBone> bones = jointsToParentSpaceBones(positionParent, joints);

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

void jester::DataFusionModule::setSkeletonBones(FusionBone **bones) {
	kBones = bones;
}

void jester::DataFusionModule::setSkeletonFromBoneData(const std::map<Bone::BoneId, BoneFusionData> data) {
	setSkeletonFromWorldSpaceBones(kBones, boneDataToWorldSpaceBones(data));
}

void jester::DataFusionModule::setSkeletonFromWorldSpaceBones(FusionBone *skeleton[Bone::BONE_COUNT],
		const std::map<Bone::BoneId, FusionBone> bones) {

	for (int i = 0; i < Bone::BONE_COUNT; i++) {
		std::map<Bone::BoneId, FusionBone>::const_iterator curBoneIt = bones.find(Bone::intToBoneId(i));

		if (curBoneIt == bones.end()) {
			skeleton[i]->setOrientation(kDefaultBones[i]->getOrientation(), 0.0);
			skeleton[i]->setLength(kDefaultBones[i]->getLength());

			if (Bone::intToBoneId(i) == Bone::ROOT) {
				skeleton[i]->setPosition(kDefaultBones[i]->getPosition(), 0.0);
			} else {
				FusionBone *defaultParent = (FusionBone*) kDefaultBones[i]->getParent();
				FusionBone *parent = (FusionBone*) skeleton[i]->getParent();
				float endOffset = parent->getLength() - defaultParent->getLength();

				skeleton[i]->setPosition(kDefaultBones[i]->getPosition() + glm::vec3(0, 0, endOffset), 0.0);
			}
		} else {
			setBoneWithWorldSpaceBone(skeleton[Bone::intToBoneId(i)], curBoneIt->second);
		}
	}
}

void jester::DataFusionModule::setBoneWithWorldSpaceBone(FusionBone *set, FusionBone worldSpace) {
	glm::mat4 boneWorldTransform = set->getParent()->getWorldTransform();
	glm::mat4 invBoneWorldTransform = glm::inverse(boneWorldTransform);

	set->setPosition(glm::vec3(invBoneWorldTransform * glm::vec4(worldSpace.getPosition(), 1.0)),
			worldSpace.getConfidence());
	set->setOrientation(glm::quat_cast(invBoneWorldTransform * glm::mat4_cast(worldSpace.getOrientation())),
			worldSpace.getConfidence());
	set->setLength(worldSpace.getLength());
}

void jester::DataFusionModule::setSceneRoot(Scene *root) {
	kSceneRoot = root;
}

jester::DataFusionModule::DataFusionModule(FilterFactory *filterFactory) {
	kFilterFactory = filterFactory;
	kBones = NULL;
	kDefaultBones = NULL;
	kSceneRoot = NULL;
}

jester::DataFusionModule::~DataFusionModule() {

}

jester::DataFusionModuleFactory::DataFusionModuleFactory(FilterFactory *factory) {
	kFilterFactory = factory;
}

jester::DataFusionModuleFactory::~DataFusionModuleFactory() {}
