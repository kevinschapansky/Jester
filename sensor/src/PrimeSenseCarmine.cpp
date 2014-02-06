#include "PrimeSenseCarmine.h"

#include <stdio.h>
#include <thread>
#include <chrono>

#include <OpenNI.h>
#include "NiTE.h"

void jester::PrimeSenseCarmine::pollData() {
	do {
		std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_MILLISECONDS));
	} while (kDataWanted);
}

jester::PrimeSenseCarmine::PrimeSenseCarmine(SceneGraphNode *parent) : Sensor(SceneGraphNode *parent, Controller *controller) {
	nite::Status niteRc;

	nite::NiTE::initialize();
	niteRc = kUserTracker.create();

	if (niteRc != nite::STATUS_OK)
	{
		printf("Couldn't create user tracker\n");
		return;
	}
}

void jester::PrimeSenseCarmine::start() {
	kDataWanted = true;
	kUpdateThread = new std::thread(&jester::PrimeSenseCarmine::pollData, this);
}

jester::PrimeSenseCarmine::~PrimeSenseCarmine() {
	kDataWanted = false;
	kUpdateThread->join();
	delete kUpdateThread;
	nite::NiTE::shutdown();
}