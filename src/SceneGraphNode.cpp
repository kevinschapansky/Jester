#include "SceneGraphNode.h"

std::vector<jester::SceneGraphNode *>::const_iterator jester::SceneGraphNode::children() {
	return kChildren.cbegin();
}

glm::vec3 jester::SceneGraphNode::getPosition() {
	return kPosition;
}

glm::quat jester::SceneGraphNode::getOrientation() {
	return kOrientation;
}

glm::mat4 jester::SceneGraphNode::getTransform() {
	return glm::translate(glm::mat4_cast(kOrientation), kPosition);
}

glm::quat jester::SceneGraphNode::getWorldOrientation() {
	if (kParent == NULL)
		return kOrientation;

	return kParent->getWorldOrientation() * kOrientation;
}

glm::mat4 jester::SceneGraphNode::getWorldTransform() {
	if (kParent == NULL)
		return getTransform();
	return kParent->getWorldTransform() * getTransform();
}

glm::vec3 jester::SceneGraphNode::getWorldPosition() {
	return glm::vec3(getWorldTransform() * glm::vec4(0, 0, 0, 1));
}

glm::vec3 jester::SceneGraphNode::positionSpaceConversion(glm::vec3 position, SceneGraphNode *current, SceneGraphNode *desired) {
	glm::mat4 currentWorldTransform = current->getWorldTransform();
	glm::mat4 desiredWorldTransform = desired->getWorldTransform();
	glm::vec3 worldPosition = glm::vec3(currentWorldTransform * glm::vec4(position, 1));

	return glm::vec3(glm::inverse(desiredWorldTransform) * glm::vec4(worldPosition, 1));
}

glm::quat jester::SceneGraphNode::orientationSpaceConversion(glm::quat orientation, SceneGraphNode *current, SceneGraphNode *desired) {
	glm::quat currentWorldOrientation = current->getWorldOrientation();
	glm::quat desiredWorldOrientation = desired->getWorldOrientation();
	glm::quat worldOrientation = currentWorldOrientation * orientation;

	return glm::inverse(desiredWorldOrientation) * worldOrientation;
}

jester::SceneGraphNode* jester::SceneGraphNode::getParent() {
	return kParent;
}

jester::SceneGraphNode::SceneGraphNode(SceneGraphNode *parent) {
	if (parent != NULL)
		parent->addChild(this);
	kParent = parent;
}
		
jester::SceneGraphNode::~SceneGraphNode() {

}

void jester::SceneGraphNode::addChild(SceneGraphNode *child) {
	kChildren.push_back(child);
}

void jester::SceneGraphNode::removeChild(SceneGraphNode *child) {
	for (std::vector<SceneGraphNode *>::iterator it = kChildren.begin(); it != kChildren.end(); ++it) {
		if (*it == child) {
			kChildren.erase(it);
		}
	}
}