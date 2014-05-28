#ifndef _Controller_h_
#define _Controller_h_

#include <ctime>
#include <glm/vec3.hpp>
#include <map>

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
		void suggestBoneInfo(Sensor *sensor, std::map<Bone::BoneId, BoneFusionData> data);
		void suggestJointInfo(Sensor *sensor, std::map<Bone::JointId, JointFusionData> data);
		void addSensor(Sensor *sensor);
		DataFusionModule* getDataFusionModule();

		Controller(DataFusionModuleFactory *fusionModuleFactory);
		~Controller();
	private:
		std::clock_t kStartClock;
		Scene *kScene;
		DataFusionModule *kFusionModule;
	};
};

#endif
