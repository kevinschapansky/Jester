#include "SceneGraphNode.h"

std::vector<jester::SceneGraphNode *>::iterator jester::SceneGraphNode::children() {
	return kChildren.begin();
}

glm::vec3 jester::SceneGraphNode::getPosition() {
	return kPosition;
}

glm::quat jester::SceneGraphNode::getOrientation() {
	return kOrientation;
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