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
		virtual void newData(Sensor *sensor, std::map<Bone::JointId, JointFusionData> data);
		void setCarmine(Sensor *carmine, std::map<jester::Bone::BoneId, double> carmineConfMap);
		void setLeap(Sensor *leap, std::map<jester::Bone::BoneId, double> leapConfMap);

		LeapCarmineFuser();
		~LeapCarmineFuser();
	protected:
		virtual void insertBoneDataIntoFrame(int frame, Sensor* sensor, std::map<Bone::BoneId, BoneFusionData> bones);
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
