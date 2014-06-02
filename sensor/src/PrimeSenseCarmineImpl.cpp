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
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	} while (kDataWanted);
}

void jester::PrimeSenseCarmineImpl::handleNewData(const nite::UserData &user) {
	if (user.isNew()) {
		kUserTracker.startSkeletonTracking(user.getId());
	} else if (user.getSkeleton().getState() == nite::SKELETON_TRACKED) {
		const nite::Skeleton &skeleton = user.getSkeleton();

		std::map<Bone::JointId, JointFusionData> jointData;

		for (unsigned int i = 0; i < JOINT_COUNT; i++) {
			const nite::SkeletonJoint &joint = skeleton.getJoint(intToNiteJoint(i));
			const nite::Point3f &nitePos = joint.getPosition();
			JointFusionData newJoint;

			newJoint.position = glm::vec3(((float) nitePos.x) / DISTANCE_SCALING_FACTOR,
								((float) nitePos.y) / DISTANCE_SCALING_FACTOR,
								((float) nitePos.z) / DISTANCE_SCALING_FACTOR);
			newJoint.id = static_cast<jester::Bone::JointId>(i);
			newJoint.confidence = joint.getPositionConfidence();

			jointData.insert(std::pair<Bone::JointId, JointFusionData>(newJoint.id, newJoint));
		}

		const nite::Point3f &headPos =  skeleton.getJoint(nite::JOINT_HEAD).getPosition();
		const nite::Point3f &centerPos =  skeleton.getJoint(nite::JOINT_TORSO).getPosition();
		glm::vec3 headGPos = glm::vec3(((float) headPos.x) / DISTANCE_SCALING_FACTOR,
								((float) headPos.y) / DISTANCE_SCALING_FACTOR,
								((float) headPos.z) / DISTANCE_SCALING_FACTOR);

		glm::vec3 centerGPos = glm::vec3(((float) centerPos.x) / DISTANCE_SCALING_FACTOR,
								((float) centerPos.y) / DISTANCE_SCALING_FACTOR,
								((float) centerPos.z) / DISTANCE_SCALING_FACTOR);

		JointFusionData upJoint;
		upJoint.id = jester::Bone::EXTEND_UP;
		upJoint.confidence = 1;
		upJoint.position = glm::vec3(headGPos.x, headGPos.y + 0.2, headGPos.z);

		JointFusionData downJoint;
		downJoint.id = jester::Bone::EXTEND_DOWN;
		downJoint.confidence = 1;
		downJoint.position = glm::vec3(centerGPos.x, centerGPos.y - 0.3, centerGPos.z);

		jointData.insert(std::pair<Bone::JointId, JointFusionData>(upJoint.id, upJoint));
		jointData.insert(std::pair<Bone::JointId, JointFusionData>(downJoint.id, downJoint));

		kController->suggestJointInfo(this, jointData);
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
