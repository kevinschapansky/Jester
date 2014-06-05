#ifndef _PassThroughFuser_h_
#define _PassThroughFuser_h_

#include "DataFusionModule.h"

namespace jester {
	class PassThroughFuser : public DataFusionModule {
	public:
		void newData(Sensor *sensor, std::map<Bone::BoneId, BoneFusionData> data);
		void newData(Sensor *sensor, std::map<Bone::JointId, JointFusionData> data);
		PassThroughFuser(FilterFactory *factory);
	};
};

#endif
