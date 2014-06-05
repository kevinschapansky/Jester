#include "BoneDoubleExponentialFilter.h"
#include "DataFusionModule.h"

jester::BoneFusionData jester::BoneDoubleExponentialFilter::update(Sensor *sensor, BoneFusionData boneData) {
	BoneFusionData filteredData = boneData;
	glm::vec3 boneStart = boneData.position;
	glm::vec3 boneEnd = DataFusionModule::getEndpointFromBoneData(boneData);
	glm::vec3 filteredStart;
	glm::vec3 filteredEnd;

	if (kT0) {
		filteredStart = boneStart;
		filteredEnd = boneEnd;
		kT0 = false;
	} else if (kT1) {
		filteredStart = boneStart;
		kTrendStart = boneStart - kSmoothedStart;

		filteredEnd = boneEnd;
		kTrendEnd = boneEnd - kSmoothedEnd;
		kT1 = false;
	} else {
		filteredStart = kAlpha * boneStart + (1.0f - kAlpha) * (kSmoothedStart - kTrendStart);
		kTrendStart = kBeta * (filteredStart - kSmoothedStart) + (1.0f - kBeta) * kTrendStart;

		filteredEnd = kAlpha * boneEnd + (1.0f - kAlpha) * (kSmoothedEnd - kTrendEnd);
		kTrendEnd = kBeta * (filteredEnd - kSmoothedEnd) + (1.0f - kBeta) * kTrendEnd;
	}
	kSmoothedStart = filteredStart;
	kSmoothedEnd = filteredEnd;

	filteredData.orientation = DataFusionModule::getQuaternionFromEndpoints(kSmoothedStart, kSmoothedEnd);
	filteredData.position = kSmoothedStart;

	return filteredData;
}

jester::BoneDoubleExponentialFilter::BoneDoubleExponentialFilter(float dataSmoothingFactor, float rateSmoothingFactor) {
	kAlpha = dataSmoothingFactor;
	kBeta = rateSmoothingFactor;

	kT0 = true;
	kT1 = true;
}

jester::Filter* jester::DoubleExponentialFilterFactory::createFilter() {
	return new BoneDoubleExponentialFilter(kAlpha, kBeta);
}

jester::DoubleExponentialFilterFactory::DoubleExponentialFilterFactory(float dataSmoothingFactor, float rateSmoothingFactor) {
	kAlpha = dataSmoothingFactor;
	kBeta = rateSmoothingFactor;
}

jester::DoubleExponentialFilterFactory::~DoubleExponentialFilterFactory() {

}
