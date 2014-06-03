#ifndef _BasicDataFuser_h_
#define _BasicDataFuser_h_

#include <ctime>
#include <stdio.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <cstring>
#include <map>
#include <vector>
#include <queue>
#include <sys/time.h>

#include "DataFusionModule.h"
#include "Sensor.h"
#include "Bone.h"
#include "Scene.h"
#include "Filter.h"

namespace jester {
	class BasicDataFuser : public DataFusionModule {
	public:
		virtual void newData(Sensor *sensor, std::map<Bone::BoneId, BoneFusionData> data);
		virtual void newData(Sensor *sensor, std::map<Bone::JointId, JointFusionData> data);

		virtual void addSensor(Sensor *sensor, std::map<Bone::BoneId, double> boneConfidence);
		virtual void startFusion();

		BasicDataFuser();
		virtual ~BasicDataFuser();

	protected:
		typedef struct BonePositionHistory {
			double timestamp;
			std::map<Sensor*, std::map<Bone::BoneId, BoneFusionData>> rawBoneData;
		} BonePositionHistory;

		static const int HistoryLength;
		static const int UpdateHertz;
		static const int MaxRetrievalDistance;
		static const float FusionLowerThreshold;

		std::thread *kSkeletonUpdateThread;

		bool kContinueUpdating;
		double kSkeletonDelayTime;
		std::mutex kHistoryMutex;

		BonePositionHistory *kBoneHistory;
		int kNewestInfo;

		double kInitClock;

		std::vector<Sensor *> kSensors;
		std::map<Sensor*, std::map<Bone::BoneId, double>> kSensorConfidences;
		std::map<Bone::BoneId, Filter *> kFilters;

		virtual void initializeHistory();
		virtual void initializeFilters();
		virtual void updateSkeleton();
		virtual std::map<Bone::BoneId, BoneFusionData> fuseBoneDataFromStartFrame(int frame);
		virtual void insertBoneDataIntoMap(std::map<Bone::BoneId, BoneFusionData> *map, Sensor* sensor, std::map<Bone::BoneId, BoneFusionData> bones);
		virtual void mergeKnownBoneWithAssumedBone(BoneFusionData *boneA, BoneFusionData *boneB);
		virtual jester::BoneFusionData findSensorBoneFromFrame(int frame, Sensor *sensor, Bone::BoneId boneId);
		virtual void advanceHistoryFrame();
	private:
		double getWallTime() {
			struct timeval time;
			if (gettimeofday(&time,NULL)){
				//  Handle error
				return 0;
			}
			return (double)time.tv_sec + (double)time.tv_usec * .000001;
		}
	};

	class BasicDataFuserFactory : public DataFusionModuleFactory {
	public:
		DataFusionModule* CreateFusionModule();
	};
};

#endif
