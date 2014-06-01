#include "BoneParticleFilter.h"

#define PI 3.141592653

jester::FusionBone jester::BoneParticleFilter::update(jester::FusionBone bone) {
	glm::dvec3 mmStartPosition(bone.getPosition() * 1000.f);
	glm::dvec3 mmEndPosition(bone.getPosition() + glm::vec3(1.0, 1.0, bone.getLength()));

	moveParticles();
	generateProbabilities(mmStartPosition, mmEndPosition);
	resampleParticles();

	glm::dvec3 start = getCurrentStartPosition();
	glm::dvec3 end = getCurrentEndPosition();

	return bone;
}

double jester::BoneParticleFilter::gaussianProbability(double mean, double stdDev, double x) {
	return exp(-1.0 * pow((mean - x), 2.0) / pow(stdDev, 2.0) / 2.0) / sqrt(2.0 * PI * pow(stdDev, 2.0));
}

void jester::BoneParticleFilter::initializeParticles() {
	std::default_random_engine generator;
	std::uniform_int_distribution<int> endDist(-kBone->getLength() * 1000.f, kBone->getLength() * 1000.f);
	std::uniform_int_distribution<int> angleDist(-100, 100);
	std::uniform_int_distribution<int> rateDist(-1500, 1500);

	for (int i = 0; i < kParticleCount; i++) {
		kParticles[i].movementRate = rateDist(generator) / 1000.0;
		kParticles[i].movementVector = glm::normalize(glm::vec3(
				(angleDist(generator) / 100.0),
				(angleDist(generator) / 100.0),
				(angleDist(generator) / 100.0)));
		kParticles[i].start = glm::vec3(0);
		kParticles[i].end = glm::vec3(
				endDist(generator),
				endDist(generator),
				endDist(generator));
	}

	kLastTimeStamp = std::clock();
}

void jester::BoneParticleFilter::resampleParticles() {
	BoneParticle *resampledParticles = new BoneParticle[kParticleCount];
	std::uniform_int_distribution<int> resampleDist(0, kParticleCount);
	std::uniform_int_distribution<int> betaDist(0, 1000);

	int index = resampleDist(kResampleGenerator);
	float beta = 0.0;
	float maxWeight = 0.0;
	float random;

	for (int i = 0; i < kParticleCount; i++)
		maxWeight = fmax(maxWeight, kParticles[i].probability);

	//resample into the new particle array
	for (int i = 0; i < kParticleCount; i++) {
		random = ((double) betaDist(kResampleGenerator)) / 1000.0;
		beta += random * 2.0 * maxWeight;
		while (beta > kParticles[index].probability) {
			beta -= kParticles[index].probability;
			index = (index + 1) % kParticleCount;
		}
		resampledParticles[i] = kParticles[index];
	}

	//delete old particles
	delete[] kParticles;
	kParticles = resampledParticles;
}

void jester::BoneParticleFilter::generateProbabilities(glm::dvec3 start, glm::dvec3 end) {
	double probSum = 0.0;

	for (int i = 0; i < kParticleCount; i++) {
		kParticles[i].probability = getProbability(kParticles[i], start, end);
		probSum += kParticles[i].probability;
	}

	for (int i = 0; i < kParticleCount; i++) {
		kParticles[i].probability = kParticles[i].probability / probSum;
		if (kBone->getType() == Bone::RADIUS_L) {
			printf("Particle %d: %f\n", i, kParticles[i].probability);
		}
	}
}

double jester::BoneParticleFilter::getProbability(BoneParticle particle, glm::dvec3 sensedStart, glm::dvec3 sensedEnd) {
	double startProb = gaussianProbability(0.0, kSensorNoise, glm::distance(sensedStart, particle.start));
	double endProb = gaussianProbability(0.0, kSensorNoise, glm::distance(sensedEnd, particle.end));

	return startProb * endProb;
}

void jester::BoneParticleFilter::moveParticles() {
	std::time_t currentTime = std::clock();
	double secondsElapsed = (double) (currentTime - kLastTimeStamp) / CLOCKS_PER_SEC;

	for (int i = 0; i < kParticleCount; i++) {
		kParticles[i].end += kParticles[i].movementVector * kParticles[i].movementRate * secondsElapsed;
	}
}

glm::dvec3 jester::BoneParticleFilter::getCurrentStartPosition() {
	glm::dvec3 position(0.0);

	for (int i = 0; i < kParticleCount; i++) {
		position = position + kParticles[i].start;
	}

	position = position / (double) kParticleCount;
	position = position / 1000.0;

	return position;
}

glm::dvec3 jester::BoneParticleFilter::getCurrentEndPosition() {
	glm::dvec3 position(0.0);

	for (int i = 0; i < kParticleCount; i++) {
		position = position + kParticles[i].end;
	}

	position = position / (double) kParticleCount;
	position = position / 1000.0;

	return position;
}

jester::BoneParticleFilter::BoneParticleFilter(FusionBone *bone, int particleCount, double sensorNoise) {
	kBone = bone;
	kParticleCount = particleCount;
	kParticles = new BoneParticle[kParticleCount];
	kSensorNoise = sensorNoise;

	initializeParticles();
}

jester::BoneParticleFilter::~BoneParticleFilter() {

}
