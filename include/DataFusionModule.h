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
#include "Scene.h"

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

	class Scene;

	class DataFusionModule {
	public:
		virtual void newData(Sensor *sensor, std::map<Bone::BoneId, BoneFusionData> data) = 0;
		virtual void newData(Sensor *sensor, std::map<Bone::JointId, JointFusionData> data) = 0;
		virtual void setDefaultSkeleton();
		void setSkeletonBones(FusionBone *bones[Bone::JOINT_COUNT]);
		void setSceneRoot(Scene *root);

		virtual ~DataFusionModule();
	protected:
		FusionBone **kBones;
		FusionBone **kDefaultBones;
		Scene *kSceneRoot;

		virtual void setBoneDataFromEndpoints(FusionBone *bone, glm::vec3 startPos, glm::vec3 endPos, float confidence);
		virtual glm::quat getQuaternionFromEndpoints(glm::vec3 startPos, glm::vec3 endPos);
		virtual std::map<Bone::BoneId, BoneFusionData> jointDataToBoneData(SceneGraphNode *positionParent,
				const std::map<Bone::JointId, JointFusionData> joints);
		virtual std::map<Bone::BoneId, FusionBone> jointsToParentSpaceBones(SceneGraphNode *positionParent,
				const std::map<Bone::JointId, JointFusionData> joints);
		virtual std::map<Bone::BoneId, FusionBone> boneDataToWorldSpaceBones(const std::map<Bone::BoneId, BoneFusionData> bones);
		virtual void setSkeletonFromBoneData(const std::map<Bone::BoneId, BoneFusionData> data);
		virtual void setSkeletonFromWorldSpaceBones(FusionBone *skeleton[Bone::BONE_COUNT],
				const std::map<Bone::BoneId, FusionBone> bones);
		virtual void setBoneWithWorldSpaceBone(FusionBone *set, FusionBone worldSpace);
	};

	class DataFusionModuleFactory {
	public:
		virtual DataFusionModule* CreateFusionModule() = 0;

		virtual ~DataFusionModuleFactory();
	};
};

#endif
