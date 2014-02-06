#ifndef _Scene_h_
#define _Scene_h_

#include "SceneGraphNode.h"
#include "DataFusionModule.h"
#include "Bone.h"
#include "FusionBone.h"

namespace jester {
	class Scene : public SceneGraphNode {
	public:
		glm::vec3 getWorldPosition();
		glm::quat getWorldOrientation();

		Bone* getBone(Bone::BoneId bone);
		
		Scene(DataFusionModule *fuser);
		~Scene();
	protected:
		FusionBone *kSkeleton[Bone::BONE_COUNT];

		void buildSkeleton();
	};
};

#endif