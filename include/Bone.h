#ifndef _Bone_h_
#define _Bone_h_

#include "SceneGraphNode.h"
#include "Sensor.h"

#include <glm/vec3.hpp>
#include <map>
#include <mutex>

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
			DISTAL_L_1,
			DISTAL_L_2,
			DISTAL_L_3,
			DISTAL_L_4,
			DISTAL_L_5,
			DISTAL_R_1,
			DISTAL_R_2,
			DISTAL_R_3,
			DISTAL_R_4,
			DISTAL_R_5,
			METACARPO_L_1,
			METACARPO_L_2,
			METACARPO_L_3,
			METACARPO_L_4,
			METACARPO_L_5,
			METACARPO_R_1,
			METACARPO_R_2,
			METACARPO_R_3,
			METACARPO_R_4,
			METACARPO_R_5,
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
		 PHALANX_L_1,
		 PHALANX_L_2,
		 PHALANX_L_3,
		 PHALANX_L_4,
		 PHALANX_L_5,
		 PHALANX_R_1,
		 PHALANX_R_2,
		 PHALANX_R_3,
		 PHALANX_R_4,
		 PHALANX_R_5,
		 BONE_COUNT
		};

		static const glm::vec3 DefaultPositions[JointId::JOINT_COUNT];
		static const std::map<Bone::BoneId, std::pair<Bone::JointId, Bone::JointId>> BoneToJointsMap;
		static const float SkullWidth;
		static const float PhalanxWidth;
		static const float RootWidth;
		static const float DefaultWidth;
		static const float DefaultLength;
		static const float DefaultConfidence;

		BoneId getType();
		float getConfidence();
		float getLength();
		float getWidth();
		std::pair<glm::vec3, glm::vec3> getDefaultJointPositions();

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
		float kWidth;
		std::pair<glm::vec3, glm::vec3> *kDefaultJointPositions;
	};
};

#endif
