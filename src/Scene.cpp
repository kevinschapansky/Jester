#include "Scene.h"

jester::Bone* jester::Scene::getBone(Bone::BoneId bone) {
	if (bone < Bone::BONE_COUNT && bone >= 0)
		return kSkeleton[bone];
	return NULL;
}

jester::Bone* jester::Scene::getRootBone() {
	return kSkeleton[Bone::ROOT];
}

void jester::Scene::buildSkeleton() {
	//root bone
	kSkeleton[Bone::ROOT] = new FusionBone(this, Bone::ROOT);

	//torso/hip chains
	kSkeleton[Bone::PELVIS_L] = new FusionBone(kSkeleton[Bone::ROOT], Bone::PELVIS_L);
	kSkeleton[Bone::PELVIS_R] = new FusionBone(kSkeleton[Bone::ROOT], Bone::PELVIS_R);

	kSkeleton[Bone::SPINE] = new FusionBone(kSkeleton[Bone::ROOT], Bone::SPINE);
	kSkeleton[Bone::COLLAR_L] = new FusionBone(kSkeleton[Bone::SPINE], Bone::COLLAR_L);
	kSkeleton[Bone::COLLAR_R] = new FusionBone(kSkeleton[Bone::SPINE], Bone::COLLAR_R);
	kSkeleton[Bone::NECK] = new FusionBone(kSkeleton[Bone::SPINE], Bone::NECK);

	kSkeleton[Bone::SKULL] = new FusionBone(kSkeleton[Bone::NECK], Bone::SKULL);

	//extremeity chains
	kSkeleton[Bone::FEMUR_L] = new FusionBone(kSkeleton[Bone::PELVIS_L], Bone::FEMUR_L);
	kSkeleton[Bone::FEMUR_R] = new FusionBone(kSkeleton[Bone::PELVIS_R], Bone::FEMUR_R);

	kSkeleton[Bone::TIBIA_L] = new FusionBone(kSkeleton[Bone::FEMUR_L], Bone::TIBIA_L);
	kSkeleton[Bone::TIBIA_R] = new FusionBone(kSkeleton[Bone::FEMUR_R], Bone::TIBIA_R);

	kSkeleton[Bone::HUMERUS_L] = new FusionBone(kSkeleton[Bone::COLLAR_L], Bone::HUMERUS_L);
	kSkeleton[Bone::HUMERUS_R] = new FusionBone(kSkeleton[Bone::COLLAR_R], Bone::HUMERUS_R);

	kSkeleton[Bone::RADIUS_L] = new FusionBone(kSkeleton[Bone::HUMERUS_L], Bone::RADIUS_L);
	kSkeleton[Bone::RADIUS_R] = new FusionBone(kSkeleton[Bone::HUMERUS_R], Bone::RADIUS_R);
}

jester::Scene::Scene(DataFusionModule *fuser) : SceneGraphNode(NULL) {
	kOrientation = glm::quat_cast(glm::mat4(1));
	kPosition = glm::vec3(0);

	buildSkeleton();

	fuser->setSkeleton(kSkeleton);
}

jester::Scene::~Scene() {
	for (int i = 0; i < Bone::BONE_COUNT; i++)
		delete kSkeleton[i];
}