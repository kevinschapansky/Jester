#include "Controller.h"

jester::Controller::Controller() {
	kStartClock = std::clock();
	kScene = NULL;
}

jester::Controller::~Controller() {
	delete kScene;
}

void jester::Controller::init() {
	if (kScene != NULL) {
		delete kScene;
	}
	kScene = new Scene();
}

jester::Scene* jester::Controller::getScene() {
	return kScene;
}

long int jester::Controller::getTimestamp() {
	float clocks;

	clocks = (float) (std::clock() - kStartClock);
	return (long int) (clocks / CLOCKS_PER_SEC);
}