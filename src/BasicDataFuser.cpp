#include "BasicDataFuser.h"

const int jester::BasicDataFuser::HistoryLength = 50;
const int jester::BasicDataFuser::UpdateHertz = 30;
const int jester::BasicDataFuser::MaxRetrievalDistance = 10;
const float jester::BasicDataFuser::FusionLowerThreshold = 0.01;

jester::BasicDataFuser::BasicDataFuser() {
	kNewestInfo = 0;
	kInitClock = getWallTime();
	kContinueUpdating = false;
	kSkeletonDelayTime = 1.f / UpdateHertz;
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
	double processingStartTime;
	double processingSec;
	int startFrame;

	do {
		processingStartTime = getWallTime();
		bestSkeleton.clear();

		kHistoryMutex.lock();
		startFrame = kNewestInfo;
		advanceHistoryFrame();
		kHistoryMutex.unlock();

		fuseBoneDataInFrame(startFrame);

		bestSkeleton = findBestSkeletonFromFrame(startFrame);

		kSceneRoot->beginSkeletonUpdate();
		setSkeletonFromBoneData(bestSkeleton);
		kSceneRoot->finishSkeletonUpdate();

		processingSec = (getWallTime() - processingStartTime);

		std::this_thread::sleep_for(std::chrono::milliseconds(std::max(0, (int) ((kSkeletonDelayTime - processingSec) * 1000.0))));
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

			bool merged = false;

			if (existingData.confidence < FusionLowerThreshold &&
					fusedData.confidence > FusionLowerThreshold) {
				glm::vec3 endpoint = glm::normalize(glm::vec3(glm::mat4_cast(existingData.orientation) * glm::vec4(0, 0, 1, 0))) * existingData.length;
				existingData.orientation = getQuaternionFromEndpoints(fusedData.position, endpoint);
				existingData.confidence = fusedData.confidence;
				merged = true;
			} else if (fusedData.confidence < FusionLowerThreshold &&
					existingData.confidence > FusionLowerThreshold) {
				glm::vec3 endpoint = glm::normalize(glm::vec3(glm::mat4_cast(fusedData.orientation) * glm::vec4(0, 0, 1, 0))) * fusedData.length;
				fusedData.orientation = getQuaternionFromEndpoints(existingData.position, endpoint);
				fusedData.confidence = existingData.confidence;
				merged = true;
			}

			float interpolationFactor = fusedData.confidence / (existingData.confidence + fusedData.confidence);

			fusedData.orientation = glm::mix(fusedData.orientation, existingData.orientation, interpolationFactor);
			fusedData.position = glm::mix(fusedData.position, existingData.position, interpolationFactor);
			fusedData.length = (fusedData.length + existingData.length) / 2.0;

			if (!merged) {
				fusedData.confidence = interpolationFactor;
			} else {
				fusedData.confidence = 0.1337;
			}
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

			if (Bone::intToBoneId(boneId) == Bone::RADIUS_L) {
				printf("Rad Conf: %f\n", kBoneHistory[curInfo].fusedBoneData.find(Bone::intToBoneId(boneId))->second.confidence);
			}

			bestSkeleton.insert(*(kBoneHistory[curInfo].fusedBoneData.find(Bone::intToBoneId(boneId))));
		}
	}
	return bestSkeleton;
}

//assumes that mutex lock/unlock is handled by the calling function, otherwise
//there will be concurrency problems.
void jester::BasicDataFuser::advanceHistoryFrame() {
	kBoneHistory[kNewestInfo].timestamp = getWallTime();
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
