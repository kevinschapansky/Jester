#ifndef _PassThroughFuser_h_
#define _PassThroughFuser_h_

#include "DataFusionModule.h"

namespace jester {
	class PassThroughFuser : public DataFusionModule {
	public:
		void newData(Bone::BoneId id, Sensor *sensor, float confidence, glm::vec3 *position, glm::quat *orientation);
		PassThroughFuser();
	};
};

#endif