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
	kSkeleton[Bone::SKULL] = new FusionBone(kSkeleton[Bone::NECK], Bone::SKULL);
	kSkeleton[Bone::SHOULDER_L] = new FusionBone(kSkeleton[Bone::TORSO], Bone::SHOULDER_L);
	kSkeleton[Bone::SHOULDER_R] = new FusionBone(kSkeleton[Bone::TORSO], Bone::SHOULDER_R);
	kSkeleton[Bone::HIP_L] = new FusionBone(kSkeleton[Bone::TORSO], Bone::HIP_L);
	kSkeleton[Bone::HIP_R] = new FusionBone(kSkeleton[Bone::TORSO], Bone::HIP_R);

	//extremity chains
	kSkeleton[Bone::HUMERUS_L] = new FusionBone(kSkeleton[Bone::SHOULDER_L], Bone::HUMERUS_L);
	kSkeleton[Bone::RADIUS_L] = new FusionBone(kSkeleton[Bone::HUMERUS_L], Bone::RADIUS_L);

	kSkeleton[Bone::HUMERUS_R] = new FusionBone(kSkeleton[Bone::SHOULDER_R], Bone::HUMERUS_R);
	kSkeleton[Bone::RADIUS_R] = new FusionBone(kSkeleton[Bone::HUMERUS_R], Bone::RADIUS_R);

	kSkeleton[Bone::FEMUR_L] = new FusionBone(kSkeleton[Bone::HIP_L], Bone::FEMUR_L);
	kSkeleton[Bone::TIBIA_L] = new FusionBone(kSkeleton[Bone::FEMUR_L], Bone::TIBIA_L);

	kSkeleton[Bone::FEMUR_R] = new FusionBone(kSkeleton[Bone::HIP_R], Bone::FEMUR_R);
	kSkeleton[Bone::TIBIA_R] = new FusionBone(kSkeleton[Bone::FEMUR_R], Bone::TIBIA_R);
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