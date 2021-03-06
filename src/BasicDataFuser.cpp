#include "BasicDataFuser.h"

const int jester::BasicDataFuser::HistoryLength = 50;
const int jester::BasicDataFuser::UpdateHertz = 60;
const int jester::BasicDataFuser::MaxRetrievalDistance = 10;
const float jester::BasicDataFuser::FusionLowerThreshold = 0.01;

jester::BasicDataFuser::BasicDataFuser(FilterFactory *filterFactory) : DataFusionModule(filterFactory) {
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
		for (unsigned int sensorIx = 0; sensorIx < kSensors.size(); sensorIx++) {
			std::map<Bone::BoneId, BoneFusionData> cleanMap;
			kBoneHistory[historyIx].rawBoneData[kSensors[sensorIx]] = cleanMap;
		}
	}
}

void jester::BasicDataFuser::initializeFilters() {
	if (kFilterFactory != NULL) {
		for (unsigned int sensorIx = 0; sensorIx < kSensors.size(); sensorIx++) {
			std::map<Bone::BoneId, Filter*> filtersForSensor;

			for (int i = 0; i < Bone::BONE_COUNT; i++) {
				filtersForSensor.insert(std::pair<Bone::BoneId, Filter*>(Bone::intToBoneId(i), kFilterFactory->createFilter()));
			}
			kFilters.insert(std::pair<Sensor *, std::map<Bone::BoneId, Filter*>>(kSensors[sensorIx], filtersForSensor));
		}
	}
}

void jester::BasicDataFuser::startFusion() {
	initializeHistory();
	initializeFilters();

	kContinueUpdating = true;
	kSkeletonUpdateThread = new std::thread(&jester::BasicDataFuser::updateSkeleton, this);
}

void jester::BasicDataFuser::newData(Sensor *sensor, std::map<Bone::BoneId, BoneFusionData> data) {
	/*if (data.find(Bone::RADIUS_L) != data.end()) {
		glm::vec3 lwPos = getEndpointFromBoneData(data.find(Bone::RADIUS_L)->second);
		lwPos = glm::vec3(sensor->getWorldTransform() * glm::vec4(lwPos, 1));
		if (sensor == kSensors[1]) {
			printf("Leap B: %2.4f %2.4f %2.4f\n", lwPos.x, lwPos.y, lwPos.z);
		} else {
			printf("Carmine B: %2.4f %2.4f %2.4f\n", lwPos.x, lwPos.y, lwPos.z);
		}
	}*/

	if (!checkSensorRegistration(sensor))
		return;

	data = filterData(sensor, data);

	kHistoryMutex.lock();
	kBoneHistory[kNewestInfo].rawBoneData[sensor] = data;
	kHistoryMutex.unlock();
}

void jester::BasicDataFuser::newData(Sensor *sensor, std::map<Bone::JointId, JointFusionData> data) {
	newData(sensor, jointDataToBoneData(sensor, data));
}

void jester::BasicDataFuser::addSensor(Sensor *sensor, std::map<Bone::BoneId, double> boneConfidence) {
	kSensors.push_back(sensor);
	kSensorConfidences.insert(std::pair<Sensor *, std::map<Bone::BoneId, double>>(sensor, boneConfidence));
}

std::map<jester::Bone::BoneId, jester::BoneFusionData> jester::BasicDataFuser::filterData(Sensor *sensor, std::map<Bone::BoneId, BoneFusionData> data) {
	std::map<Bone::BoneId, BoneFusionData> filteredData;
	std::map<Bone::BoneId, Filter*> filtersForSensor = kFilters.find(sensor)->second;

	for (int i = 0; i < Bone::BONE_COUNT; i++) {
		std::map<Bone::BoneId, Filter*>::iterator filterIt = filtersForSensor.find(Bone::intToBoneId(i));
		std::map<Bone::BoneId, BoneFusionData>::iterator dataIt = data.find(Bone::intToBoneId(i));

		if (filterIt != filtersForSensor.end() && dataIt != data.end()) {
			filteredData[Bone::intToBoneId(i)] = filterIt->second->update(sensor, dataIt->second);
		} else if (dataIt != data.end()) {
			filteredData[Bone::intToBoneId(i)] = dataIt->second;
		}
	}

	return filteredData;
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

		bestSkeleton = fuseBoneDataFromStartFrame(startFrame);

		kSceneRoot->beginSkeletonUpdate();
		setSkeletonFromBoneData(bestSkeleton);
		kSceneRoot->finishSkeletonUpdate();

		processingSec = (getWallTime() - processingStartTime);

		std::this_thread::sleep_for(std::chrono::milliseconds(std::max(0, (int) ((kSkeletonDelayTime - processingSec) * 1000.0))));
	} while (kContinueUpdating);
}

