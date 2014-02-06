#ifndef _PassThroughFuser_h_
#define _PassThroughFuser_h_

#include "DataFusionModule.h"

namespace jester {
	class PassThroughFuser : public DataFusionModule {
	public:
		void newPosition(Bone::BoneId id, Sensor *sensor, glm::vec3 position);
		PassThroughFuser();
	};
};

#endif