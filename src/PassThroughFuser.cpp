#include "PassThroughFuser.h"

void jester::PassThroughFuser::newPosition(Bone::BoneId id, Sensor *sensor, glm::vec3 position) {
	kBones.find(id)->second->setPosition(position);
}

void jester::PassThroughFuser::newPosition(Bone::BoneId id, float confidence, glm::vec3 position) {
	kBones.find(id)->second->setPosition(position);
}

jester::PassThroughFuser::PassThroughFuser() : DataFusionModule() {

}