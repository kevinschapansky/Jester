#ifndef _LeapCarmineFuser_h_
#define _LeapCarmineFuser_h_

#include <ctime>
#include <stdio.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <cstring>

#include "DataFusionModule.h"
#include "Sensor.h"
#include "Bone.h"
#include "Scene.h"

namespace jester {
	class LeapCarmineFuser : public DataFusionModule {
	public:
		void newData(Sensor *sensor, BoneFusionData data[Bone::BONE_COUNT]);
		void newData(Sensor *sensor, JointFusionData data[Bone::JOINT_COUNT]);

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
			JointFusionData jointData[Bone::JOINT_COUNT];
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
		void fuseHandPositions(JointFusionData *carmineJoints, JointFusionData *leapJoints);
		void insertLeapJoints(JointFusionData *leapJoints);
		void insertCarmineJoints(JointFusionData *carmineJoints);
		void launchThreads();
	};
};

#endif
