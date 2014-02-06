#include "PrimeSenseCarmineImpl.h"

void jester::PrimeSenseCarmineImpl::pollData() {
	do {
		std::this_thread::sleep_for(std::chrono::milliseconds(100);
	} while (kDataWanted);
}

jester::PrimeSenseCarmineImpl::PrimeSenseCarmineImpl(SceneGraphNode *parent, Controller *controller) : Sensor(parent, controller) {
	nite::Status niteRc;

	nite::NiTE::initialize();
	niteRc = kUserTracker.create();

	if (niteRc != nite::STATUS_OK)
	{
		printf("Couldn't create user tracker\n");
		return;
	}
}

void jester::PrimeSenseCarmineImpl::start() {
	kDataWanted = true;
	kUpdateThread = new std::thread(&jester::PrimeSenseCarmineImpl::pollData, this);
}

jester::PrimeSenseCarmineImpl::~PrimeSenseCarmineImpl() {
	kDataWanted = false;
	kUpdateThread->join();
	delete kUpdateThread;
	nite::NiTE::shutdown();
}