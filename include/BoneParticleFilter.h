#ifndef _BoneParticleFilter_h_
#define _BoneParticleFilter_h_

#define GLM_FORCE_RADIANS

#include <cmath>
#include <random>
#include <ctime>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "FusionBone.h"
#include "Filter.h"

namespace jester {
	class BoneParticleFilter : public Filter {
	public:
		jester::FusionBone update(jester::FusionBone bone);

		BoneParticleFilter(FusionBone *bone, int particleCount, double sensorNoise);
		~BoneParticleFilter();
	private:
		typedef struct BoneParticle {
			glm::dvec3 start;
			glm::dvec3 end;
			glm::dvec3 movementVector;
			double movementRate;
			double probability;
		} BoneParticle;

		FusionBone *kBone;
		int kParticleCount;
		BoneParticle *kParticles;
		double kSensorNoise;

		std::time_t kLastTimeStamp;
		std::default_random_engine kResampleGenerator;

		double gaussianProbability(double mean, double variance, double x);

		void initializeParticles();
		void resampleParticles();
		void generateProbabilities(glm::dvec3 start, glm::dvec3 end);
		double getProbability(BoneParticle particle, glm::dvec3 sensedStart, glm::dvec3 sensedEnd);
		void moveParticles();
		glm::dvec3 getCurrentStartPosition();
		glm::dvec3 getCurrentEndPosition();
	};
};

#endif
