#include "Bone.h"

void jester::Bone::suggestPosition() {

}

jester::Bone::BoneId jester::Bone::getType() {
	return kType;
}

jester::Bone::Bone(SceneGraphNode *parent, BoneId type) : SceneGraphNode(parent) {
	kType = type;
}

jester::Bone::~Bone() {
	
}