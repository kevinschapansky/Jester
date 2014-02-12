#ifndef _PrimeSenseCarmineImpl_h_
#define _PrimeSenseCarmineImpl_h_

#include <stdio.h>
#include <thread>
#include <chrono>

#include <OpenNI.h>
#include <NiTE.h>

#include "Sensor.h"
#include "SceneGraphNode.h"
#include "Controller.h"

namespace jester {
	class PrimeSenseCarmineImpl : public Sensor {
	public:
		void start();

		PrimeSenseCarmineImpl(SceneGraphNode *parent, Controller *controller);
		~PrimeSenseCarmineImpl();
	private:
		//static const unsigned int SLEEP_MILLISECONDS = 100;

		nite::UserTracker kUserTracker;
		bool kDataWanted;
		std::thread *kUpdateThread;

		void pollData();
	};
};

#endif