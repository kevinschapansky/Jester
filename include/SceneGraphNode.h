#ifndef _SceneGraphNode_h_
#define _SceneGraphNode_h_

#define GLM_FORCE_RADIANS

#include <iterator>
#include <vector>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace jester {
	class SceneGraphNode {
	public:
		std::vector<SceneGraphNode *> children();
		
		glm::vec3 getPosition();
		virtual glm::vec3 getWorldPosition();

		glm::quat getOrientation();
		virtual glm::quat getWorldOrientation();

		virtual glm::mat4 getTransform();
		virtual glm::mat4 getWorldTransform();

		static glm::vec3 positionSpaceConversion(glm::vec3 position, SceneGraphNode *current, SceneGraphNode *desired);
		static glm::quat orientationSpaceConversion(glm::quat orientation, SceneGraphNode *current, SceneGraphNode *desired);

		SceneGraphNode *getParent();

		SceneGraphNode(SceneGraphNode *parent);
		virtual ~SceneGraphNode();

	private:
		void addChild(SceneGraphNode *child);
		void removeChild(SceneGraphNode *child);

	protected:
		SceneGraphNode *kParent;
		glm::vec3 kPosition;
		glm::quat kOrientation;
		std::vector<SceneGraphNode *> kChildren;
	};
};

#endif
