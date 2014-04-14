#include "LeapCarmineFuser.h"

const float jester::LeapCarmineFuser::SwitchDelta = 0.1f;
const int jester::LeapCarmineFuser::HistoryLength = 10;
const int jester::LeapCarmineFuser::UpdateHertz = 100;
const int jester::LeapCarmineFuser::DataFrameExpirationMs = 200;
const int jester::LeapCarmineFuser::JointsPerHand = 10;
const int jester::LeapCarmineFuser::MaxRetrievalDistance = 4;

void jester::LeapCarmineFuser::newData(Sensor *sensor, BoneFusionData data[Bone::BONE_COUNT]) {
	printf("This implementation of data fusion is not expecting raw bone fusion data.\nExiting...\n");
	exit(EXIT_FAILURE);
}

void jester::LeapCarmineFuser::newData(Sensor *sensor, JointFusionData data[Bone::JOINT_COUNT]) {
	JointFusionData *jointData;

	kHistoryMutex.lock();
	if (kNewestInfo == 1)
		advanceHistoryFrame();
	jointData = kJointHistory[kNewestInfo].jointData;

	if (sensor == kLeap) {
		if (kJointHistory[kNewestInfo].carmineData) {
			fuseHandPositions(jointData, data);
		} else {
			insertLeapJoints(data);
		}
		kJointHistory[kNewestInfo].leapData = true;
	} else if (sensor == kCarmine) {
		if (kJointHistory[kNewestInfo].leapData) {
			fuseHandPositions(data, jointData);
		} else {
			insertCarmineJoints(data);
		}
		kJointHistory[kNewestInfo].carmineData = true;
	} else {
		printf("Unregistered sensor sending data! Ignoring...\n");
	}
	//if the frame is now full, advance it
	if (kJointHistory[kNewestInfo].carmineData && kJointHistory[kNewestInfo].leapData)
		advanceHistoryFrame();
	kHistoryMutex.unlock();
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
	kNewestInfo = -1;
	kInitClock = std::clock();
	kContinueUpdating = false;
	kSkeletonDelayTime = 1.f / UpdateHertz * 1000.f;
	kCarmine = kLeap = NULL;
	kSceneRoot = NULL;
	kJointHistory = new JointPositionHistory[HistoryLength];

	for (int historyIx = 0; historyIx < HistoryLength; historyIx++) {
		for (int jointIx = 0; jointIx < Bone::JOINT_COUNT; jointIx++) {
			kJointHistory[historyIx].jointData[jointIx].position = NULL;
			kJointHistory[historyIx].jointData[jointIx].confidence = 0;
			kJointHistory[historyIx].jointData[jointIx].id = Bone::intToJointId(jointIx);
		}
	}
	kSkeletonUpdateThread = kFrameAdvanceThread = NULL;
}

jester::LeapCarmineFuser::~LeapCarmineFuser() {
	kContinueUpdating = false;
	kSkeletonUpdateThread->join();
	kFrameAdvanceThread->join();
}

void jester::LeapCarmineFuser::updateSkeleton() {
	JointFusionData bestSkeleton[Bone::JOINT_COUNT];

	do {
		if (kNewestInfo == -1) {
			std::this_thread::sleep_for(std::chrono::milliseconds((int) kSkeletonDelayTime));
			continue;
		}

		kHistoryMutex.lock();
		for (int jointId = 0; jointId < Bone::JOINT_COUNT; jointId++) {
			int curInfo = kNewestInfo;

			while (abs(kNewestInfo - curInfo) < MaxRetrievalDistance &&
					kJointHistory[curInfo].jointData[jointId].position == NULL) {
				curInfo -= 1;
				curInfo = (curInfo % HistoryLength + HistoryLength) % HistoryLength;
			}
			bestSkeleton[jointId] = kJointHistory[curInfo].jointData[jointId];
		}
		setSkeletonFromJoints(kSceneRoot, bestSkeleton);
		kHistoryMutex.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds((int) kSkeletonDelayTime));
	} while (kContinueUpdating);
}

