#include "PrimeSenseCarmineImpl.h"


void jester::PrimeSenseCarmineImpl::pollData() {
	nite::UserTrackerFrameRef userTrackerFrame;
	nite::Status niteRc;

	do {
		niteRc = kUserTracker.readFrame(&userTrackerFrame);

		if (niteRc != nite::STATUS_OK) {
			printf("Carmine: Failed to get next frame\n");
			continue;
		}

		const nite::Array<nite::UserData> &users = userTrackerFrame.getUsers();

		if (users.getSize() > 0) {
			const nite::UserData &user = users[users.getSize() - 1];
			
			if (user.isNew()) {
				kUserTracker.startSkeletonTracking(user.getId());
			} else if (user.getSkeleton().getState() == nite::SKELETON_TRACKED) {
				const nite::SkeletonJoint &head = user.getSkeleton().getJoint(nite::JOINT_HEAD);

				if (head.getPositionConfidence() > 0.5) {
					printf("%d. (%5.2f, %5.2f, %5.2f)\n", user.getId(), head.getPosition().x, head.getPosition().y, head.getPosition().z);
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	} while (kDataWanted);
}

jester::PrimeSenseCarmineImpl::PrimeSenseCarmineImpl(SceneGraphNode *parent, Controller *controller) : Sensor(parent, controller) {
	nite::Status niteRc;

	nite::NiTE::initialize();
	niteRc = kUserTracker.create();

	if (niteRc != nite::STATUS_OK)
	{
		printf("Carmine:: Couldn't create user tracker\n");
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