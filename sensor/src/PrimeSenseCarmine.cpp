#include "PrimeSenseCarmine.h"

void jester::PrimeSenseCarmine::pollData() {
	do {
		std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_MILLISECONDS));
	} while (kDataWanted);
}

jester::PrimeSenseCarmine::PrimeSenseCarmine(SceneGraphNode *parent) : Sensor(parent) {
	nite::Status niteRc;

	nite::NiTE::initialize();
	niteRc = kUserTracker.create();

	if (niteRc != nite::STATUS_OK)
	{
		printf("Couldn't create user tracker\n");
		return;
	}

	kDataWanted = true;
	kUpdateThread = new std::thread(&jester::PrimeSenseCarmine::pollData, this);
}

jester::PrimeSenseCarmine::~PrimeSenseCarmine() {
	kDataWanted = false;
	kUpdateThread->join();
	delete kUpdateThread;
	nite::NiTE::shutdown();
}