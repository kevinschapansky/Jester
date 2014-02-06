#include "PrimeSenseCarmineFactory.h"
#include "PrimeSenseCarmineImpl.h"

jester::Sensor* jester::PrimeSenseCarmineFactory::CreateCarmineSensor(SceneGraphNode *parent, Controller * controller) {
	return new PrimeSenseCarmineImpl(parent, controller);
}
