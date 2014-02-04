#include "Sensor.h"

void jester::Sensor::setTransform(JesterTransform newTransform) {
	kTransform = newTransform.toGLMMatrix();
}

jester::Sensor::Sensor(SceneGraphNode *parent) : SceneGraphNode(parent) {

}

jester::Sensor::~Sensor() {

}