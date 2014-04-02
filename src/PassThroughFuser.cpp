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
	setSkeletonFromJoints(sensor, data);
}

jester::PassThroughFuser::PassThroughFuser() : DataFusionModule() {

}
