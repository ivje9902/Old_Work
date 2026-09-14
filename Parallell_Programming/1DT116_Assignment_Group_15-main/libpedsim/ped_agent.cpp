//
// pedsim - A microscopic pedestrian simulation system.
// Copyright (c) 2003 - 2014 by Christian Gloor
//
//
// Adapted for Low Level Parallel Programming 2017
//
#include "ped_agent.h"
#include "ped_waypoint.h"
#include "ped_model.h"
#include <math.h>

#include <stdlib.h>

Ped::Tagent::Tagent(int posX, int posY) {
	Ped::Tagent::init(posX, posY);
}

Ped::Tagent::Tagent(double posX, double posY) {
	Ped::Tagent::init((int)round(posX), (int)round(posY));
}

void Ped::Tagent::init(int posX, int posY) {
	x = posX;
	y = posY;
	destination = NULL;
	lastDestination = NULL;
	model = nullptr;
	index = 0;
	lockIndex = -1;
}

void Ped::Tagent::setModel(Ped::Model *newModel) {
	model = newModel;
}

void Ped::Tagent::setIndex(size_t newIndex) {
	index = newIndex;
}

size_t Ped::Tagent::getIndex() const { 
	return index; 
}

void Ped::Tagent::setX(int newX) {
	x = newX;
	if (model != nullptr) {
		model->posX[index] = static_cast<float>(newX);
	}
}

void Ped::Tagent::setY(int newY) {
	y = newY;
	if (model != nullptr) {
		model->posY[index] = static_cast<float>(newY);
	}
}

int Ped::Tagent::getX() const {
	if (model != nullptr) {
		return static_cast<int>(model->posX[index]);
	}
	return x;
}

int Ped::Tagent::getY() const {
	if (model != nullptr) {
		return static_cast<int>(model->posY[index]);
	}
	return y;
}

int Ped::Tagent::getDesiredX() const {
	if (model != nullptr) {
		return static_cast<int>(model->desiredX[index]);
	}
	return desiredPositionX;
}

int Ped::Tagent::getDesiredY() const {
	if (model != nullptr) {
		return static_cast<int>(model->desiredY[index]);
	}
	return desiredPositionY;
}

void Ped::Tagent::computeNextDesiredPosition() {
	destination = getNextDestination();
	if (destination == NULL) {
		// no destination, no need to
		// compute where to move to
		return;
	}

	const int currentX = getX();
	const int currentY = getY();
	double diffX = destination->getx() - currentX;
	double diffY = destination->gety() - currentY;
	double len = sqrt(diffX * diffX + diffY * diffY);
	desiredPositionX = (int)round(currentX + diffX / len);
	desiredPositionY = (int)round(currentY + diffY / len);
	if (model != nullptr) {
		model->desiredX[index] = static_cast<float>(desiredPositionX);
		model->desiredY[index] = static_cast<float>(desiredPositionY);
	}
}

void Ped::Tagent::addWaypoint(Twaypoint* wp) {
	waypoints.push_back(wp);
}

Ped::Twaypoint* Ped::Tagent::getNextDestination() {
	Ped::Twaypoint* nextDestination = NULL;
	bool agentReachedDestination = false;

	if (destination != NULL) {
		// compute if agent reached its current destination
		const int currentX = getX();
		const int currentY = getY();
		double diffX = destination->getx() - currentX;
		double diffY = destination->gety() - currentY;
		double length = sqrt(diffX * diffX + diffY * diffY);
		agentReachedDestination = length < destination->getr();
	}

	if ((agentReachedDestination || destination == NULL) && !waypoints.empty()) {
		// Case 1: agent has reached destination (or has no current destination);
		// get next destination if available
		waypoints.push_back(destination);
		nextDestination = waypoints.front();
		waypoints.pop_front();
	}
	else {
		// Case 2: agent has not yet reached destination, continue to move towards
		// current destination
		nextDestination = destination;
	}

	return nextDestination;
}
