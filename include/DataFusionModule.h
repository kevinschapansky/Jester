#ifndef _DataFusionModule_h_
#define _DataFusionModule_h_

#define GLM_FORCE_RADIANS

#include <map>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Bone.h"
#include "Sensor.h"
#include "FusionBone.h"

namespace jester {
	class DataFusionModule {
	public:
		enum FusionAlgorithm {
			PASS_THROUGH
		} ;
		virtual void newData(Bone::BoneId id, Sensor *sensor, float confidence, glm::vec3 *position, glm::quat *orientation) = 0;
		void addBone(FusionBone *bone);
	protected:
		std::map<Bone::BoneId, FusionBone *> kBones;
	};
};

#endif