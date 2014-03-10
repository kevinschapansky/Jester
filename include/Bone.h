#ifndef _Bone_h_
#define _Bone_h_

#include "SceneGraphNode.h"
#include "Sensor.h"

#include <glm/vec3.hpp>

namespace jester {
	class Bone : public SceneGraphNode {
	public:
		enum JointId {
			HEAD = 0,
			C7,
			SHOULDER_L,
			SHOULDER_R,
			ELBOW_L,
			ELBOW_R,
			WRIST_L,
			WRIST_R,
			HIP_L,
			HIP_R,
			KNEE_L,
			KNEE_R,
			ANKLE_L,
			ANKLE_R,
			JOINT_COUNT
		};

		enum BoneId {
		 SKULL = 0,
		 NECK,
		 COLLAR_L,
		 COLLAR_R,
		 HUMERUS_L,
		 HUMERUS_R,
		 RADIUS_L,
		 RADIUS_R,
		 SPINE,
		 ROOT,
		 PELVIS_L, 
		 PELVIS_R,
		 FEMUR_L,
		 FEMUR_R,
		 TIBIA_L,
		 TIBIA_R,
		 BONE_COUNT
		};

		static const glm::vec3 DefaultPositions[BoneId::BONE_COUNT];

		BoneId getType();
		float getConfidence();

		static BoneId intToBoneId(int bone) {
			return static_cast<BoneId>(bone);
		}

		Bone(SceneGraphNode *parent, BoneId type);
		virtual ~Bone();
	private:
		BoneId kType;
	protected:
		float kConfidence;
		float kLength;
	};
};

#endif