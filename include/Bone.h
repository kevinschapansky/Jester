#ifndef _Bone_h_
#define _Bone_h_

namespace jester {
	enum BoneId {HEAD, LEFT_SHOULDER};

	class Bone : public SceneGraphNode {
		virtual void suggestPosition();
	};
};

#endif