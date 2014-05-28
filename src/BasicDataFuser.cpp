#include "BasicDataFuser.h"

const int jester::BasicDataFuser::HistoryLength = 50;
const int jester::BasicDataFuser::UpdateHertz = 100;
const int jester::BasicDataFuser::DataFrameExpirationMs = 200;
const int jester::BasicDataFuser::DataFrameExpirationCheckMs = 10;
const int jester::BasicDataFuser::MaxRetrievalDistance = 10;

jester::BasicDataFuser::BasicDataFuser() {
	kNewestInfo = 0;
	kInitClock = std::clock();
	kContinueUpdating = false;
	kSkeletonDelayTime = 1.f / UpdateHertz * 1000.f;
	kJointHistory = NULL;
	kSensorCount = 0;

	kSkeletonUpdateThread = kFrameAdvanceThread = NULL;
}

void jester::BasicDataFuser::initializeHistory() {
	kJointHistory = new JointPositionHistory[HistoryLength];

	for (int historyIx = 0; historyIx < HistoryLength; historyIx++) {
		kJointHistory[historyIx].jointData.clear();
		kJointHistory[historyIx].sensorDataPresent = new bool[kSensorCount];

		for (int i = 0; i < kSensorCount; i++)
			kJointHistory[historyIx].sensorDataPresent[i] = false;
	}
}

void jester::BasicDataFuser::initializeFilters() {

}

void jester::BasicDataFuser::startFusion() {
	initializeHistory();
	initializeFilters();

	kContinueUpdating = true;
	kSkeletonUpdateThread = new std::thread(&jester::BasicDataFuser::updateSkeleton, this);
	kFrameAdvanceThread = new std::thread(&jester::BasicDataFuser::checkTimeout, this);
}

void jester::BasicDataFuser::newData(Sensor *sensor, std::map<Bone::BoneId, BoneFusionData> data) {
	kHistoryMutex.lock();

	kDataQueue.push(std::pair<Sensor*, std::map<Bone::BoneId, BoneFusionData>>(sensor, data));

	kHistoryMutex.unlock();
}

void jester::BasicDataFuser::newData(Sensor *sensor, std::map<Bone::JointId, JointFusionData> data) {
	newData(sensor, jointDataToBoneData(sensor, data));
}

void jester::BasicDataFuser::addSensor(Sensor* sensor) {
	kSensors.insert(std::pair<int, Sensor*>(kSensorCount, sensor));
	kSensorCount++;
}

void jester::BasicDataFuser::insertJoints(std::map<Bone::JointId, JointFusionData> joints, glm::mat4 jointWorldTransform) {
	/*for (int i = 0; i < Bone::JOINT_COUNT; i++) {
		if (joints.find(Bone::intToJointId(i)) != joints.end()) {
			std::pair<Bone::JointId, JointFusionData> joint = *(joints.find(Bone::intToJointId(i)));
			std::map<Bone::JointId, JointParticleFilter *>::iterator filter = kFilters.find(Bone::intToJointId(i));

			if (filter != kFilters.end())
				joint.second.position = (*filter).second->update(glm::vec3(jointWorldTransform * glm::vec4(joint.second.position, 1.f)));
			else
				joint.second.position = glm::vec3(jointWorldTransform * glm::vec4(joint.second.position, 1.f));
			kJointHistory[kNewestInfo].jointData.insert(joint);
		}
	}*/
}

void jester::BasicDataFuser::updateSkeleton() {
	std::map<Bone::JointId, JointFusionData> bestSkeleton;

	do {/*
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
		std::this_thread::sleep_for(std::chrono::milliseconds((int) kSkeletonDelayTime));*/
	} while (kContinueUpdating);
}

//assumes that mutex lock/unlock is handled by the calling function, otherwise
//there will be concurrency problems.
void jester::BasicDataFuser::advanceHistoryFrame() {
	kJointHistory[kNewestInfo].timestamp = std::clock();
	kNewestInfo = (kNewestInfo + 1) % HistoryLength;

	kJointHistory[kNewestInfo].jointData.clear();

	for (int i = 0; i < kSensorCount; i++)
		kJointHistory[kNewestInfo].sensorDataPresent[i] = false;
}

void jester::BasicDataFuser::checkTimeout() {
	do {
		std::this_thread::sleep_for(std::chrono::milliseconds(jester::BasicDataFuser::DataFrameExpirationMs));

		kHistoryMutex.lock();
		advanceHistoryFrame();
		kHistoryMutex.unlock();
	} while (kContinueUpdating);
}

jester::BasicDataFuser::~BasicDataFuser() {
	kContinueUpdating = false;
	kSkeletonUpdateThread->join();
	kFrameAdvanceThread->join();

	for (int i = 0; i < HistoryLength; i++)
		delete[] kJointHistory[i].sensorDataPresent;
	delete[] kJointHistory;
}

jester::DataFusionModule* jester::BasicDataFuserFactory::CreateFusionModule() {
	return new BasicDataFuser();
}
