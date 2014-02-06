#include "DataFusionModule.h"

void jester::DataFusionModule::addBone(FusionBone *bone) {
	kBones.insert(std::pair<Bone::BoneId, FusionBone *>(bone->getType(), bone));
}