#ifndef _DataFusionAlgorithm_h_
#define _DataFusionAlgorithm_h_

#include "Bone.h"
#include "Sensor.h"
#include "JesterPosition.h"

namespace jester {
	class DataFusionAlgorithm {
	public:
		virtual void newData(Bone *bone, Sensor *sensor);
		DataFusionAlgorithm();
		virtual ~DataFusionAlgorithm();
	private:
		void notifyBone(Bone *bone, JesterPosition *position);
	};
};

#endif