#ifndef _JointParticleFilter_h_
#define _JointParticleFilter_h_

#define GLM_FORCE_RADIANS

#include <cmath>
#include <random>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "FusionBone.h"

namespace jester {
	class JointParticleFilter {
	public:
		glm::vec3 update(glm::vec3 worldSpacePosition);

		JointParticleFilter(FusionBone *boneForEndpoint, int particleCount, double sensorNoise);
		~JointParticleFilter();
	private:
		typedef struct JointParticle {
			glm::vec3 position;
			glm::vec3 movementVector;
			double movementRate;
			double probability;
		} JointParticle;

		FusionBone *kBone;
		int kParticleCount;
		JointParticle *kParticles;
		double kSensorNoise;

		std::default_random_engine kResampleGenerator;

		double gaussianProbability(double mean, double variance, double x);

		void initializeParticles();
		void resampleParticles();
		void generateProbabilities(glm::vec3 endpoint);
		double getProbability(JointParticle particle, glm::vec3 sensedPosition);
		void moveParticles();
		glm::vec3 getCurrentFilterPosition();
	};
};

#endif
