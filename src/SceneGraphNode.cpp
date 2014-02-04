#include "SceneGraphNode.h"

std::vector<jester::SceneGraphNode *>::iterator jester::SceneGraphNode::children() {
	return kChildren.begin();
}
		
jester::JesterTransform jester::SceneGraphNode::getTransform() {
	return JesterTransform(kTransform);
}
		
jester::JesterTransform jester::SceneGraphNode::getWorldTransform() {
	glm::mat4x4 worldTransform = kTransform;
	SceneGraphNode *parent = kParent;

	while (parent != NULL) {
		worldTransform *= parent->kTransform;
		parent = parent->kParent;
	}
	return JesterTransform(worldTransform);
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