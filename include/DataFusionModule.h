#ifndef _DataFusionModule_h_
#define _DataFusionModule_h_

#define GLM_FORCE_RADIANS

#include <map>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cfloat>
#include <vector>

#include "Bone.h"
#include "Sensor.h"
#include "FusionBone.h"
#include "Scene.h"
#include "Filter.h"

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
	class FilterFactory;
	class Filter;

	class DataFusionModule {
	public:
		virtual void newData(Sensor *sensor, std::map<Bone::BoneId, BoneFusionData> data) = 0;
		virtual void newData(Sensor *sensor, std::map<Bone::JointId, JointFusionData> data) = 0;
		virtual void setDefaultSkeleton();
		void setSkeletonBones(FusionBone *bones[Bone::JOINT_COUNT]);
		void setSceneRoot(Scene *root);

		static void setBoneDataFromEndpoints(FusionBone *bone, glm::vec3 startPos, glm::vec3 endPos, float confidence);
		static glm::quat getQuaternionFromEndpoints(glm::vec3 startPos, glm::vec3 endPos);
		static glm::vec3 getEndpointFromBoneData(const BoneFusionData bone);
		static void setBoneWithWorldSpaceBone(FusionBone *set, FusionBone worldSpace);

		DataFusionModule(FilterFactory *filterFactory);
		virtual ~DataFusionModule();
	protected:
		FusionBone **kBones;
		FusionBone **kDefaultBones;
		Scene *kSceneRoot;
		FilterFactory *kFilterFactory;

		std::map<Bone::BoneId, BoneFusionData> jointDataToBoneData(SceneGraphNode *positionParent,
				const std::map<Bone::JointId, JointFusionData> joints);
		std::map<Bone::BoneId, FusionBone> jointsToParentSpaceBones(SceneGraphNode *positionParent,
				const std::map<Bone::JointId, JointFusionData> joints);
		std::map<Bone::BoneId, FusionBone> boneDataToWorldSpaceBones(const std::map<Bone::BoneId, BoneFusionData> bones);
		void setSkeletonFromBoneData(const std::map<Bone::BoneId, BoneFusionData> data);
		void setSkeletonFromWorldSpaceBones(FusionBone *skeleton[Bone::BONE_COUNT],
				const std::map<Bone::BoneId, FusionBone> bones);
	};

	class DataFusionModuleFactory {
	public:
		virtual DataFusionModule* createFusionModule() = 0;

		DataFusionModuleFactory(FilterFactory *factory);
		virtual ~DataFusionModuleFactory();

	protected:
		FilterFactory *kFilterFactory;
	};
};

#endif
