#ifndef _DataFusionModule_h_
#define _DataFusionModule_h_

#define GLM_FORCE_RADIANS

#include <map>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cfloat>

#include "Bone.h"
#include "Sensor.h"
#include "FusionBone.h"

namespace jester {
	class BoneFusionData {
	public:
		Bone::BoneId id;
		float confidence;
		glm::vec3 position;
		glm::quat orientation;
		float length;
	};

	class JointFusionData {
	public:
		Bone::JointId id;
		float confidence;
		glm::vec3 position;
	};

	class DataFusionModule {
	public:
		virtual void newData(Sensor *sensor, std::map<Bone::BoneId, BoneFusionData> data) = 0;
		virtual void newData(Sensor *sensor, std::map<Bone::JointId, JointFusionData> data) = 0;
		virtual void setDefaultSkeleton();
		void setSkeletonBones(FusionBone *bones[Bone::JOINT_COUNT]);

		virtual ~DataFusionModule();
	protected:
		std::map<Bone::BoneId, FusionBone *> kBones;

		virtual void setBoneDataFromEndpoints(SceneGraphNode *curStartParent, SceneGraphNode *curEndParent,
			FusionBone *bone, glm::vec3 startPos, glm::vec3 endPos, float confidence);
		virtual void setSkeletonFromJoints(SceneGraphNode *positionParent, std::map<Bone::JointId, JointFusionData> joints);
	};
};

#endif
