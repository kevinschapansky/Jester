#ifndef _PhysicsEngine_h_
#define _PhysicsEngine_h_

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <queue>
#include <ctime>
#include <chrono>
#include <stdio.h>
#include <sys/time.h>

class PhysicsBall {
public:
	PhysicsBall(glm::vec3 pos, glm::vec3 vel, float rad) {
		position = pos;
		velocity = vel;
		radius = rad;
	};

	glm::vec3 position;
	glm::vec3 velocity;
	float radius;
};

class PhysicsInteractor {
public:
	PhysicsInteractor(glm::vec3 pos, float rad, bool handDown) {
		position = pos;
		radius = rad;
		bounce = handDown;
	};

	glm::vec3 position;
	float radius;
	bool bounce;
};

class PhysicsEngine {
public:
	void start();
	std::vector<PhysicsBall> update(glm::vec3 desiredDropOrigin, std::vector<PhysicsInteractor> hands);

	PhysicsEngine(int ballsPerSecond, float minimumY, float ballRadius);

private:
	PhysicsBall generateBall(glm::vec3 origin);

	double get_wall_time() {
	    struct timeval time;
	    if (gettimeofday(&time,NULL)){
	        //  Handle error
	        return 0;
	    }
	    return (double)time.tv_sec + (double)time.tv_usec * .000001;
	}

	std::queue<PhysicsBall> kBalls;
	double kLastUpdate;
	double kLastBall;
	unsigned int kMsBetweenBalls;
	float kMinimumY;
	float kBallRadius;
};

#endif
