#ifndef _SceneGraphNode_h_
#define _SceneGraphNode_h_

#define GLM_FORCE_RADIANS

#include <iterator>
#include <vector>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

namespace jester {
	class SceneGraphNode {
	public:
		std::vector<SceneGraphNode *>::iterator children();
		
		glm::vec3 getPosition();
		virtual glm::vec3 getWorldPosition() = 0;

		glm::quat getOrientation();
		virtual glm::quat getWorldOrientation() = 0;

		SceneGraphNode *getParent();


		SceneGraphNode(SceneGraphNode *parent);
		virtual ~SceneGraphNode();

	private:
		void addChild(SceneGraphNode *child);
		void removeChild(SceneGraphNode *child);

		std::vector<SceneGraphNode *> kChildren;
	protected:
		SceneGraphNode *kParent;
		glm::vec3 kPosition;
		glm::quat kOrientation;
	};
};

#endif