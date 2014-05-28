#include "JointParticleFilter.h"

#define PI 3.141592653

glm::vec3 jester::JointParticleFilter::update(glm::vec3 worldSpacePosition) {
	glm::vec3 parentSpacePosition = glm::vec3(glm::inverse(kBone->getWorldTransform()) * glm::vec4(worldSpacePosition, 1));
	glm::dvec3 mmPosition(parentSpacePosition * 1000.f);

	moveParticles();
	generateProbabilities(mmPosition);
	resampleParticles();
	return glm::vec3(kBone->getWorldTransform() * glm::vec4(glm::vec3(getCurrentFilterPosition()), 1));
}

double jester::JointParticleFilter::gaussianProbability(double mean, double stdDev, double x) {
	return exp(-1.0 * pow((mean - x), 2.0) / pow(stdDev, 2.0) / 2.0) / sqrt(2.0 * PI * pow(stdDev, 2.0));
}

void jester::JointParticleFilter::initializeParticles() {
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
		kParticles[i].position = glm::vec3(
				endDist(generator),
				endDist(generator),
				endDist(generator));
	}

	kLastTimeStamp = std::clock();
}

void jester::JointParticleFilter::resampleParticles() {
	JointParticle *resampledParticles = new JointParticle[kParticleCount];
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

void jester::JointParticleFilter::generateProbabilities(glm::dvec3 endpoint) {
	double probSum = 0.0;

	for (int i = 0; i < kParticleCount; i++) {
		kParticles[i].probability = getProbability(kParticles[i], endpoint);
		probSum += kParticles[i].probability;
	}

	for (int i = 0; i < kParticleCount; i++) {
		kParticles[i].probability = kParticles[i].probability / probSum;
		if (kBone->getType() == Bone::RADIUS_L) {
			printf("Particle %d: %f\n", i, kParticles[i].probability);
		}
	}
}

double jester::JointParticleFilter::getProbability(JointParticle particle, glm::dvec3 sensedPosition) {
	return gaussianProbability(0.0, kSensorNoise, glm::distance(sensedPosition, particle.position));
}

void jester::JointParticleFilter::moveParticles() {
	std::time_t currentTime = std::clock();
	double secondsElapsed = (double) (currentTime - kLastTimeStamp) / CLOCKS_PER_SEC;

	for (int i = 0; i < kParticleCount; i++)
		kParticles[i].position += kParticles[i].movementVector * kParticles[i].movementRate * secondsElapsed;
}

glm::dvec3 jester::JointParticleFilter::getCurrentFilterPosition() {
	glm::dvec3 position(0.0);

	for (int i = 0; i < kParticleCount; i++) {
		position = position + kParticles[i].position;
	}

	position = position / (double) kParticleCount;
	position = position / 1000.0;

	return position;
}

jester::JointParticleFilter::JointParticleFilter(FusionBone *bone, int particleCount, double sensorNoise) {
	kBone = bone;
	kParticleCount = particleCount;
	kParticles = new JointParticle[kParticleCount];
	kSensorNoise = sensorNoise;

	initializeParticles();
}

jester::JointParticleFilter::~JointParticleFilter() {

}
