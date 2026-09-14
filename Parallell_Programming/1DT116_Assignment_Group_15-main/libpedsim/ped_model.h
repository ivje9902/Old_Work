//
// pedsim - A microscopic pedestrian simulation system.
// Copyright (c) 2003 - 2014 by Christian Gloor
//
// Adapted for Low Level Parallel Programming 2017
//
// Model coordinates a time step in a scenario: for each
// time step all agents need to be moved by one position if
// possible.
//
#ifndef _ped_model_h_
#define _ped_model_h_

#include <vector>
#include <map>
#include <set>
#include <list>
#include "ped_agent.h"

#include <cstddef>
#include <cstdint>
#include <atomic>

namespace Ped
{
	class Tagent;

	// The implementation modes for Assignment 1 + 2:
	// chooses which implementation to use for tick()
	enum IMPLEMENTATION
	{
		CUDA,
		VECTOR,
		OMP,
		PTHREAD,
		SEQ
	};

	class Model
	{
	public:
		// Sets everything up
		void setup(std::vector<Tagent *> agentsInScenario, std::vector<Twaypoint *> destinationsInScenario, IMPLEMENTATION implementation);

		// Coordinates a time step in the scenario: move all agents by one step (if applicable).
		void tick();

		// Region areas for agents
		struct PED_MODEL_REGION_t
		{
			int xl, yu, xr, yl;
			int nAgents = 0;
			std::list<const Ped::Tagent *> agents;
			bool isSplit = false;
			PED_MODEL_REGION_t *child1 = nullptr;
			PED_MODEL_REGION_t *child2 = nullptr;
			PED_MODEL_REGION_t *child3 = nullptr;
			PED_MODEL_REGION_t *child4 = nullptr;
			PED_MODEL_REGION_t *parent = nullptr;
		};

		PED_MODEL_REGION_t *baseRegion = nullptr;

		std::list<PED_MODEL_REGION_t*> leafRegions;

		// Returns the agents of this scenario
		const std::vector<Tagent *> &getAgents() const { return agents; };

		// Adds an agent to the tree structure
		void placeAgent(const Ped::Tagent *a);

		// Cleans up the tree and restructures it. Worth calling every now and then.
		void cleanup();
		~Model();

		// Returns the heatmap visualizing the density of agents
		int const *const *getHeatmap() const { return blurred_heatmap; };
		int getHeatmapSize() const;

		// The agents in this scenario
		std::vector<Tagent *> agents;

		// Moves an agent towards its next position
		bool tryMoveCAS(Ped::Tagent* agent, int nx, int ny);
        void move(Ped::Tagent *agent);
		bool tryLockBorderCell(int idx, int agentId);
		void unlockBorderCell(int idx, int agentId);

		void updateHeatmapSeq();

		int* h_desX_int = nullptr;
		int* h_desY_int = nullptr;

		// Also store the flat pointer to blurred buffer:
		int* blurred_flat = nullptr;

	private:
		friend class Tagent;

		// Denotes which implementation (sequential, parallel implementations..)
		// should be used for calculating the desired positions of
		// agents (Assignment 1)
		IMPLEMENTATION implementation;

		// Use float here for SIMD width (SSE=4 lanes, AVX=8 lanes).
		// If you need stricter numerical matching with the double math, change these to double*.
		float *posX = nullptr;
		float *posY = nullptr;
		float *desiredX = nullptr;
		float *desiredY = nullptr;
		float *destX = nullptr;
		float *destY = nullptr;

		// 1 if agent currently has a destination (destination != NULL), else 0
		uint8_t *active = nullptr;

		// Alignment in bytes for SIMD loads: 16 for SSE, 32 for AVX
		static constexpr size_t ALIGN_BYTES = 16;

		// The waypoints in this scenario
		std::vector<Twaypoint *> destinations;

		// -1 means free, otherwise owner agent id.
		std::atomic<int> *borderOwner = nullptr;
		int borderOwnerSize = 0;

		////////////
		/// Everything below here won't be relevant until Assignment 3
		///////////////////////////////////////////////

		// Returns the set of neighboring agents for the specified position
		set<const Ped::Tagent *> getNeighbors(int x, int y, int dist) const;

		////////////
		/// Everything below here won't be relevant until Assignment 4
		///////////////////////////////////////////////

#define SIZE 1024
#define CELLSIZE 5
#define SCALED_SIZE SIZE *CELLSIZE

		// The heatmap representing the density of agents
		int **heatmap;

		// The scaled heatmap that fits to the view
		int **scaled_heatmap;

		// The final heatmap: blurred and scaled to fit the view
		int **blurred_heatmap;

		void setupHeatmapSeq();
	};
}
#endif
