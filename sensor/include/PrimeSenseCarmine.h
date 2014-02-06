#ifndef _PrimeSenseCarmine_h_
#define _PrimeSenseCarmine_h_

#include <stdio.h>
#include <thread>
#include <chrono>

#include <OpenNI.h>
#include "NiTE.h"

#include "Sensor.h"
#include "SceneGraphNode.h"

namespace jester {
	class PrimeSenseCarmine : public Sensor {
	public:
		PrimeSenseCarmine(SceneGraphNode *parent);
		~PrimeSenseCarmine();
	private:
		nite::UserTracker kUserTracker;
		std::thread *kUpdateThread;
		bool kDataWanted;
		static const SLEEP_MILLISECONDS = 100;

		void pollData();
	};
};

#endif