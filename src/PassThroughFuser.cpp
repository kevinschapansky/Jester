#include "PassThroughFuser.h"

void jester::PassThroughFuser::newData(Sensor *sensor, std::vector<BoneFusionData *> data) {
	for (int i = 0; i < data.size(); i++) {
		FusionBone *bone = kBones.find(data[i].id);

		if (data[i].position != NULL) {
			bone->setPosition(SceneGraphNode::positionSpaceConversion(*(data[i].position), sensor, bone->getParent()), data[i].confidence);
		}
		if (data[i].orientation != NULL) {
			bone->setOrientation(SceneGraphNode::orientationSpaceConversion(*(data[i].orientation), sensor, bone->getParent()), data[i].confidence);
		}
	}
}

void jester::PassThroughFuser::newData(Sensor *sensor, std::vector<JointFusionData *> data) {
	/*FusionBone *bone = kBones.find(id)->second;

	if (position != NULL) {
		bone->setPosition(*position, confidence);
	}
	if (orientation != NULL) {
		bone->setOrientation(*orientation, confidence);
	}*/
}

jester::PassThroughFuser::PassThroughFuser() : DataFusionModule() {

}