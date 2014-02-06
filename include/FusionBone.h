#ifndef _FusionBone_h_
#define _FusionBone_h_

#include "Bone.h"
#include <glm/vec3.hpp>

namespace jester {
	class FusionBone : public Bone {
	public:
		void setPosition(glm::vec3 position);

		FusionBone(SceneGraphNode *parent, BoneId type);
	};
};

#endif