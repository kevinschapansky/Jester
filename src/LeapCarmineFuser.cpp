#include "LeapCarmineFuser.h"

const float jester::LeapCarmineFuser::SwitchDelta = 0.1f;
const int jester::LeapCarmineFuser::HistoryLength = 50;
const int jester::LeapCarmineFuser::UpdateHertz = 100;
const int jester::LeapCarmineFuser::DataFrameExpirationMs = 200;
const int jester::LeapCarmineFuser::JointsPerHand = 10;
const int jester::LeapCarmineFuser::MaxRetrievalDistance = 10;

void jester::LeapCarmineFuser::newData(Sensor *sensor, std::map<Bone::BoneId, BoneFusionData> data) {
	printf("This implementation of data fusion is not expecting raw bone fusion data.\nExiting...\n");
	exit(EXIT_FAILURE);
}

void jester::LeapCarmineFuser::newData(Sensor *sensor, std::map<Bone::JointId, JointFusionData> data) {
	kHistoryMutex.lock();

	if (sensor == kLeap) {
		insertJoints(data, kLeap->getWorldTransform());
		kJointHistory[kNewestInfo].leapData = true;
	} else if (sensor == kCarmine) {
		insertJoints(data, kCarmine->getWorldTransform());
		kJointHistory[kNewestInfo].carmineData = true;
	} else {
		printf("Unregistered sensor sending data! Ignoring...\n");
	}
	//if the frame is now full, advance it
	if (kJointHistory[kNewestInfo].carmineData && kJointHistory[kNewestInfo].leapData)
		advanceHistoryFrame();
	kHistoryMutex.unlock();
}

void jester::LeapCarmineFuser::insertJoints(std::map<Bone::JointId, JointFusionData> joints, glm::mat4 jointWorldTransform) {
	for (int i = 0; i < Bone::JOINT_COUNT; i++) {
		if (joints.find(Bone::intToJointId(i)) != joints.end()) {
			std::pair<Bone::JointId, JointFusionData> joint = *(joints.find(Bone::intToJointId(i)));

			joint.second.position = glm::vec3(jointWorldTransform * glm::vec4(joint.second.position, 1.f));
			kJointHistory[kNewestInfo].jointData.insert(joint);
		}
	}
}

void jester::LeapCarmineFuser::setCarmine(Sensor *carmine) {
	kCarmine = carmine;
	launchThreads();
}

void jester::LeapCarmineFuser::setLeap(Sensor *leap) {
	kLeap = leap;
	launchThreads();
}

void jester::LeapCarmineFuser::setSceneRoot(SceneGraphNode *root) {
	kSceneRoot = root;
}

jester::LeapCarmineFuser::LeapCarmineFuser() {
	kNewestInfo = 0;
	kInitClock = std::clock();
	kContinueUpdating = false;
	kSkeletonDelayTime = 1.f / UpdateHertz * 1000.f;
	kCarmine = kLeap = NULL;
	kSceneRoot = NULL;
	kJointHistory = new JointPositionHistory[HistoryLength];

	kHasHadC7Lock = false;

	for (int historyIx = 0; historyIx < HistoryLength; historyIx++) {
		kJointHistory[historyIx].carmineData = kJointHistory[historyIx].leapData = false;
		kJointHistory[historyIx].jointData.clear();
	}
	kSkeletonUpdateThread = kFrameAdvanceThread = NULL;
}

jester::LeapCarmineFuser::~LeapCarmineFuser() {
	kContinueUpdating = false;
	kSkeletonUpdateThread->join();
	kFrameAdvanceThread->join();
}

void jester::LeapCarmineFuser::updateSkeleton() {
	std::map<Bone::JointId, JointFusionData> bestSkeleton;

	do {
		kHistoryMutex.lock();
		bestSkeleton.clear();
		for (int jointId = 0; jointId < Bone::JOINT_COUNT; jointId++) {
			int curInfo = kNewestInfo;
			int backSteps = 0;

			while (backSteps < MaxRetrievalDistance &&
					!(kJointHistory[curInfo].jointData.count(Bone::intToJointId(jointId)))) {
				curInfo -= 1;
				curInfo = (curInfo % HistoryLength + HistoryLength) % HistoryLength;
				backSteps++;
			}

			if (kJointHistory[curInfo].jointData.count(Bone::intToJointId(jointId))) {
				bestSkeleton.insert(*(kJointHistory[curInfo].jointData.find(Bone::intToJointId(jointId))));
			}
		}

		setSkeletonFromJoints(kSceneRoot, bestSkeleton);
		kHistoryMutex.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds((int) kSkeletonDelayTime));
	} while (kContinueUpdating);
}

//assumes that mutex lock/unlock is handled by the calling function, otherwise
//there will be concurrency problems.
void jester::LeapCarmineFuser::advanceHistoryFrame() {
	kJointHistory[kNewestInfo].timestamp = std::clock();
	kNewestInfo = (kNewestInfo + 1) % HistoryLength;

	kJointHistory[kNewestInfo].jointData.clear();
	kJointHistory[kNewestInfo].carmineData = false;
	kJointHistory[kNewestInfo].leapData = false;
}

void jester::LeapCarmineFuser::checkTimeout() {
	do {
		std::this_thread::sleep_for(std::chrono::milliseconds(jester::LeapCarmineFuser::DataFrameExpirationMs));

		kHistoryMutex.lock();
		advanceHistoryFrame();
		kHistoryMutex.unlock();
	} while (kContinueUpdating);
}

void jester::LeapCarmineFuser::launchThreads() {
	if (kContinueUpdating == false) {
		kSkeletonUpdateThread = new std::thread(&jester::LeapCarmineFuser::updateSkeleton, this);
		kFrameAdvanceThread = new std::thread(&jester::LeapCarmineFuser::checkTimeout, this);
		kContinueUpdating = true;
	}
}
