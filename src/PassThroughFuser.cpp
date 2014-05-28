#include "PassThroughFuser.h"

void jester::PassThroughFuser::newData(Sensor *sensor, std::map<Bone::BoneId, BoneFusionData> data) {
	for (unsigned int i = 0; i < Bone::BONE_COUNT; i++) {
		std::map<Bone::BoneId, BoneFusionData>::iterator it = data.find(Bone::intToBoneId(i));

		if (data.count(Bone::intToBoneId(i))) {
			FusionBone *bone = kBones.find(it->second.id)->second;
			bone->setPosition(SceneGraphNode::positionSpaceConversion(it->second.position, sensor, bone->getParent()), it->second.confidence);
			bone->setOrientation(SceneGraphNode::orientationSpaceConversion(it->second.orientation, sensor, bone->getParent()), it->second.confidence);
			bone->setLength(it->second.length);
		}
	}
}

void jester::PassThroughFuser::newData(Sensor *sensor, std::map<Bone::JointId, JointFusionData> data) {
	newData(sensor, jointDataToBoneData(sensor, data));
}

jester::PassThroughFuser::PassThroughFuser() : DataFusionModule() {

}
