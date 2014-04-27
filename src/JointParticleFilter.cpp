#include "JointParticleFilter.h"

#define PI 3.141592653

glm::vec3 jester::JointParticleFilter::update(glm::vec3 worldSpacePosition) {
	glm::vec3 parentSpacePosition = glm::vec3(glm::inverse(kBone->getWorldTransform()) * glm::vec4(worldSpacePosition, 1));

	moveParticles();
	generateProbabilities(parentSpacePosition);
	resampleParticles();
	return glm::vec3(kBone->getWorldTransform() * glm::vec4(getCurrentFilterPosition(), 1));
}

double jester::JointParticleFilter::gaussianProbability(double mean, double variance, double x) {
	return exp(-1.0 * pow((mean - x), 2.0) / pow(variance, 2.0) / 2.0) / sqrt(2.0 * PI * pow(variance, 2.0));
}

void jester::JointParticleFilter::initializeParticles() {
	std::default_random_engine generator;
	std::uniform_int_distribution<int> endDist(-kBone->getLength() * 1000, kBone->getLength() * 1000);
	std::uniform_int_distribution<int> angleDist(-100, 100);
	std::uniform_int_distribution<int> rateDist(-1500, 1500);

	for (int i = 0; i < kParticleCount; i++) {
		kParticles[i].movementRate = rateDist(generator) / 1000.0;
		kParticles[i].movementVector = glm::normalize(glm::vec3(
				(angleDist(generator) / 100.0),
				(angleDist(generator) / 100.0),
				(angleDist(generator) / 100.0)));
		kParticles[i].position = glm::vec3(
				(endDist(generator) / 1000.0),
				(endDist(generator) / 1000.0),
				(endDist(generator) / 1000.0));
	}
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

void jester::JointParticleFilter::generateProbabilities(glm::vec3 endpoint) {
	for (int i = 0; i < kParticleCount; i++)
		kParticles[i].probability = getProbability(kParticles[i], endpoint);
}

double jester::JointParticleFilter::getProbability(JointParticle particle, glm::vec3 sensedPosition) {
	double probability = 1.0;

	for (int i = 0; i < 3; i++) {
		probability *= gaussianProbability(particle.position[i], kSensorNoise, sensedPosition[i]);

	}

	return probability;
}

void jester::JointParticleFilter::moveParticles() {
	//Do nothing for now, possibly investigate continue movement path
}

glm::vec3 jester::JointParticleFilter::getCurrentFilterPosition() {
	glm::vec3 position(0.0);

	for (int i = 0; i < kParticleCount; i++) {
		position += kParticles[i].position;
	}

	position.x = position.x / kParticleCount;
	position.y = position.y / kParticleCount;
	position.z = position.z / kParticleCount;

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
