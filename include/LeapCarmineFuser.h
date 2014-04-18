#ifndef _LeapCarmineFuser_h_
#define _LeapCarmineFuser_h_

#include <ctime>
#include <stdio.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <cstring>
#include <map>

#include "DataFusionModule.h"
#include "Sensor.h"
#include "Bone.h"
#include "Scene.h"

namespace jester {
	class LeapCarmineFuser : public DataFusionModule {
	public:
		void newData(Sensor *sensor, std::map<Bone::BoneId, BoneFusionData> data);
		void newData(Sensor *sensor, std::map<Bone::JointId, JointFusionData> data);

		void setCarmine(Sensor *carmine);
		void setLeap(Sensor *leap);
		void setSceneRoot(SceneGraphNode *root);

		LeapCarmineFuser();
		~LeapCarmineFuser();
	private:
		typedef struct JointPositionHistory {
			std::clock_t timestamp;
			bool leapData;
			bool carmineData;
			std::map<Bone::JointId, JointFusionData> jointData;
		} JointPositionHistory;

		static const int HistoryLength;
		static const int UpdateHertz;
		static const int DataFrameExpirationMs;
		static const int JointsPerHand;
		static const int MaxRetrievalDistance;
		static const float SwitchDelta;

		std::thread *kSkeletonUpdateThread;
		std::thread *kFrameAdvanceThread;
		bool kContinueUpdating;
		float kSkeletonDelayTime;
		std::mutex kHistoryMutex;

		bool kHasHadC7Lock;

		JointPositionHistory *kJointHistory;
		int kNewestInfo;

		std::clock_t kInitClock;
		Sensor *kCarmine;
		Sensor *kLeap;
		SceneGraphNode *kSceneRoot;

		void updateSkeleton();
		void advanceHistoryFrame();
		void checkTimeout();
		void insertJoints(std::map<Bone::JointId, JointFusionData> joints, glm::mat4 jointWorldTransform);
		void launchThreads();
	};
};

#endif