std::map<jester::Bone::BoneId, jester::BoneFusionData> jester::BasicDataFuser::fuseBoneDataFromStartFrame(int frame) {
	std::map<Bone::BoneId, BoneFusionData> bestData;
	std::map<Sensor *, std::map<Bone::BoneId, BoneFusionData>> sensorData;

	for (unsigned int curSensor = 0; curSensor < kSensors.size(); curSensor++) {
		std::map<Bone::BoneId, double> curSensorConfs = kSensorConfidences.find(kSensors[curSensor])->second;
		std::map<Bone::BoneId, BoneFusionData> curSensorBoneMap;

		for (int curBone = Bone::ROOT; curBone < Bone::BONE_COUNT; curBone++) {
			if (curSensorConfs.find(Bone::intToBoneId(curBone)) != curSensorConfs.end()) {
				BoneFusionData foundData = findSensorBoneFromFrame(frame, kSensors[curSensor], Bone::intToBoneId(curBone));

				if (foundData.confidence >= 0.0)
					curSensorBoneMap.insert(std::pair<Bone::BoneId, BoneFusionData>(Bone::intToBoneId(curBone), foundData));
			}
		}
		sensorData.insert(std::pair<Sensor *, std::map<Bone::BoneId, BoneFusionData>>(kSensors[curSensor], curSensorBoneMap));
	}

	handleSwaps(&sensorData);

	//merge the sensor data sets
	for (std::map<Sensor *, std::map<Bone::BoneId, BoneFusionData>>::iterator dataIt = sensorData.begin();
			dataIt != sensorData.end(); dataIt++) {
		insertBoneDataIntoMap(&bestData, dataIt->first, dataIt->second);
	}

	return bestData;
}

void jester::BasicDataFuser::handleSwaps(std::map<Sensor *, std::map<Bone::BoneId, BoneFusionData>> *dataset) {
	//Dont do anything
}

void jester::BasicDataFuser::insertBoneDataIntoMap(std::map<Bone::BoneId, BoneFusionData> *map, Sensor* sensor, std::map<Bone::BoneId, BoneFusionData> bones) {
	glm::mat4 worldTransform = sensor->getWorldTransform();

	for (std::map<Bone::BoneId, BoneFusionData>::iterator it = bones.begin(); it != bones.end(); it++) {
		BoneFusionData fusedData = it->second;
		std::map<Bone::BoneId, BoneFusionData>::iterator fusionSlot = map->find(it->first);

		fusedData.position = glm::vec3(worldTransform * glm::vec4(fusedData.position, 1.0));
		fusedData.orientation = glm::quat_cast(worldTransform * glm::mat4_cast(fusedData.orientation));

		if (fusionSlot != map->end()) {
			BoneFusionData existingData = fusionSlot->second;

			mergeKnownBoneWithAssumedBone(&existingData, &fusedData);

			std::map<Bone::BoneId, double>::iterator confIt = kSensorConfidences.find(sensor)->second.find(it->first);
			double interpScalar;

			if (confIt != kSensorConfidences.find(sensor)->second.end())
				interpScalar = confIt->second;
			else
				interpScalar = 1.0;

			float denom = existingData.confidence + (fusedData.confidence * interpScalar);
			float interpolationFactor = (fusedData.confidence * interpScalar) /
					(denom > 0.001f ? denom : 0.001f);

			glm::vec3 existingEndpoint = getEndpointFromBoneData(existingData);
			glm::vec3 newEndpoint = getEndpointFromBoneData(fusedData);
			glm::vec3 interpEndpoint = glm::mix(newEndpoint, existingEndpoint, interpolationFactor);
			fusedData.position = glm::mix(fusedData.position, existingData.position, interpolationFactor);
			fusedData.orientation = getQuaternionFromEndpoints(fusedData.position, interpEndpoint);
			fusedData.length = glm::distance(interpEndpoint, fusedData.position);

			fusedData.confidence = interpolationFactor;
		}
		map->operator[](it->first) = fusedData;
	}
}

