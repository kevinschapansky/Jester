#ifndef _Sensor_h_
#define _Sensor_h_

#include "SceneGraphNode.h"

namespace jester {
	class Controller;

	class Sensor : public SceneGraphNode {
	public:
		void setPosition(glm::vec3 position);
		void setOrientation(glm::quat orientation);

		virtual bool start() = 0;

		Sensor(SceneGraphNode *parent, Controller *controller);
		virtual ~Sensor();
	protected:
		Controller *kController;
	};
};


#endif