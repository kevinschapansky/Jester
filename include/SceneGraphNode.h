#ifndef _SceneGraphNode_h_
#define _SceneGraphNode_h_

#include <iterator>
#include <vector>
#include <glm/mat4x4.hpp>

#include "JesterTransform.h"

namespace jester {
	class SceneGraphNode {
	public:
		std::vector<SceneGraphNode *>::iterator children();
		JesterTransform getTransform();
		virtual JesterTransform getWorldTransform();

		SceneGraphNode(SceneGraphNode *parent);
		virtual ~SceneGraphNode();

	private:
		void addChild(SceneGraphNode *child);
		void removeChild(SceneGraphNode *child);

		std::vector<SceneGraphNode *> kChildren;
		SceneGraphNode *kParent;

	protected:
		glm::mat4x4 kTransform;
	};
};

#endif