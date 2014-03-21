#include "PassThroughFuser.h"

void jester::PassThroughFuser::newData(Sensor *sensor, BoneFusionData data[Bone::BONE_COUNT]) {
	for (unsigned int i = 0; i < Bone::BONE_COUNT; i++) {
		FusionBone *bone = kBones.find(data[i].id)->second;

		if (data[i].position != NULL) {
			bone->setPosition(SceneGraphNode::positionSpaceConversion(*(data[i].position), sensor, bone->getParent()), data[i].confidence);
		}
		if (data[i].orientation != NULL) {
			bone->setOrientation(SceneGraphNode::orientationSpaceConversion(*(data[i].orientation), sensor, bone->getParent()), data[i].confidence);
		}
		if (data[i].length > 0) {
			bone->setLength(data[i].length);
		}
	}
}

void jester::PassThroughFuser::newData(Sensor *sensor, JointFusionData data[Bone::JOINT_COUNT]) {
	std::vector<glm::vec3*> defaultBones;
	glm::mat4 invSensorTrans = glm::inverse(sensor->getWorldTransform());

	for (int i = 0; i < Bone::JOINT_COUNT; i++) {
		if (data[i].position == NULL) {
			glm::vec3 *defaultPosition = new glm::vec3(Bone::DefaultPositions[i]);
			*defaultPosition = glm::vec3(invSensorTrans * glm::vec4(*defaultPosition, 1));

			defaultBones.push_back(defaultPosition);
			data[i].position = defaultPosition;
		}
	}

	setSkeletonFromJoints(sensor, data);

	for (int i = 0; i < defaultBones.size(); i++)
		delete defaultBones[i];
}

jester::PassThroughFuser::PassThroughFuser() : DataFusionModule() {

}
