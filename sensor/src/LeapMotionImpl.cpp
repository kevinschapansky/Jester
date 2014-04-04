#include "LeapMotionImpl.h"

const float jester::LeapMotionImpl::LeapMeasurmentScalingFactor = 1000.f;
const float jester::LeapMotionImpl::LeapConfidence = 0.7;

bool finger_count_comparator (Leap::Hand a, Leap::Hand b) { return a.fingers().count() > b.fingers().count(); }
bool x_coordinate_comparator (Leap::Hand a, Leap::Hand b) { return a.palmPosition()[0] < b.palmPosition()[0]; }

jester::LeapFrameListener::LeapFrameListener(jester::LeapMotionImpl *wrapper) : Leap::Listener() {
	kLeapWrapper = wrapper;
}

void jester::LeapFrameListener::onFrame(const Leap::Controller &controller) {
	kLeapWrapper->processLeapFrame(controller.frame());
}

void jester::LeapMotionImpl::processLeapFrame(Leap::Frame frame) {
	Leap::HandList handList = frame.hands();
	std::vector<Leap::Hand> hands;
	Leap::Hand right, left;
	bool foundRight = false, foundLeft = false;

	clearJointData();

	//put all of the hands in a std::vector
	for (Leap::HandList::const_iterator handsIter = handList.begin(); handsIter != handList.end(); handsIter++)
		hands.push_back(*handsIter);

	std::sort(hands.begin(), hands.end(), finger_count_comparator);

	//search to see if last known hand Ids are still available, assign hand, and delete from available hands
	// also, delete any unknown hands that have no fingers
	for (unsigned int i = 0; i < hands.size(); i++)
		if (hands[i].id() == kRightHandId) {
			foundRight = true;
			right = hands[i];
			hands.erase(hands.begin() + i);
		} else if (hands[i].id() == kLeftHandId) {
			foundLeft = true;
			left = hands[i];
			hands.erase(hands.begin() + i);
		} else if (hands[i].fingers().count() == 0){
			hands.erase(hands.begin() + i);
		}

	//figure out how many hands we need
	int neededHands = 0;
	if (!foundLeft) neededHands++;
	if (!foundRight) neededHands++;

	//assume that the highest finger count objects are the most likely to be hands
	//only keep as many hands as we need (bounded by the number actually available)
	hands.erase(hands.begin() + std::min((int) hands.size(), neededHands), hands.end());

	//sort by x coordinate
	std::sort(hands.begin(), hands.end(), x_coordinate_comparator);

	if (hands.size() == 0 || neededHands == 0) { //if we have or need no hands, do nothing here
	} else if (neededHands > (int) hands.size()) { //if we need two and only have 1
		//assume left side is left, right side is right
		if (hands[0].palmPosition()[0] < 0) {
			foundLeft = true;
			left = hands[0];
			kLeftHandId = left.id();
		} else {
			foundRight = true;
			right = hands[0];
			kRightHandId = right.id();
		}
	} else if (foundLeft == false && neededHands == 1) { //if we need left
		foundLeft = true;
		left = hands[0];
		kLeftHandId = left.id();
	} else if (foundRight == false && neededHands == 1) { //if we need right
		foundRight = true;
		right = hands[hands.size() - 1];
		kRightHandId = right.id();
	} else { //if we need both and have two
		foundLeft = foundRight = true;
		left = hands[0];
		kLeftHandId = left.id();
		right = hands[1];
		kRightHandId = right.id();
	}
	if (foundLeft)
		processHand(left, LeapHand::LEFT);
	if (foundRight)
		processHand(right, LeapHand::RIGHT);

	kController->suggestJointInfo(this, kJointData);
}

void jester::LeapMotionImpl::processHand(Leap::Hand hand, LeapHand whichHand) {
	Bone::JointId toSet = (whichHand == LeapHand::LEFT ? Bone::JointId::WRIST_L : Bone::JointId::WRIST_R);

	kJointData[toSet].confidence = LeapConfidence;
	kJointData[toSet].position = new glm::vec3(hand.palmPosition()[0] / LeapMeasurmentScalingFactor,
			hand.palmPosition()[1] / LeapMeasurmentScalingFactor,
			hand.palmPosition()[2] / LeapMeasurmentScalingFactor);
}

void jester::LeapMotionImpl::processFingers(Leap::Hand *hand, Bone::BoneId fingerOne,
		JointFusionData *jointData[Bone::JOINT_COUNT]) {
	/*
	Leap::FingerList fingers = hand->fingers();
	glm::vec3 handVector(hand->direction()[0], hand->direction()[1], hand->direction()[2]);
	glm::vec3 handNormal(hand->palmNormal()[0], hand->palmNormal()[1], hand->palmNormal()[2]);
	glm::vec3 handPosition(hand->palmPosition()[0], hand->palmPosition()[1], hand->palmPosition()[2])

	for (Leap::FingerList::const_iterator fingerIter = fingers.begin(); fingerIter != fingers.end(); fingerIter++) {
		Leap::Finger curFinger = *fingerIter;
		glm::vec3 fingerVector(curFinger.direction()[0], 0, curFinger.direction()[2]);

		printf("Finger %d: %f\n", curFinger.id(), glm::degrees(glm::acos(glm::dot(glm::normalize(fingerVector), handVector))));
	}
	printf("\n");*/
}

void jester::LeapMotionImpl::clearJointData() {
	for (int i = 0; i < Bone::JOINT_COUNT; i++) {
		if (kJointData[i].position != NULL)
			delete kJointData[i].position;
		kJointData[i].confidence = 0.f;
		kJointData[i].position = NULL;
		kJointData[i].id = Bone::intToJointId(i);
	}
}

bool jester::LeapMotionImpl::start() {
	printf("Leap Started\n");
	return kLeapController->addListener(*kFrameListener);
}

jester::LeapMotionImpl::LeapMotionImpl(SceneGraphNode *parent, Controller *controller) : Sensor(parent, controller) {
	kLeapController = new Leap::Controller();
	kRightHandId = -1;
	kLeftHandId = -1;
	kRightFingerIds[0] = kRightFingerIds[1] = kRightFingerIds[2] = kRightFingerIds[3] = kRightFingerIds[4] = -1;
	kLeftFingerIds[0] = kLeftFingerIds[1] = kLeftFingerIds[2] = kLeftFingerIds[3] = kLeftFingerIds[4] = -1;

	printf("Connecting to Leap Controller\n");
	while (!kLeapController->isConnected()) {}
	printf("Connection established\n");

	//ensure proper init for avoiding bad deletes
	for (int i = 0; i < Bone::JOINT_COUNT; i++)
		kJointData[i].position = NULL;

	kFrameListener = new LeapFrameListener(this);
}

jester::LeapMotionImpl::~LeapMotionImpl() {
	kLeapController->removeListener(*kFrameListener);
	delete kFrameListener;
	delete kLeapController;
}
