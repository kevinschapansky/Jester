#ifndef _Bone_h_
#define _Bone_h_

#include "SceneGraphNode.h"
#include <glm/vec3.hpp>

namespace jester {
	class Bone : public SceneGraphNode {
	public:
		enum BoneId {
		 HEAD = 0,
		 NECK,
		 SHOULDER_L,
		 SHOULDER_R,
		 ELBOW_L,
		 ELBOW_R,
		 HAND_L,
		 HAND_R,
		 TORSO,
		 HIP_L, 
		 HIP_R,
		 KNEE_L,
		 KNEE_R,
		 FOOT_L,
		 FOOT_R,
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