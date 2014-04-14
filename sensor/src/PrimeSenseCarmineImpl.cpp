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

		jester::JointFusionData jointData[jester::Bone::JOINT_COUNT];

		for (unsigned int i = 0; i < JOINT_COUNT; i++) {
			const nite::SkeletonJoint &joint = skeleton.getJoint(intToNiteJoint(i));
			const nite::Point3f &nitePos = joint.getPosition();

			glm::vec3 *position = new glm::vec3(((float) nitePos.x) / DISTANCE_SCALING_FACTOR,
								((float) nitePos.y) / DISTANCE_SCALING_FACTOR,
								((float) nitePos.z) / DISTANCE_SCALING_FACTOR);
			jointData[i].id = static_cast<jester::Bone::JointId>(i);
			jointData[i].confidence = joint.getPositionConfidence();
			jointData[i].position = position;
		}

		//Fill the finger bones with null confidence
		for (int i = JOINT_COUNT; i < jester::Bone::JOINT_COUNT; i++) {
			jointData[i].id = static_cast<jester::Bone::JointId>(i);
			jointData[i].confidence = 0.0;
			jointData[i].position = NULL;
		}

		const nite::Point3f &headPos =  skeleton.getJoint(nite::JOINT_HEAD).getPosition();
		const nite::Point3f &centerPos =  skeleton.getJoint(nite::JOINT_TORSO).getPosition();
		glm::vec3 headGPos = glm::vec3(((float) headPos.x) / DISTANCE_SCALING_FACTOR,
								((float) headPos.y) / DISTANCE_SCALING_FACTOR,
								((float) headPos.z) / DISTANCE_SCALING_FACTOR);

		glm::vec3 centerGPos = glm::vec3(((float) centerPos.x) / DISTANCE_SCALING_FACTOR,
								((float) centerPos.y) / DISTANCE_SCALING_FACTOR,
								((float) centerPos.z) / DISTANCE_SCALING_FACTOR);

		jointData[jester::Bone::EXTEND_UP].id = jester::Bone::EXTEND_UP;
		jointData[jester::Bone::EXTEND_UP].confidence = 1;
		jointData[jester::Bone::EXTEND_UP].position = new glm::vec3(headGPos.x, headGPos.y + 0.2, headGPos.z);

		jointData[jester::Bone::EXTEND_DOWN].id = jester::Bone::EXTEND_DOWN;
		jointData[jester::Bone::EXTEND_DOWN].confidence = 1;
		jointData[jester::Bone::EXTEND_DOWN].position = new glm::vec3(centerGPos.x, centerGPos.y - 0.3, centerGPos.z);

		kController->suggestJointInfo(this, jointData);

		for (int i = 0; i < jester::Bone::JOINT_COUNT; i++)
			delete jointData[i].position;
	}
}

jester::PrimeSenseCarmineImpl::PrimeSenseCarmineImpl(SceneGraphNode *parent, Controller *controller) : Sensor(parent, controller) {
	nite::Status niteRc;

	nite::NiTE::initialize();
	niteRc = kUserTracker.create();
	kDataWanted = false;
	kUpdateThread = NULL;

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
