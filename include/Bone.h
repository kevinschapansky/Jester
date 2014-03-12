#ifndef _Bone_h_
#define _Bone_h_

#include "SceneGraphNode.h"
#include "Sensor.h"

#include <glm/vec3.hpp>
#include <map>

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
			PELVIS_MIDPOINT,
			HIP_L,
			HIP_R,
			KNEE_L,
			KNEE_R,
			ANKLE_L,
			ANKLE_R,
			EXTEND_UP,
			EXTEND_DOWN,
			JOINT_COUNT
		};

		enum BoneId {
		 ROOT = 0,
		 SPINE,
		 NECK,
		 SKULL,
		 COLLAR_L,
		 COLLAR_R,
		 HUMERUS_L,
		 HUMERUS_R,
		 RADIUS_L,
		 RADIUS_R,
		 PELVIS_L, 
		 PELVIS_R,
		 FEMUR_L,
		 FEMUR_R,
		 TIBIA_L,
		 TIBIA_R,
		 BONE_COUNT
		};

		static const glm::vec3 DefaultPositions[JointId::JOINT_COUNT];
		static const std::map<Bone::BoneId, std::pair<Bone::JointId, Bone::JointId>> JointToBoneMapping;

		BoneId getType();
		float getConfidence();
		float getLength();

		static BoneId intToBoneId(int bone) {
			return static_cast<BoneId>(bone);
		}

		static JointId intToJointId(int joint) {
			return static_cast<JointId>(joint);
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