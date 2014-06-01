#ifndef _PhysicsEngine_h_
#define _PhysicsEngine_h_

#include <glm/vec3.hpp>
#include <vector>

class PhysicsBall {
public:
	glm::vec3 position;
	glm::vec3 direction;
	float velocity;
};

class PhysicsEngine {
public:
	PhysicsEngine(int ballsPerSecond, glm::vec3 startPosition);

private:
	std::vector<PhysicsBall> kBalls;
};

#endif
