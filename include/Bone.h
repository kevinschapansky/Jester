#ifndef _Bone_h_
#define _Bone_h_

#include "SceneGraphNode.h"
#include <glm/vec3.hpp>

namespace jester {
	class Bone : public SceneGraphNode {
	public:
		enum BoneId {
		 SKULL = 0,
		 NECK,
		 SHOULDER_L,
		 SHOULDER_R,
		 HUMERUS_L,
		 HUMERUS_R,
		 RADIUS_L,
		 RADIUS_R,
		 TORSO,
		 HIP_L, 
		 HIP_R,
		 FEMUR_L,
		 FEMUR_R,
		 TIBIA_L,
		 TIBIA_R,
		 BONE_COUNT
		};

		static const glm::vec3 DefaultPositions[BoneId::BONE_COUNT];

		glm::vec3 getWorldPosition();
		glm::quat getWorldOrientation();
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
	};
};

#endif