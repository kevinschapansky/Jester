#include "Sensor.h"
#include "Controller.h"

glm::quat jester::Sensor::getWorldOrientation() {
	return glm::quat();
}

glm::vec3 jester::Sensor::getWorldPosition() {
	glm::vec3 worldPosition = kPosition;
	SceneGraphNode *parent = kParent;

	while (parent != NULL) {
		worldPosition += parent->getPosition();
		parent = parent->getParent();
	}
	return worldPosition;
}

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