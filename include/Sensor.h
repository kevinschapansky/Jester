#ifndef _Sensor_h_
#define _Sensor_h_

#include "SceneGraphNode.h"

namespace jester {
	class Controller;

	class Sensor : public SceneGraphNode {
	public:
		glm::vec3 getWorldPosition();
		glm::quat getWorldOrientation();

		void setPosition(glm::vec3 position);
		void setOrientation(glm::quat orientation);

		virtual void start() = 0;

		Sensor(SceneGraphNode *parent, Controller *controller);
		virtual ~Sensor();
	protected:
		Controller *kController;
	};
};


#endif