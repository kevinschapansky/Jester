#ifndef _PrimeSenseCarmineImpl_h_
#define _PrimeSenseCarmineImpl_h_

#include <stdio.h>
#include <thread>
#include <chrono>

#include <glm/vec3.hpp>
#include <OpenNI.h>
#include <NiTE.h>

#include "Sensor.h"
#include "SceneGraphNode.h"
#include "Controller.h"
#include "Bone.h"

namespace jester {
	class PrimeSenseCarmineImpl : public Sensor {
	public:
		bool start();

		PrimeSenseCarmineImpl(SceneGraphNode *parent, Controller *controller);
		~PrimeSenseCarmineImpl();
	private:
		//static const unsigned int SLEEP_MILLISECONDS = 100;
		static const unsigned int JOINT_COUNT = 15;
		static const unsigned int JOINTS[];
		static const float DISTANCE_SCALING_FACTOR;

		nite::UserTracker kUserTracker;
		bool kDataWanted;
		bool kCreationSuccessful;
		std::thread *kUpdateThread;

		void pollData();
		void handleNewData(const nite::UserData &user);
		static nite::JointType intToNiteJoint(int jointInt);
	};
};

#endif