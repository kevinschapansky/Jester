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

#include "DataFusionModule.h"
#include "Sensor.h"
#include "Bone.h"
#include "Scene.h"

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
		typedef struct JointPositionHistory {
			std::clock_t timestamp;
			bool *sensorDataPresent;
			std::map<Bone::JointId, JointFusionData> jointData;
		} JointPositionHistory;

		static const int HistoryLength;
		static const int UpdateHertz;
		static const int DataFrameExpirationMs;
		static const int DataFrameExpirationCheckMs;
		static const int MaxRetrievalDistance;

		std::thread *kSkeletonUpdateThread;
		std::thread *kFrameAdvanceThread;

		bool kContinueUpdating;
		float kSkeletonDelayTime;
		std::mutex kHistoryMutex;

		JointPositionHistory *kJointHistory;
		int kNewestInfo;

		std::clock_t kInitClock;

		std::map<int, Sensor *> kSensors;
		int kSensorCount;
		std::queue<std::pair<Sensor*, std::map<Bone::BoneId, BoneFusionData>>> kDataQueue;
		//std::map<Bone::JointId, JointParticleFilter *> kFilters;

		void updateSkeleton();
		void advanceHistoryFrame();
		void checkTimeout();
		void insertJoints(std::map<Bone::JointId, JointFusionData> joints, glm::mat4 jointWorldTransform);
		void initializeHistory();
		void initializeFilters();
	private:

	};

	class BasicDataFuserFactory : public DataFusionModuleFactory {
	public:
		DataFusionModule* CreateFusionModule();
	};
};

#endif
