#include "DataFusionModule.h"

void jester::DataFusionModule::setSkeleton(FusionBone *bones[Bone::BONE_COUNT]) {
	for (int i = 0; i < Bone::BONE_COUNT; i++) {
		kBones.insert(std::pair<Bone::BoneId, FusionBone *>(bones[i]->getType(), bones[i]));
	}
}

void jester::DataFusionModule::setDefaultSkeleton() {

}

void jester::DataFusionModule::setBoneDataFromEndpoints(SceneGraphNode *curStartParent, SceneGraphNode *curEndParent,
			FusionBone *bone, glm::vec3 startPos, glm::vec3 endPos, float confidence) {
	glm::vec3 worldStartPosition = curStartParent->getWorldTransform() * glm::vec4(startPos, 1);
	glm::vec3 worldEndPosition = curEndParent->getWorldTransform() * glm::vec4(startPos, 1);
	glm::mat4 parentTransform = bone->getParent()->getWorldTransform();
	glm::mat4 invParentWorldTransform = glm::inverse(parentTransform);

	glm::vec3 parentSpaceBoneVector = invParentTransform * glm::vec4(worldEndPosition - worldStartPosition, 0);

	bone->setPosition(invParentWorldTransform * glm::vec4(worldStartPosition, 0), confidence);
}