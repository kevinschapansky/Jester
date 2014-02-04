#ifndef _Sensor_h_
#define _Sensor_h_

#include "SceneGraphNode.h"
#include "JesterTransform.h"

namespace jester {
	class Sensor : public SceneGraphNode {
	public:
		virtual void setTransform(JesterTransform newTransform);
		Sensor(SceneGraphNode *parent);
		virtual ~Sensor();
	};
};


#endif