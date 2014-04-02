#ifndef _LeapMotionImpl_h_
#define _LeapMotionImpl_h_

#define GLM_FORCE_RADIANS

#include <stdio.h>
#include <thread>
#include <chrono>
#include <vector>

#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

#include "Leap.h"

#include "Sensor.h"
#include "SceneGraphNode.h"
#include "DataFusionModule.h"
#include "Controller.h"
#include "Bone.h"

namespace jester {
	class LeapFrameListener : public Leap::Listener {
	public:
		 void onFrame(const Leap::Controller&);

		LeapFrameListener(Sensor *leap, Controller *controller);
	private:
		void processHands(Leap::Hand *left, Leap::Hand *right);

		Sensor *kJSensor;
		Controller *kController;
	};

	class LeapMotionImpl : public Sensor {
	public:
		static const float LeapMeasurmentScalingFactor;
		bool start();

		LeapMotionImpl(SceneGraphNode *parent, Controller *controller);
		~LeapMotionImpl();
	private:
		Leap::Listener *kFrameListener;
		Leap::Controller *kLeapController;
	};
};

#endif
