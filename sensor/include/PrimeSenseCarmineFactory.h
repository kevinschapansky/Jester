#ifndef _PrimeSenseCarmineFactory_h_
#define _PrimeSenseCarmineFactory_h_

#include "Sensor.h"
#include "SceneGraphNode.h"
#include "Controller.h"

namespace jester {
	class PrimeSenseCarmineFactory {
	public:
		static Sensor* CreateCarmineSensor(SceneGraphNode *parent, Controller * controller);
	};
};

#endif