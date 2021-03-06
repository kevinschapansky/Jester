#ifndef _FusionBone_h_
#define _FusionBone_h_

#include "Bone.h"
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace jester {
	class FusionBone : public Bone {
	public:
		void setPosition(glm::vec3 position, float confidence);
		void setOrientation(glm::quat orientation, float confidence);
		void setLength(float length);
		void setWidth(float width);
		void setDefaultEndpoints(glm::vec3, glm::vec3);
		void setChildren(std::vector<SceneGraphNode *> children);

		FusionBone(SceneGraphNode *parent, BoneId type);
	};
};

#endif
