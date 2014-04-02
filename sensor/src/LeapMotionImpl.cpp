#include "LeapMotionImpl.h"

const float jester::LeapMotionImpl::LeapMeasurmentScalingFactor = 1000.f;

void jester::LeapFrameListener::onFrame(const Leap::Controller &controller) {
	Leap::Frame curFrame = controller.frame();
	Leap::HandList trackedHands = curFrame.hands();
	int handCount = trackedHands.count();

	if (handCount == 0) {
		return;
	} else if (handCount == 1) {
		Leap::Hand hand = *trackedHands.begin();

		if (hand.palmPosition()[0] < 0)
			processHands(&hand, NULL);
		else
			processHands(NULL, &hand);
	} else {
		if (handCount > 2) {
			printf("Tracking more than 2 hands. Guessing which are valid...\n");
		}
		Leap::HandList::const_iterator handsIter = trackedHands.begin();
		Leap::Hand first = *handsIter;
		handsIter++;
		Leap::Hand second = *handsIter;
		float firstX = first.palmPosition()[0];
		float secondX = second.palmPosition()[0];

		if (firstX < secondX)
			processHands(&first, &second);
		else
			processHands(&second, &first);
	}
}

void jester::LeapFrameListener::processHands(Leap::Hand *left, Leap::Hand *right) {
	JointFusionData jointData[Bone::JOINT_COUNT];

	for (int i = 0; i < Bone::JOINT_COUNT; i++) {
		jointData[i].id = Bone::intToJointId(i);
		if (i == Bone::JointId::WRIST_L && left != NULL) {
			glm::vec3 leftPosition(left->palmPosition()[0], left->palmPosition()[1], left->palmPosition()[2]);

			leftPosition /= LeapMotionImpl::LeapMeasurmentScalingFactor;
			jointData[i].confidence = 0.7;
			jointData[i].position = &leftPosition;
		} else if (i == Bone::JointId::WRIST_R && right != NULL) {
			glm::vec3 rightPosition(right->palmPosition()[0], right->palmPosition()[1], right->palmPosition()[2]);

			rightPosition /= LeapMotionImpl::LeapMeasurmentScalingFactor;
			jointData[i].confidence = 0.7;
			jointData[i].position = &rightPosition;
		} else {
			jointData[i].confidence = 0.f;
			jointData[i].position = NULL;
		}
	}
	kController->suggestJointInfo(kJSensor, jointData);
}

jester::LeapFrameListener::LeapFrameListener(jester::Sensor *leap, jester::Controller *controller) : Leap::Listener() {
	kController = controller;
	kJSensor = leap;
}

bool jester::LeapMotionImpl::start() {
	printf("Leap Started\n");
	return kLeapController->addListener(*kFrameListener);
}

jester::LeapMotionImpl::LeapMotionImpl(SceneGraphNode *parent, Controller *controller) : Sensor(parent, controller) {
	kLeapController = new Leap::Controller();

	printf("Connecting to Leap Controller\n");
	while (!kLeapController->isConnected()) {}
	printf("Connection established\n");

	kFrameListener = new LeapFrameListener(this, kController);
}

jester::LeapMotionImpl::~LeapMotionImpl() {
	kLeapController->removeListener(*kFrameListener);
	delete kFrameListener;
	delete kLeapController;
}
