#include "PassThroughFuser.h"

void jester::PassThroughFuser::newData(Bone::BoneId id, Sensor *sensor, float confidence, glm::vec3 *position, glm::quat *orientation) {
	FusionBone *bone = kBones.find(id)->second;

	if (position != NULL) {
		bone->setPosition(*position, confidence);
	}
	if (orientation != NULL) {
		bone->setOrientation(*orientation, confidence);
	}
}

jester::PassThroughFuser::PassThroughFuser() : DataFusionModule() {

}