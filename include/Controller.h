#ifndef _Controller_h_
#define _Controller_h_

#include <ctime>

#include "SceneGraphNode.h"
#include "Scene.h"

namespace jester {
	class Controller {
	public:
		void init();
		Scene *getScene();
		long int getTimestamp();
		Controller();
		~Controller();

	private:
		std::clock_t kStartClock;
		Scene *kScene;
	};
};

#endif