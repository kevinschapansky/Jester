#ifndef _BoneDoubleExponentialFilter_h_
#define _BoneDoubleExponentialFilter_h_

#include <glm/vec3.hpp>

#include "DataFusionModule.h"

namespace jester {
	class BoneDoubleExponentialFilter : public Filter {
	public:
		BoneFusionData update(Sensor *sensor, BoneFusionData boneData);

		BoneDoubleExponentialFilter(float dataSmoothingFactor, float rateSmoothingFactor);
	private:
		float kAlpha;
		float kBeta;

		bool kT0;
		bool kT1;

		glm::vec3 kSmoothedStart;
		glm::vec3 kSmoothedEnd;
		glm::vec3 kTrendStart;
		glm::vec3 kTrendEnd;
	};

	class DoubleExponentialFilterFactory : public FilterFactory {
	public:
		Filter* createFilter();

		DoubleExponentialFilterFactory(float dataSmoothingFactor, float rateSmoothingFactor);
		~DoubleExponentialFilterFactory();

	private:
		float kAlpha;
		float kBeta;
	};
};

#endif