//assumes that mutex lock/unlock is handled by the calling function, otherwise
//there will be concurrency problems.
void jester::LeapCarmineFuser::advanceHistoryFrame() {
	if (kNewestInfo != -1)
		kJointHistory[kNewestInfo].timestamp = std::clock();

	kNewestInfo = (kNewestInfo + 1) % HistoryLength;

	for (int jointIx = 0; jointIx < Bone::JOINT_COUNT; jointIx++) {
		if (kJointHistory[kNewestInfo].jointData[jointIx].position != NULL) {
			delete kJointHistory[kNewestInfo].jointData[jointIx].position;
			kJointHistory[kNewestInfo].jointData[jointIx].position = NULL;
		}
	}
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

//assumes that mutex lock/unlock is handled by the calling function, otherwise
//there will be concurrency problems.
void jester::LeapCarmineFuser::fuseHandPositions(JointFusionData *carmineJoints, JointFusionData *leapJoints) {
	float noSwitchDelta = glm::distance(*(carmineJoints[Bone::WRIST_L].position), *(leapJoints[Bone::WRIST_L].position)) +
			glm::distance(*(carmineJoints[Bone::WRIST_R].position), *(leapJoints[Bone::WRIST_R].position));
	float switchDelta = glm::distance(*(carmineJoints[Bone::WRIST_L].position), *(leapJoints[Bone::WRIST_R].position)) +
				glm::distance(*(carmineJoints[Bone::WRIST_R].position), *(leapJoints[Bone::WRIST_L].position));


	//if we see an improvement for switching the hands
	if (noSwitchDelta - switchDelta > SwitchDelta) {
		JointFusionData tempJoints[JointsPerHand * 2];

		memcpy(tempJoints, leapJoints, sizeof(JointFusionData) * JointsPerHand * 2);
		memcpy(&leapJoints[Bone::DISTAL_L_1], &tempJoints[Bone::DISTAL_R_1], sizeof(JointFusionData) * 5);
		memcpy(&leapJoints[Bone::DISTAL_R_1], &tempJoints[Bone::DISTAL_L_1], sizeof(JointFusionData) * 5);
		memcpy(&leapJoints[Bone::METACARPO_L_1], &tempJoints[Bone::METACARPO_R_1], sizeof(JointFusionData) * 5);
		memcpy(&leapJoints[Bone::METACARPO_R_1], &tempJoints[Bone::METACARPO_L_1], sizeof(JointFusionData) * 5);
		leapJoints[Bone::WRIST_L] = tempJoints[Bone::WRIST_R];
		leapJoints[Bone::WRIST_R] = tempJoints[Bone::WRIST_L];
	}
	insertLeapJoints(leapJoints);
	insertCarmineJoints(carmineJoints);
}

//Assumes the the finger joints never have anything inserted in-between them in the Bone::JointId enum
void jester::LeapCarmineFuser::insertLeapJoints(JointFusionData *leapJoints) {
	JointFusionData *positions = kJointHistory[kNewestInfo].jointData;

	printf("insertLeapJoints::Begin\n");
	fflush(stdout);
	for (int jointIx = Bone::DISTAL_L_1; jointIx <= Bone::METACARPO_R_5; jointIx++) {
		if (leapJoints[jointIx].position != NULL) {
			positions[jointIx] = leapJoints[jointIx];
			positions[jointIx].position = new glm::vec3(kLeap->getWorldTransform() * glm::vec4(*(leapJoints[jointIx].position), 1.f));
		}
	}

	for (int jointIx = Bone::WRIST_L; jointIx <= Bone::WRIST_R; jointIx++) {
		if (leapJoints[jointIx].position != NULL) {
			positions[jointIx] = leapJoints[jointIx];
			positions[jointIx].position = new glm::vec3(kLeap->getWorldTransform() * glm::vec4(*(leapJoints[jointIx].position), 1.f));
		}
	}
}

//Assumes that the upper torso joints (the carmine-sensed joints) all lie between HEAD and WRIST_R in Bone::JointId enum
void jester::LeapCarmineFuser::insertCarmineJoints(JointFusionData *carmineJoints) {
	JointFusionData *positions = kJointHistory[kNewestInfo].jointData;

	for (int jointIx = Bone::HEAD; jointIx <= Bone::WRIST_R; jointIx++) {
		if (carmineJoints[jointIx].position != NULL) {
			positions[jointIx] = carmineJoints[jointIx];
			positions[jointIx].position = new glm::vec3(kLeap->getWorldTransform() * glm::vec4(*(carmineJoints[jointIx].position), 1.f));
		}
	}
}

void jester::LeapCarmineFuser::launchThreads() {
	if (kContinueUpdating == false) {
		kSkeletonUpdateThread = new std::thread(&jester::LeapCarmineFuser::updateSkeleton, this);
		kFrameAdvanceThread = new std::thread(&jester::LeapCarmineFuser::checkTimeout, this);
		kContinueUpdating = true;
	}
}
