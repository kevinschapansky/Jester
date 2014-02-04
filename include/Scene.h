#ifndef _Scene_h_
#define _Scene_h_

#include "SceneGraphNode.h"
#include "Bone.h"
#include "JesterTransform.h"

namespace jester {
	class Scene : public SceneGraphNode {
	public:
		Bone* getBone(Bone::BoneId bone);
		JesterTransform getWorldTransform();
		
		Scene();
		~Scene();
	private:
		Bone *kSkeleton[Bone::BONE_COUNT];

		void buildSkeleton();
	};
};

#endif