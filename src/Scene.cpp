#include "Scene.h"

glm::vec3 jester::Scene::getWorldPosition() {
	return glm::vec3(0);
}
		
glm::quat jester::Scene::getWorldOrientation() {
	return glm::quat();
}

jester::Bone* jester::Scene::getBone(Bone::BoneId bone) {
	if (bone < Bone::BONE_COUNT && bone >= 0)
		return kSkeleton[bone];
	return NULL;
}

void jester::Scene::buildSkeleton() {
	//root bone
	kSkeleton[Bone::TORSO] = new FusionBone(this, Bone::TORSO);

	//direct torso children
	kSkeleton[Bone::NECK] = new FusionBone(kSkeleton[Bone::TORSO], Bone::NECK);
	kSkeleton[Bone::HEAD] = new FusionBone(kSkeleton[Bone::NECK], Bone::HEAD);
	kSkeleton[Bone::SHOULDER_L] = new FusionBone(kSkeleton[Bone::TORSO], Bone::SHOULDER_L);
	kSkeleton[Bone::SHOULDER_R] = new FusionBone(kSkeleton[Bone::TORSO], Bone::SHOULDER_R);
	kSkeleton[Bone::HIP_L] = new FusionBone(kSkeleton[Bone::TORSO], Bone::HIP_L);
	kSkeleton[Bone::HIP_R] = new FusionBone(kSkeleton[Bone::TORSO], Bone::HIP_R);

	//extremity chains
	kSkeleton[Bone::ELBOW_L] = new FusionBone(kSkeleton[Bone::SHOULDER_L], Bone::ELBOW_L);
	kSkeleton[Bone::HAND_L] = new FusionBone(kSkeleton[Bone::ELBOW_L], Bone::HAND_L);

	kSkeleton[Bone::ELBOW_R] = new FusionBone(kSkeleton[Bone::SHOULDER_R], Bone::ELBOW_R);
	kSkeleton[Bone::HAND_R] = new FusionBone(kSkeleton[Bone::ELBOW_R], Bone::HAND_R);

	kSkeleton[Bone::KNEE_L] = new FusionBone(kSkeleton[Bone::HIP_L], Bone::KNEE_L);
	kSkeleton[Bone::FOOT_L] = new FusionBone(kSkeleton[Bone::KNEE_L], Bone::FOOT_L);

	kSkeleton[Bone::KNEE_R] = new FusionBone(kSkeleton[Bone::HIP_R], Bone::KNEE_R);
	kSkeleton[Bone::FOOT_R] = new FusionBone(kSkeleton[Bone::KNEE_R], Bone::FOOT_R);
}

jester::Scene::Scene(DataFusionModule *fuser) : SceneGraphNode(NULL) {
	buildSkeleton();

	for (int i = 0; i < Bone::BONE_COUNT; i++)
		fuser->addBone(kSkeleton[i]);
}

jester::Scene::~Scene() {
	for (int i = 0; i < Bone::BONE_COUNT; i++)
		delete kSkeleton[i];
}