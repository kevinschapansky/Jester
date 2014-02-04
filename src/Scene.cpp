#include <glm/mat4x4.hpp>

#include "Scene.h"

jester::JesterTransform jester::Scene::getWorldTransform() {
	return JesterTransform(glm::mat4x4(1));
}

jester::Bone* jester::Scene::getBone(Bone::BoneId bone) {
	if (bone < Bone::BONE_COUNT && bone >= 0)
		return kSkeleton[bone];
	return NULL;
}

void jester::Scene::buildSkeleton() {
	//root bone
	kSkeleton[Bone::TORSO] = new Bone(this, Bone::TORSO);

	//direct torso children
	kSkeleton[Bone::NECK] = new Bone(kSkeleton[Bone::TORSO], Bone::NECK);
	kSkeleton[Bone::HEAD] = new Bone(kSkeleton[Bone::NECK], Bone::HEAD);
	kSkeleton[Bone::SHOULDER_L] = new Bone(kSkeleton[Bone::TORSO], Bone::SHOULDER_L);
	kSkeleton[Bone::SHOULDER_R] = new Bone(kSkeleton[Bone::TORSO], Bone::SHOULDER_R);
	kSkeleton[Bone::HIP_L] = new Bone(kSkeleton[Bone::TORSO], Bone::HIP_L);
	kSkeleton[Bone::HIP_R] = new Bone(kSkeleton[Bone::TORSO], Bone::HIP_R);

	//extremity chains
	kSkeleton[Bone::ELBOW_L] = new Bone(kSkeleton[Bone::SHOULDER_L], Bone::ELBOW_L);
	kSkeleton[Bone::HAND_L] = new Bone(kSkeleton[Bone::ELBOW_L], Bone::HAND_L);

	kSkeleton[Bone::ELBOW_R] = new Bone(kSkeleton[Bone::SHOULDER_R], Bone::ELBOW_R);
	kSkeleton[Bone::HAND_R] = new Bone(kSkeleton[Bone::ELBOW_R], Bone::HAND_R);

	kSkeleton[Bone::KNEE_L] = new Bone(kSkeleton[Bone::HIP_L], Bone::KNEE_L);
	kSkeleton[Bone::FOOT_L] = new Bone(kSkeleton[Bone::KNEE_L], Bone::FOOT_L);

	kSkeleton[Bone::KNEE_R] = new Bone(kSkeleton[Bone::HIP_R], Bone::KNEE_R);
	kSkeleton[Bone::FOOT_R] = new Bone(kSkeleton[Bone::KNEE_R], Bone::FOOT_R);
}

jester::Scene::Scene() : SceneGraphNode(NULL) {
	buildSkeleton();
}

jester::Scene::~Scene() {

}