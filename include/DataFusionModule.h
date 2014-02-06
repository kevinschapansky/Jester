#ifndef _DataFusionModule_h_
#define _DataFusionModule_h_

#include <map>
#include <glm/vec3.hpp>

#include "Bone.h"
#include "Sensor.h"
#include "FusionBone.h"

namespace jester {
	class DataFusionModule {
	public:
		enum FusionAlgorithm {
			PASS_THROUGH
		} ;
		virtual void newPosition(Bone::BoneId id, Sensor *sensor, glm::vec3 position) = 0;
		void addBone(FusionBone *bone);
	protected:
		std::map<Bone::BoneId, FusionBone *> kBones;
	};
};

#endif