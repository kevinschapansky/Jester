#ifndef _Controller_h_
#define _Controller_h_

#include <ctime>
#include <glm/vec3.hpp>

#include "Scene.h"
#include "DataFusionModule.h"
#include "Sensor.h"
#include "Bone.h"

namespace jester {
	class Controller {
	public:
		void init();
		Scene *getScene();
		long int getTimestamp();
		void suggestBoneInfo(Bone::BoneId bone, Sensor *sensor, float confidence, glm::vec3 *position, glm::quat *orientation);

		Controller(DataFusionModule::FusionAlgorithm algorithm);
		~Controller();
	private:
		DataFusionModule::FusionAlgorithm kAlgorithm;
		std::clock_t kStartClock;
		Scene *kScene;
		DataFusionModule *kFusionModule;
	};
};

#endif