#include "LeapMotionImpl.h"

const float jester::LeapMotionImpl::LeapMeasurmentScalingFactor = 1000.f;
const float jester::LeapMotionImpl::LeapConfidence = 0.7;

bool finger_count_comparator (Leap::Hand a, Leap::Hand b) { return a.fingers().count() > b.fingers().count(); }
bool hand_x_coordinate_comparator (Leap::Hand a, Leap::Hand b) { return a.palmPosition()[0] < b.palmPosition()[0]; }
bool left_hand_finger_x_coordinate_comparator (Leap::Finger a, Leap::Finger b) { return a.tipPosition()[0] > b.tipPosition()[0]; }
bool right_hand_finger_x_coordinate_comparator (Leap::Finger a, Leap::Finger b) { return a.tipPosition()[0] < b.tipPosition()[0]; }

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
	std::sort(hands.begin(), hands.end(), hand_x_coordinate_comparator);

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
	Bone::BoneId firstFinger = (whichHand == LeapHand::LEFT ? Bone::BoneId::PHALANX_L_1 : Bone::BoneId::PHALANX_R_1);

	kJointData[toSet].confidence = LeapConfidence;
	kJointData[toSet].position = new glm::vec3(hand.palmPosition()[0] / LeapMeasurmentScalingFactor,
			hand.palmPosition()[1] / LeapMeasurmentScalingFactor,
			hand.palmPosition()[2] / LeapMeasurmentScalingFactor);

	glm::vec3 handVector(hand.direction()[0], hand.direction()[1], hand.direction()[2]);
	glm::vec3 straight(0, 0, -1);

	processFingers(hand, firstFinger, whichHand);
}

void jester::LeapMotionImpl::processFingers(Leap::Hand hand, Bone::BoneId fingerOne, LeapHand whichHand) {
	Leap::FingerList fingerList = hand.fingers();
	std::vector<Leap::Finger> fingers;
	FingerData *fingerIds = (whichHand == LeapHand::LEFT ? kLeftFingerIds : kRightFingerIds);
	bool fingersFound[] = {false, false, false, false, false};

	//project finger tip positions onto palm plane and calculate angle
	for (Leap::FingerList::const_iterator fingerIter = fingerList.begin(); fingerIter != fingerList.end(); fingerIter++) {
		fingers.push_back(*fingerIter);
	}

	//if this is the first time we've seen the full hand, calibrate the finger lengths
	if (fingerIds[0].id == -1 && fingers.size() == 5) {
		if (whichHand == LeapHand::LEFT)
			std::sort(fingers.begin(), fingers.end(), left_hand_finger_x_coordinate_comparator);
		else if (whichHand == LeapHand::RIGHT)
			std::sort(fingers.begin(), fingers.end(), right_hand_finger_x_coordinate_comparator);
		else
			return;
		fingersFound[0] = fingersFound[1] = fingersFound[2] = fingersFound[3] = fingersFound[4] = true;

		for (int i = 0; i < 5; i++) {
			fingerIds[i].id = fingers[i].id();
			fingerIds[i].length = fingers[i].length();
		}
	} else if (fingerIds[0].id == -1)
		return;

	//search for all of the known finger ids
	for (unsigned int i = 0; i < 5; i++) {
		for (unsigned int j = 0; j < fingers.size(); j++) {
			if (fingers[j].id() == fingerIds[i].id) {
				fingersFound[i] = true;
				setFingerInJointData(fingers[j], static_cast<Bone::BoneId>(fingerOne + i));
				fingers.erase(fingers.begin() + j);
			}
		}
	}

	//match all unclaimed fingers based on closest size
	for (unsigned int i = 0; i < fingers.size(); i++) {
		float closestLengthDelta = FLT_MAX;
		int closestFingerIx = -1;

		for (int j = 0; j < 5; j++) {
			if (!fingersFound[j]) {
				float lengthDelta = abs(fingerIds[j].length - fingers[i].length());

				if (lengthDelta < closestLengthDelta) {
					closestLengthDelta = lengthDelta;
					closestFingerIx = j;
				}
			}
		}
		if (closestFingerIx >= 0) {
			fingersFound[closestFingerIx] = true;
			fingerIds[closestFingerIx].id = fingers[i].id();
			setFingerInJointData(fingers[i], static_cast<Bone::BoneId>(fingerOne + closestFingerIx));
		}
	}
}

void jester::LeapMotionImpl::setFingerInJointData(Leap::Finger finger, Bone::BoneId bone) {
	std::pair<Bone::JointId, Bone::JointId> joints = Bone::BoneToJointsMap.find(bone)->second;
	glm::vec3 fingerTip(finger.tipPosition()[0], finger.tipPosition()[1], finger.tipPosition()[2]);
	glm::vec3 fingerDir(finger.direction()[0], finger.direction()[1], finger.direction()[2]);
	float fingerLength = finger.length();

	fingerDir = glm::normalize(fingerDir);
	glm::vec3 fingerStart = fingerTip - fingerLength * fingerDir;

	glm::vec3 *boneStart = new glm::vec3(fingerStart / LeapMeasurmentScalingFactor);
	glm::vec3 *boneEnd = new glm::vec3(fingerTip / LeapMeasurmentScalingFactor);
	kJointData[joints.first].position = boneStart;
	kJointData[joints.second].position = boneEnd;
	kJointData[joints.first].confidence = kJointData[joints.second].confidence = LeapConfidence;
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
	kRightFingerIds[0].id = kRightFingerIds[1].id = kRightFingerIds[2].id = kRightFingerIds[3].id = kRightFingerIds[4].id = -1;
	kLeftFingerIds[0].id = kLeftFingerIds[1].id = kLeftFingerIds[2].id = kLeftFingerIds[3].id = kLeftFingerIds[4].id = -1;

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
