#ifndef _Filter_h_
#define _Filter_h_

#include "DataFusionModule.h"

namespace jester {
	class BoneFusionData;

	class Filter {
	public:
		virtual BoneFusionData update(Sensor *sensor, BoneFusionData boneData) = 0;

		virtual ~Filter() {};
	};

	class FilterFactory {
	public:
		virtual Filter* createFilter() = 0;

		virtual ~FilterFactory() {};
	};
};

#endif
