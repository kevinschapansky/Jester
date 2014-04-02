#ifndef _LeapMotionFactory_h_
#define _LeapMotionFactory_h_

#include "Sensor.h"
#include "SceneGraphNode.h"
#include "Controller.h"

namespace jester {
	class LeapMotionFactory {
	public:
		static Sensor* CreateLeapSensor(SceneGraphNode *parent, Controller * controller);
	};
};

#endif
