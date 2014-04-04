#ifndef _LeapMotionImpl_h_
#define _LeapMotionImpl_h_

#define GLM_FORCE_RADIANS

#include <stdio.h>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>

#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

#include "Leap.h"

#include "Sensor.h"
#include "SceneGraphNode.h"
#include "DataFusionModule.h"
#include "Controller.h"
#include "Bone.h"

namespace jester {
class LeapMotionImpl : public Sensor {
	public:
		enum LeapHand { LEFT, RIGHT };

		static const float LeapMeasurmentScalingFactor;
		static const float LeapConfidence;

		bool start();
		void processLeapFrame(Leap::Frame frame);

		LeapMotionImpl(SceneGraphNode *parent, Controller *controller);
		~LeapMotionImpl();
	private:
		Leap::Listener *kFrameListener;
		Leap::Controller *kLeapController;

		int kRightHandId;
		int kLeftHandId;
		int kRightFingerIds[5];
		int kLeftFingerIds[5];

		JointFusionData kJointData[Bone::JOINT_COUNT];

		void processHand(Leap::Hand hand, LeapHand whichHand);
		void processFingers(Leap::Hand *hand, Bone::BoneId fingerOne,
				JointFusionData *jointData[Bone::JOINT_COUNT]);
		void clearJointData();
	};

	class LeapFrameListener : public Leap::Listener {
	public:
		 void onFrame(const Leap::Controller&);

		LeapFrameListener(LeapMotionImpl *wrapper);
	private:
		LeapMotionImpl *kLeapWrapper;
	};
};

#endif
