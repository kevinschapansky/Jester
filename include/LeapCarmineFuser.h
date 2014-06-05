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
		void setCarmine(Sensor *carmine, std::map<jester::Bone::BoneId, double> carmineConfMap);
		void setLeap(Sensor *leap, std::map<jester::Bone::BoneId, double> leapConfMap);

		LeapCarmineFuser(FilterFactory* filterFactory);
		~LeapCarmineFuser();
	protected:
		virtual void handleSwaps(std::map<Sensor *, std::map<Bone::BoneId, BoneFusionData>> *dataset);
		void swapBone(Bone::BoneId a, Bone::BoneId b, std::map<Bone::BoneId, BoneFusionData> *set);
	private:
		static const int BonesPerHand;
		static const float SwitchDelta;

		Sensor *kCarmine;
		Sensor *kLeap;
	};

	class LeapCarmineFuserFactory : public DataFusionModuleFactory {
	public:
		DataFusionModule* createFusionModule();

		LeapCarmineFuserFactory(FilterFactory *filterFactory);
	};
};

#endif
