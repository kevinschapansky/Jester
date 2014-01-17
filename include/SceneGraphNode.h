#ifndef _SceneGraphNode_h_
#define _SceneGraphNode_h_

#include <iterator>
#include <vector>

#include "JesterTransform.h"

namespace jester {
	class SceneGraphNode {
	public:
		SceneGraphNode(SceneGraphNode *parent);
		std::iterator<SceneGraphNode> children();
		JesterTransform getTransform();
		JesterTransform getWorldTransform();
	};
};

#endif