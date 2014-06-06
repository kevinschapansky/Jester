#ifndef _SceneGraphNode_h_
#define _SceneGraphNode_h_

#define GLM_FORCE_RADIANS

#include <iterator>
#include <vector>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <float.h>
#include <string>
#include <sys/time.h>

namespace jester {
	class TimeData {
	public:
		double max;
		double min;
		double total;
		double average;
		unsigned int measurements;
		double startTime;

		TimeData() {
			max = -DBL_MAX;
			min = DBL_MAX;
			total = 0.0;
			average = 0.0;
			measurements = 0;
			startTime = 0.0;
		}

		static double getWallTime() {
			struct timeval time;
			if (gettimeofday(&time,NULL)){
				//  Handle error
				return 0;
			}
			return (double)time.tv_sec + (double)time.tv_usec * .000001;
		}

		void start() {
			startTime = getWallTime();
		}

		void stopAndCalculate() {
			double elapsed = getWallTime() - startTime;

			if (elapsed > max)
				max = elapsed;
			if (elapsed < min)
				min = elapsed;
			total += elapsed;
			measurements++;
			average = total / measurements;
		}

		void print(std::string name) {
			printf("%s:\n", name.c_str());
			printf("\tMax: %f\n", max);
			printf("\tMin: %f\n", min);
			printf("\tAverage: %f over %d\n", average, measurements);
		}
	};

	class SceneGraphNode {
	public:
		std::vector<SceneGraphNode *> children();
		
		glm::vec3 getPosition();
		virtual glm::vec3 getWorldPosition();

		glm::quat getOrientation();
		virtual glm::quat getWorldOrientation();

		virtual glm::mat4 getTransform();
		virtual glm::mat4 getWorldTransform();

		static glm::vec3 positionSpaceConversion(glm::vec3 position, SceneGraphNode *current, SceneGraphNode *desired);
		static glm::quat orientationSpaceConversion(glm::quat orientation, SceneGraphNode *current, SceneGraphNode *desired);

		SceneGraphNode *getParent();

		SceneGraphNode(SceneGraphNode *parent);
		virtual ~SceneGraphNode();

	private:
		void addChild(SceneGraphNode *child);
		void removeChild(SceneGraphNode *child);

	protected:
		SceneGraphNode *kParent;
		glm::vec3 kPosition;
		glm::quat kOrientation;
		std::vector<SceneGraphNode *> kChildren;
	};
};

#endif
