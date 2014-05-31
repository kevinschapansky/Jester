#ifndef _DataFusionModule_h_
#define _DataFusionModule_h_

#define GLM_FORCE_RADIANS

#include <map>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cfloat>
#include <vector>

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
		void setSceneRoot(SceneGraphNode *root);

		virtual ~DataFusionModule();
	protected:
		std::map<Bone::BoneId, FusionBone *> kBones;
		SceneGraphNode *kSceneRoot;

		virtual void setBoneDataFromEndpoints(SceneGraphNode *curStartParent, SceneGraphNode *curEndParent,
			FusionBone *bone, glm::vec3 startPos, glm::vec3 endPos, float confidence);
		virtual std::map<Bone::BoneId, BoneFusionData> jointDataToBoneData(SceneGraphNode *positionParent,
				std::map<Bone::JointId, JointFusionData> joints);
		virtual std::map<Bone::BoneId, FusionBone> jointsToWorldSpaceBones(SceneGraphNode *positionParent,
				std::map<Bone::JointId, JointFusionData> joints);
		virtual std::map<Bone::BoneId, FusionBone> boneDataToWorldSpaceBones(std::map<Bone::BoneId, BoneFusionData> bones);
		virtual void setSkeletonFromBoneData(std::map<Bone::BoneId, BoneFusionData> data);
		virtual void setSkeletonFromWorldSpaceBones(std::map<Bone::BoneId, FusionBone> bones);
	};

	class DataFusionModuleFactory {
	public:
		virtual DataFusionModule* CreateFusionModule() = 0;

		virtual ~DataFusionModuleFactory();
	};
};

#endif
