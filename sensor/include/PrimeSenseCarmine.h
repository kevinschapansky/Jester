#ifndef _PrimeSenseCarmine_h_
#define _PrimeSenseCarmine_h_

#include "Sensor.h"
#include "SceneGraphNode.h"

namespace jester {
	class PrimeSenseCarmine : public Sensor {
	public:
		PrimeSenseCarmine(SceneGraphNode *parent);
		~PrimeSenseCarmine();
	};
};

#endif