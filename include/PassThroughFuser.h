#ifndef _PassThroughFuser_h_
#define _PassThroughFuser_h_

#include "DataFusionModule.h"

namespace jester {
	class PassThroughFuser : public DataFusionModule {
	public:
		void newData(Sensor *sensor, std::vector<BoneFusionData *> data);
		void newData(Sensor *sensor, std::vector<JointFusionData *> data);
		PassThroughFuser();
	};
};

#endif