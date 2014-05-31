#include "BasicDataFuser.h"

const int jester::BasicDataFuser::HistoryLength = 50;
const int jester::BasicDataFuser::UpdateHertz = 100;
const int jester::BasicDataFuser::MaxRetrievalDistance = 10;

jester::BasicDataFuser::BasicDataFuser() {
	kNewestInfo = 0;
	kInitClock = std::clock();
	kContinueUpdating = false;
	kSkeletonDelayTime = 1.f / UpdateHertz * 1000.f;
	kBoneHistory = NULL;

	kSkeletonUpdateThread = NULL;
}

void jester::BasicDataFuser::initializeHistory() {
	kBoneHistory = new BonePositionHistory[HistoryLength];

	for (int historyIx = 0; historyIx < HistoryLength; historyIx++) {
		kBoneHistory[historyIx].fusedBoneData.clear();
		kBoneHistory[historyIx].rawBoneData.clear();
	}
}

void jester::BasicDataFuser::initializeFilters() {

}

void jester::BasicDataFuser::startFusion() {
	initializeHistory();
	initializeFilters();

	kContinueUpdating = true;
	kSkeletonUpdateThread = new std::thread(&jester::BasicDataFuser::updateSkeleton, this);
}

void jester::BasicDataFuser::newData(Sensor *sensor, std::map<Bone::BoneId, BoneFusionData> data) {
	kHistoryMutex.lock();
	kBoneHistory[kNewestInfo].rawBoneData[sensor] = data;
	kHistoryMutex.unlock();
}

void jester::BasicDataFuser::newData(Sensor *sensor, std::map<Bone::JointId, JointFusionData> data) {
	newData(sensor, jointDataToBoneData(sensor, data));
}

void jester::BasicDataFuser::addSensor(Sensor* sensor) {
	kSensors.push_back(sensor);
}

void jester::BasicDataFuser::updateSkeleton() {
	std::map<Bone::BoneId, BoneFusionData> bestSkeleton;
	std::clock_t processingStartTime;
	float processingMs;
	int startFrame;

	do {
		processingStartTime = std::clock();
		bestSkeleton.clear();

		kHistoryMutex.lock();
		startFrame = kNewestInfo;
		advanceHistoryFrame();
		kHistoryMutex.unlock();

		fuseBoneDataInFrame(startFrame);

		bestSkeleton = findBestSkeletonFromFrame(startFrame);

		setSkeletonFromBoneData(bestSkeleton);

		processingMs = (std::clock() - processingStartTime) / CLOCKS_PER_SEC / 1000;
		std::this_thread::sleep_for(std::chrono::milliseconds(std::max(0, (int) (kSkeletonDelayTime - processingMs))));
	} while (kContinueUpdating);
}

void jester::BasicDataFuser::fuseBoneDataInFrame(int frame) {
	for (std::map<Sensor*, std::map<Bone::BoneId, BoneFusionData>>::iterator it = kBoneHistory[frame].rawBoneData.begin();
			it != kBoneHistory[frame].rawBoneData.end(); it++) {
		insertBoneDataIntoFrame(frame, it->first, it->second);
	}
}

void jester::BasicDataFuser::insertBoneDataIntoFrame(int frame, Sensor* sensor, std::map<Bone::BoneId, BoneFusionData> bones) {
	glm::mat4 worldTransform = sensor->getWorldTransform();

	for (std::map<Bone::BoneId, BoneFusionData>::iterator it = bones.begin(); it != bones.end(); it++) {
		BoneFusionData fusedData = it->second;
		std::map<Bone::BoneId, BoneFusionData>::iterator fusionSlot = kBoneHistory[frame].fusedBoneData.find(it->first);

		fusedData.position = glm::vec3(worldTransform * glm::vec4(fusedData.position, 1.0));
		fusedData.orientation = glm::quat_cast(worldTransform * glm::mat4_cast(fusedData.orientation));

		if (fusionSlot != kBoneHistory[frame].fusedBoneData.end()) {
			BoneFusionData existingData = fusionSlot->second;
			float interpolationFactor = fusedData.confidence / (existingData.confidence + fusedData.confidence);

			fusedData.orientation = glm::mix(fusedData.orientation, existingData.orientation, interpolationFactor);
			fusedData.position = glm::mix(fusedData.position, existingData.position, interpolationFactor);
			fusedData.length = (fusedData.length + existingData.length) / 2.0;
			fusedData.confidence = interpolationFactor;
		}
		kBoneHistory[frame].fusedBoneData[it->first] = fusedData;
	}
}

std::map<jester::Bone::BoneId, jester::BoneFusionData> jester::BasicDataFuser::findBestSkeletonFromFrame(int frame) {
	std::map<Bone::BoneId, BoneFusionData> bestSkeleton;

	for (int boneId = 0; boneId < Bone::BONE_COUNT; boneId++) {
		int curInfo = frame;
		int backSteps = 0;

		while (backSteps < MaxRetrievalDistance &&
				!(kBoneHistory[curInfo].fusedBoneData.count(Bone::intToBoneId(boneId)))) {
			curInfo -= 1;
			curInfo = (curInfo % HistoryLength + HistoryLength) % HistoryLength;
			backSteps++;
		}

		if (kBoneHistory[curInfo].fusedBoneData.count(Bone::intToBoneId(boneId))) {
			bestSkeleton.insert(*(kBoneHistory[curInfo].fusedBoneData.find(Bone::intToBoneId(boneId))));
		}
	}
	return bestSkeleton;
}

//assumes that mutex lock/unlock is handled by the calling function, otherwise
//there will be concurrency problems.
void jester::BasicDataFuser::advanceHistoryFrame() {
	kBoneHistory[kNewestInfo].timestamp = std::clock();
	kNewestInfo = (kNewestInfo + 1) % HistoryLength;

	kBoneHistory[kNewestInfo].fusedBoneData.clear();
	kBoneHistory[kNewestInfo].rawBoneData.clear();
}

jester::BasicDataFuser::~BasicDataFuser() {
	kContinueUpdating = false;
	kSkeletonUpdateThread->join();
}

jester::DataFusionModule* jester::BasicDataFuserFactory::CreateFusionModule() {
	return new BasicDataFuser();
}
