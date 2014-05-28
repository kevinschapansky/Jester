#ifndef _LeapCarmineFuser_h_
#define _LeapCarmineFuser_h_

#include <ctime>
#include <stdio.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <cstring>
#include <map>

#include "BasicDataFuser.h"

namespace jester {
	class LeapCarmineFuser : public BasicDataFuser {
	public:
		void newData(Sensor *sensor, std::map<Bone::BoneId, BoneFusionData> data);
		void newData(Sensor *sensor, std::map<Bone::JointId, JointFusionData> data);

		void setCarmine(Sensor *carmine);
		void setLeap(Sensor *leap);

		LeapCarmineFuser();
		~LeapCarmineFuser();
	private:
		static const int JointsPerHand;
		static const float SwitchDelta;

		Sensor *kCarmine;
		Sensor *kLeap;
	};

	class LeapCarmineFuserFactory : public DataFusionModuleFactory {
	public:
		DataFusionModule* CreateFusionModule();
	};
};

#endif
