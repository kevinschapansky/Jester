#include "Scene.h"

jester::Bone* jester::Scene::getBone(Bone::BoneId bone) {
	if (bone < Bone::BONE_COUNT && bone >= 0)
		return kSkeleton[bone];
	return NULL;
}

jester::Bone* jester::Scene::getRootBone() {
	return kSkeleton[Bone::ROOT];
}

jester::FusionBone** jester::Scene::buildSkeleton() {
	FusionBone **skeleton = new FusionBone*[Bone::BONE_COUNT];
	//root bone
	skeleton[Bone::ROOT] = new FusionBone(this, Bone::ROOT);

	//torso/hip chains
	skeleton[Bone::PELVIS_L] = new FusionBone(skeleton[Bone::ROOT], Bone::PELVIS_L);
	skeleton[Bone::PELVIS_R] = new FusionBone(skeleton[Bone::ROOT], Bone::PELVIS_R);

	skeleton[Bone::SPINE] = new FusionBone(skeleton[Bone::ROOT], Bone::SPINE);
	skeleton[Bone::COLLAR_L] = new FusionBone(skeleton[Bone::SPINE], Bone::COLLAR_L);
	skeleton[Bone::COLLAR_R] = new FusionBone(skeleton[Bone::SPINE], Bone::COLLAR_R);
	skeleton[Bone::NECK] = new FusionBone(skeleton[Bone::SPINE], Bone::NECK);

	skeleton[Bone::SKULL] = new FusionBone(skeleton[Bone::NECK], Bone::SKULL);

	//extremeity chains
	skeleton[Bone::FEMUR_L] = new FusionBone(skeleton[Bone::PELVIS_L], Bone::FEMUR_L);
	skeleton[Bone::FEMUR_R] = new FusionBone(skeleton[Bone::PELVIS_R], Bone::FEMUR_R);

	skeleton[Bone::TIBIA_L] = new FusionBone(skeleton[Bone::FEMUR_L], Bone::TIBIA_L);
	skeleton[Bone::TIBIA_R] = new FusionBone(skeleton[Bone::FEMUR_R], Bone::TIBIA_R);

	skeleton[Bone::HUMERUS_L] = new FusionBone(skeleton[Bone::COLLAR_L], Bone::HUMERUS_L);
	skeleton[Bone::HUMERUS_R] = new FusionBone(skeleton[Bone::COLLAR_R], Bone::HUMERUS_R);

	skeleton[Bone::RADIUS_L] = new FusionBone(skeleton[Bone::HUMERUS_L], Bone::RADIUS_L);
	skeleton[Bone::RADIUS_R] = new FusionBone(skeleton[Bone::HUMERUS_R], Bone::RADIUS_R);

	//fingers
	skeleton[Bone::PHALANX_L_1] = new FusionBone(skeleton[Bone::RADIUS_L], Bone::PHALANX_L_1);
	skeleton[Bone::PHALANX_L_2] = new FusionBone(skeleton[Bone::RADIUS_L], Bone::PHALANX_L_2);
	skeleton[Bone::PHALANX_L_3] = new FusionBone(skeleton[Bone::RADIUS_L], Bone::PHALANX_L_3);
	skeleton[Bone::PHALANX_L_4] = new FusionBone(skeleton[Bone::RADIUS_L], Bone::PHALANX_L_4);
	skeleton[Bone::PHALANX_L_5] = new FusionBone(skeleton[Bone::RADIUS_L], Bone::PHALANX_L_5);

	skeleton[Bone::PHALANX_R_1] = new FusionBone(skeleton[Bone::RADIUS_R], Bone::PHALANX_R_1);
	skeleton[Bone::PHALANX_R_2] = new FusionBone(skeleton[Bone::RADIUS_R], Bone::PHALANX_R_2);
	skeleton[Bone::PHALANX_R_3] = new FusionBone(skeleton[Bone::RADIUS_R], Bone::PHALANX_R_3);
	skeleton[Bone::PHALANX_R_4] = new FusionBone(skeleton[Bone::RADIUS_R], Bone::PHALANX_R_4);
	skeleton[Bone::PHALANX_R_5] = new FusionBone(skeleton[Bone::RADIUS_R], Bone::PHALANX_R_5);

	return skeleton;
}

jester::Scene::Scene(DataFusionModule *fuser) : SceneGraphNode(NULL) {
	kOrientation = glm::quat_cast(glm::mat4(1));
	kPosition = glm::vec3(0);

	kSkeleton = buildSkeleton();

	fuser->setSkeletonBones(kSkeleton);
}

jester::Scene::~Scene() {
	for (int i = 0; i < Bone::BONE_COUNT; i++)
		delete kSkeleton[i];
}
