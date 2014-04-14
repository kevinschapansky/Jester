#ifndef _Scene_h_
#define _Scene_h_

#include "SceneGraphNode.h"
#include "DataFusionModule.h"
#include "Bone.h"
#include "FusionBone.h"

namespace jester {
	class Scene : public SceneGraphNode {
	public:
		Bone* getBone(Bone::BoneId bone);
		Bone* getRootBone();

		static SceneGraphNode* getGraphRoot(SceneGraphNode *graphMember) {
			SceneGraphNode *curNode = graphMember;

			while (curNode != NULL && curNode->getParent() != NULL)
				curNode = curNode->getParent();
			return curNode;
		};
		
		Scene(DataFusionModule *fuser);
		~Scene();
	protected:
		FusionBone *kSkeleton[Bone::BONE_COUNT];

		void buildSkeleton();
	};
};

#endif
