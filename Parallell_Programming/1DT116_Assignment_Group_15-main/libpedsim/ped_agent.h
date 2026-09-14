//
// pedsim - A microscopic pedestrian simulation system.
// Copyright (c) 2003 - 2014 by Christian Gloor
//
// Adapted for Low Level Parallel Programming 2017
//
// TAgent represents an agent in the scenario. Each
// agent has a position (x,y) and a number of destinations
// it wants to visit (waypoints). The desired next position
// represents the position it would like to visit next as it
// will bring it closer to its destination.
// Note: the agent will not move by itself, but the movement
// is handled in ped_model.cpp.
//

#ifndef _ped_agent_h_
#define _ped_agent_h_ 1

#include <vector>
#include <deque>
#include <cstddef>

using namespace std;

namespace Ped
{
	class Twaypoint;
	class Model;

	class Tagent
	{
	public:
		Tagent(int posX, int posY);
		Tagent(double posX, double posY);

		// Returns the coordinates of the desired position
		int getDesiredX() const;
		int getDesiredY() const;

		// Sets the agent's position
		void setX(int newX);
		void setY(int newY);

		// Bind this agent to the model's SoA storage.
		void setModel(Model *newModel);
		void setIndex(size_t newIndex);
		size_t getIndex() const;
		// Update the position according to get closer
		// to the current destination
		void computeNextDesiredPosition();

		// Position of agent defined by x and y
		int getX() const;
		int getY() const;

		// Returns the next destination to visit
		Twaypoint *getNextDestination();

		// Adds a new waypoint to reach for this agent
		void addWaypoint(Twaypoint *wp);

		// The agent's desired next position
		int desiredPositionX;
		int desiredPositionY;

		// The current destination (may require several steps to reach)
		Twaypoint *destination;

		// -1 means no lock is held for the agent.
		int lockIndex = -1;

	private:
		Tagent() {};

		// The agent's current position
		int x;
		int y;

		// The last destination
		Twaypoint *lastDestination;

		// The queue of all destinations that this agent still has to visit
		deque<Twaypoint *> waypoints;

		// Binding to model SoA storage.
		Model *model;
		size_t index;

		// Internal init function
		void init(int posX, int posY);
	};
}

#endif
