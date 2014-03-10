#include "Sensor.h"
#include "Controller.h"

void jester::Sensor::setPosition(glm::vec3 position) {
	kPosition = position;
}
		
void jester::Sensor::setOrientation(glm::quat orientation) {
	kOrientation = orientation;
}

jester::Sensor::Sensor(SceneGraphNode *parent, Controller *controller) : SceneGraphNode(parent) {
	kParent = parent;
	kController = controller;
}

jester::Sensor::~Sensor() {

}