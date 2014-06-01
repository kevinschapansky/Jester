#include "PhysicsEngine.h"

void PhysicsEngine::start() {
	kLastUpdate = get_wall_time();
}

std::vector<PhysicsBall> PhysicsEngine::update(glm::vec3 desiredDropOrigin, std::vector<PhysicsInteractor> hands) {
	std::vector<PhysicsBall> balls;
	float secElapsed = get_wall_time() - kLastUpdate;
	kLastUpdate = get_wall_time();

	//create balls if its been too long
	if (((get_wall_time() - kLastBall) * 1000.f) > kMsBetweenBalls) {
		kLastBall = get_wall_time();
		kBalls.push(PhysicsBall(desiredDropOrigin, glm::vec3(0, -0.01, 0), kBallRadius));
	}

	//run physics for all balls
	for (unsigned int i = 0; i < kBalls.size(); i++) {
		PhysicsBall curBall = kBalls.front();
		bool keepBall = true;
		kBalls.pop();

		//Don't calculate ball if it has dropped out of frame
		if (curBall.position.y < kMinimumY)
			continue;

		//Calc ball movement
		curBall.position += curBall.velocity * secElapsed;
		curBall.velocity += glm::vec3(0, -9.8 * secElapsed, 0);

		//Handle hand interaction
		for (unsigned int j = 0; j < hands.size(); j++) {
			if (glm::distance(curBall.position, hands[j].position) < curBall.radius + hands[j].radius) {
				if (hands[j].bounce) {
					//ball is bounced, recalc velocity and add
					glm::vec3 newDirection = glm::normalize(curBall.position - hands[j].position);
					curBall.velocity = newDirection * glm::length(curBall.velocity);
				} else {
					keepBall = false;
					break;
				}
			}
		}
		if (keepBall) {
			kBalls.push(curBall);
			balls.push_back(curBall);
		}
	}

	return balls;
}

PhysicsBall PhysicsEngine::generateBall(glm::vec3 origin) {
	glm::vec3 pos = origin;
	glm::vec3 vel(0, -0.01, 0);
	float rad = kBallRadius;

	return PhysicsBall(pos, vel, rad);
}

PhysicsEngine::PhysicsEngine(int ballsPerSecond, float minimumY, float ballRadius) {
	kMinimumY = minimumY;
	kMsBetweenBalls = 1.f / ballsPerSecond * 1000.f;
	kLastUpdate = get_wall_time();
	kLastBall = get_wall_time();
	kBallRadius = ballRadius;
}
