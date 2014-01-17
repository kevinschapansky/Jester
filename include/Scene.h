#ifndef _Scene_h_
#define _Scene_h_

#include "SceneGraphNode.h"
#include "Bone.h"

namespace jester {
	class Scene : public SceneGraphNode {
		Bone getBone(BoneID bone);
	};
};

#endif