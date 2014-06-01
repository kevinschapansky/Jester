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

		virtual void addSensor(Sensor *sensor);
		virtual void startFusion();

		BasicDataFuser();
		virtual ~BasicDataFuser();

	protected:
		typedef struct BonePositionHistory {
			double timestamp;
			std::map<Sensor*, std::map<Bone::BoneId, BoneFusionData>> rawBoneData;
			std::map<Bone::BoneId, BoneFusionData> fusedBoneData;
		} BonePositionHistory;

		static const int HistoryLength;
		static const int UpdateHertz;
		static const int MaxRetrievalDistance;

		std::thread *kSkeletonUpdateThread;

		bool kContinueUpdating;
		double kSkeletonDelayTime;
		std::mutex kHistoryMutex;

		BonePositionHistory *kBoneHistory;
		int kNewestInfo;

		double kInitClock;

		std::vector<Sensor *> kSensors;
		std::map<Bone::BoneId, Filter *> kFilters;

		void initializeHistory();
		void initializeFilters();
		void updateSkeleton();
		void fuseBoneDataInFrame(int frame);
		void insertBoneDataIntoFrame(int frame, Sensor* sensor, std::map<Bone::BoneId, BoneFusionData> bones);
		std::map<Bone::BoneId, BoneFusionData> findBestSkeletonFromFrame(int frame);
		void advanceHistoryFrame();
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