void jester::BasicDataFuser::mergeKnownBoneWithAssumedBone(BoneFusionData *boneA, BoneFusionData *boneB) {
	if (boneA->confidence < FusionLowerThreshold &&
			boneB->confidence > FusionLowerThreshold) {
		glm::vec3 endpoint = getEndpointFromBoneData(*boneA);
		boneA->orientation = getQuaternionFromEndpoints(boneB->position, endpoint);
		boneA->confidence = boneB->confidence;
		boneA->length = glm::distance(boneB->position, endpoint);
		boneA->position = boneB->position;
	} else if (boneB->confidence < FusionLowerThreshold &&
			boneA->confidence > FusionLowerThreshold) {
		glm::vec3 endpoint = getEndpointFromBoneData(*boneB);
		boneB->orientation = getQuaternionFromEndpoints(boneA->position, endpoint);
		boneB->confidence = boneA->confidence;
		boneB->length = glm::distance(boneA->position, endpoint);
		boneB->position = boneA->position;
	}
}

jester::BoneFusionData jester::BasicDataFuser::findSensorBoneFromFrame(int frame, Sensor *sensor, Bone::BoneId boneId) {
	BoneFusionData foundBone;
	int curInfo = frame;
	int backSteps = 0;
	std::map<Bone::BoneId, BoneFusionData> curMap;
	std::map<Bone::BoneId, BoneFusionData>::iterator it;

	foundBone.confidence = -1.0f;

	do {
		curMap = kBoneHistory[curInfo].rawBoneData.find(sensor)->second;
		it = curMap.find(boneId);
		curInfo -= 1;
		curInfo = (curInfo % HistoryLength + HistoryLength) % HistoryLength;
		backSteps++;
	} while (backSteps <= MaxRetrievalDistance && it == curMap.end());

	if (backSteps <= MaxRetrievalDistance && it != curMap.end()) {
		foundBone = it->second;
	}
	return foundBone;
}

//assumes that mutex lock/unlock is handled by the calling function, otherwise
//there will be concurrency problems.
void jester::BasicDataFuser::advanceHistoryFrame() {
	kBoneHistory[kNewestInfo].timestamp = getWallTime();
	kNewestInfo = (kNewestInfo + 1) % HistoryLength;

	for (unsigned int sensorIx = 0; sensorIx < kSensors.size(); sensorIx++) {
		std::map<Bone::BoneId, BoneFusionData> cleanMap;
		kBoneHistory[kNewestInfo].rawBoneData[kSensors[sensorIx]] = cleanMap;
	}
}

bool jester::BasicDataFuser::checkSensorRegistration(Sensor *sensor) {
	for (unsigned int i = 0; i < kSensors.size(); i++)
		if (kSensors[i] == sensor)
			return true;

	printf("Unregistered sensor reporting data! Ignoring...\n");
	return false;
}

jester::BasicDataFuser::~BasicDataFuser() {
	kContinueUpdating = false;
	kSkeletonUpdateThread->join();
}

jester::BasicDataFuserFactory::BasicDataFuserFactory(FilterFactory *filterFactory) : DataFusionModuleFactory(filterFactory) {

}

jester::DataFusionModule* jester::BasicDataFuserFactory::createFusionModule() {
	return new BasicDataFuser(kFilterFactory);
}
