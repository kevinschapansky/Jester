#include "LeapMotionFactory.h"
#include "LeapMotionImpl.h"

jester::Sensor* jester::LeapMotionFactory::CreateLeapSensor(SceneGraphNode *parent, Controller * controller) {
	return new LeapMotionImpl(parent, controller);
}
