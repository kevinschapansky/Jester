#ifndef _PassThroughFuser_h_
#define _PassThroughFuser_h_

#include "DataFusionModule.h"

namespace jester {
	class PassThroughFuser : public DataFusionModule {
	public:
		void newData(Sensor *sensor, BoneFusionData data[Bone::BONE_COUNT]);
		void newData(Sensor *sensor, JointFusionData data[Bone::JOINT_COUNT]);
		PassThroughFuser();
	};
};

#endif