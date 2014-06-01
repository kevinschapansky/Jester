#ifndef _Scene_h_
#define _Scene_h_

#include "SceneGraphNode.h"
#include "DataFusionModule.h"
#include "Bone.h"
#include "FusionBone.h"

namespace jester {
	class DataFusionModule;

	class Scene : public SceneGraphNode {
	public:
		Bone* getBone(Bone::BoneId bone);
		Bone* getRootBone();
		FusionBone** buildSkeleton();
		
		Scene(DataFusionModule *fuser);
		~Scene();
	protected:
		FusionBone **kSkeleton;
	};
};

#endif
