#include "PrimeSenseCarmineImpl.h"

const float jester::PrimeSenseCarmineImpl::DISTANCE_SCALING_FACTOR = 1000.f;

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
			
			handleNewData(user);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	} while (kDataWanted);
}

void jester::PrimeSenseCarmineImpl::handleNewData(const nite::UserData &user) {
	if (user.isNew()) {
		kUserTracker.startSkeletonTracking(user.getId());
	} else if (user.getSkeleton().getState() == nite::SKELETON_TRACKED) {
		const nite::Skeleton &skeleton = user.getSkeleton();
		glm::vec3 position;
		glm::quat quaternion;

		for (unsigned int i = 0; i < JOINT_COUNT; i++) {
			const nite::SkeletonJoint &joint = skeleton.getJoint(intToNiteJoint(i));
			const nite::Point3f &nitePos = joint.getPosition();
			const nite::Quaternion &niteQuat = joint.getOrientation();

			position = glm::vec3(((float) nitePos.x) / DISTANCE_SCALING_FACTOR,
								((float) nitePos.y) / DISTANCE_SCALING_FACTOR,
								((float) nitePos.z) / DISTANCE_SCALING_FACTOR);
			quaternion = glm::quat((float) niteQuat.w, (float) niteQuat.x, (float) niteQuat.y, (float) niteQuat.z);

			kController->suggestBoneInfo(Bone::intToBoneId(i), this, joint.getPositionConfidence(), &position, &quaternion);
		}
	}
}

jester::PrimeSenseCarmineImpl::PrimeSenseCarmineImpl(SceneGraphNode *parent, Controller *controller) : Sensor(parent, controller) {
	nite::Status niteRc;

	nite::NiTE::initialize();
	niteRc = kUserTracker.create();

	if (niteRc != nite::STATUS_OK)
	{
		printf("Carmine:: Couldn't create user tracker\n");
		kCreationSuccessful = false;
		return;
	}
	kCreationSuccessful = true;
}

bool jester::PrimeSenseCarmineImpl::start() {
	if (kCreationSuccessful) {
		kDataWanted = true;
		kUpdateThread = new std::thread(&jester::PrimeSenseCarmineImpl::pollData, this);
		return true;
	} else {
		return false;
	}
}

jester::PrimeSenseCarmineImpl::~PrimeSenseCarmineImpl() {
	kDataWanted = false;
	kUpdateThread->join();
	delete kUpdateThread;
	nite::NiTE::shutdown();
}

 const unsigned int jester::PrimeSenseCarmineImpl::JOINTS[] = {
			nite::JOINT_HEAD, nite::JOINT_NECK, nite::JOINT_LEFT_SHOULDER, nite::JOINT_RIGHT_SHOULDER, 
			nite::JOINT_LEFT_ELBOW, nite::JOINT_RIGHT_ELBOW, nite::JOINT_LEFT_HAND, nite::JOINT_RIGHT_HAND,
			nite::JOINT_TORSO, nite::JOINT_LEFT_HIP, nite::JOINT_RIGHT_HIP, nite::JOINT_LEFT_KNEE,
			nite::JOINT_RIGHT_KNEE, nite::JOINT_LEFT_FOOT, nite::JOINT_RIGHT_FOOT};

nite::JointType jester::PrimeSenseCarmineImpl::intToNiteJoint(int jointInt) {
	return static_cast<nite::JointType>(jointInt);
}