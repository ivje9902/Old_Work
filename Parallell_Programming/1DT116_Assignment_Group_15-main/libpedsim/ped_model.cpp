//
// pedsim - A microscopic pedestrian simulation system.
// Copyright (c) 2003 - 2014 by Christian Gloor
//
//
// Adapted for Low Level Parallel Programming 2017
//
#include "ped_model.h"
#include "ped_waypoint.h"
#include <time.h>
#include <chrono>
#include "ped_model.h"
#include <iostream>
#include <stack>
#include <algorithm>
#include <omp.h>
#include <thread>
#include <cstdint>
#include <cstring>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <unordered_set>

#ifndef NOCDUA
#include "cuda_testkernel.h"
#endif

#include <stdlib.h>

#include <unordered_set>
#include <cuda_runtime.h>

extern "C" void heatmapCudaInit(int maxAgents);
extern "C" void heatmapCudaFree();
extern "C" void heatmapCudaTick(const int* h_desX,
                                const int* h_desY,
                                int nAgents,
                                int* h_blurredOut);
extern "C" cudaStream_t heatmapCudaStream();

struct RegionTransfer
{
	Ped::Tagent *agent;
	Ped::Model::PED_MODEL_REGION_t *oldRegion;
	Ped::Model::PED_MODEL_REGION_t *newRegion;
};

void splitRegion(Ped::Model::PED_MODEL_REGION_t *region, Ped::Model *model)
{
	if (!region)
		return;

	if (region->child1 != nullptr || region->isSplit)
	{
		std::cout << "Region already split!\n";
		return;
	}

	model->leafRegions.remove(region);

	// Inclusive midpoints -> guarantees full coverage with no gaps.
	const int midX = (region->xl + region->xr) / 2;
	const int midY = (region->yu + region->yl) / 2;

	region->child1 = new Ped::Model::PED_MODEL_REGION_t;
	region->child2 = new Ped::Model::PED_MODEL_REGION_t;
	region->child3 = new Ped::Model::PED_MODEL_REGION_t;
	region->child4 = new Ped::Model::PED_MODEL_REGION_t;

	Ped::Model::PED_MODEL_REGION_t *c1 = region->child1; // top-left
	Ped::Model::PED_MODEL_REGION_t *c2 = region->child2; // top-right
	Ped::Model::PED_MODEL_REGION_t *c3 = region->child3; // bottom-left
	Ped::Model::PED_MODEL_REGION_t *c4 = region->child4; // bottom-right

	auto initChild = [&](Ped::Model::PED_MODEL_REGION_t *c)
	{
		c->parent = region;
		c->isSplit = false;
		c->child1 = c->child2 = c->child3 = c->child4 = nullptr;
		c->agents.clear();
		c->nAgents = 0;
		model->leafRegions.push_back(c);
	};

	initChild(c1);
	initChild(c2);
	initChild(c3);
	initChild(c4);

	// Bounds (inclusive):
	// X splits into [xl..midX] and [midX+1..xr]
	// Y splits into [yu..midY] and [midY+1..yl]
	c1->xl = region->xl;
	c1->xr = midX;
	c1->yu = region->yu;
	c1->yl = midY;

	c2->xl = midX + 1;
	c2->xr = region->xr;
	c2->yu = region->yu;
	c2->yl = midY;

	c3->xl = region->xl;
	c3->xr = midX;
	c3->yu = midY + 1;
	c3->yl = region->yl;

	c4->xl = midX + 1;
	c4->xr = region->xr;
	c4->yu = midY + 1;
	c4->yl = region->yl;

	// Distribute agents into exactly one child.
	for (const Ped::Tagent *agent : region->agents)
	{
		const int ax = agent->getX();
		const int ay = agent->getY();

		auto put = [&](Ped::Model::PED_MODEL_REGION_t *c) -> bool
		{
			if (ax >= c->xl && ax <= c->xr && ay >= c->yu && ay <= c->yl)
			{
				c->agents.push_back(agent);
				c->nAgents++;
				return true;
			}
			return false;
		};

		// Exactly one should match if the parent bounds match the world (e.g., 0..160, 0..120 inclusive).
		if (put(c1))
			continue;
		if (put(c2))
			continue;
		if (put(c3))
			continue;
		(void)put(c4);
	}

	// Keep parent counts consistent (since you use parent->nAgents in merge logic).
	region->nAgents = static_cast<int>(region->agents.size());
	region->isSplit = true;
}

void mergeRegions(Ped::Model::PED_MODEL_REGION_t *parentRegion, Ped::Model *model)
{
	if (parentRegion->isSplit == false)
	{
		std::cout << "Region already merged!" << std::endl;
		return;
	}
	if (parentRegion->child1->isSplit == true ||
		parentRegion->child2->isSplit == true ||
		parentRegion->child3->isSplit == true ||
		parentRegion->child4->isSplit == true)
	{
		std::cout << "Cannot merge region with split children!" << std::endl;
		return;
	}
	model->leafRegions.remove(parentRegion->child1);
	delete parentRegion->child1;
	model->leafRegions.remove(parentRegion->child2);
	delete parentRegion->child2;
	model->leafRegions.remove(parentRegion->child3);
	delete parentRegion->child3;
	model->leafRegions.remove(parentRegion->child4);
	delete parentRegion->child4;
	parentRegion->child1 = nullptr;
	parentRegion->child2 = nullptr;
	parentRegion->child3 = nullptr;
	parentRegion->child4 = nullptr;
	parentRegion->isSplit = false;
	model->leafRegions.push_back(parentRegion);
}

static bool contains(const Ped::Model::PED_MODEL_REGION_t *region, int x, int y)
{
	if (region == nullptr)
	{
		return false;
	}
	return x >= region->xl && x <= region->xr && y >= region->yu && y <= region->yl;
}

static bool isAtRegionBorder(const Ped::Model::PED_MODEL_REGION_t *region, int x, int y)
{
	if (region == nullptr)
	{
		return false;
	}

	return (x == region->xl || x == region->xr || y == region->yu || y == region->yl);
}

Ped::Model::PED_MODEL_REGION_t *findLeafRegion(Ped::Model::PED_MODEL_REGION_t *region, int x, int y)
{
	if (!region || !contains(region, x, y))
		return nullptr;
	if (!region->isSplit)
		return region;

	Ped::Model::PED_MODEL_REGION_t *children[4] = {
		region->child1, region->child2, region->child3, region->child4};
	for (auto *c : children)
	{
		if (contains(c, x, y))
			return findLeafRegion(c, x, y);
	}
	return region;
}

void removeAgentFromOneRegion(Ped::Model::PED_MODEL_REGION_t *region, const Ped::Tagent *agent)
{
	region->agents.remove(agent);
	region->nAgents = static_cast<int>(region->agents.size());
	return;
}

Ped::Model::PED_MODEL_REGION_t *removeAgentFromRegions(Ped::Model::PED_MODEL_REGION_t *region, const Ped::Tagent *agent, int x, int y)
{
	while (region != nullptr)
	{
		removeAgentFromOneRegion(region, agent);

		// Reached root, stop.
		if (region->parent == nullptr)
		{
			return region;
		}

		// Stop when parent is the first region containing target position.
		if (contains(region->parent, x, y))
		{
			return region->parent;
		}

		region = region->parent;
	}

	return nullptr;
}

void addAgentToOneRegion(Ped::Model::PED_MODEL_REGION_t *region,
						 const Ped::Tagent *agent)
{
	for (const Ped::Tagent *a : region->agents)
		if (a->getIndex() == agent->getIndex())
			return;

	region->agents.push_back(agent);
	region->nAgents = region->agents.size();
}

void addAgentToRegions(Ped::Model::PED_MODEL_REGION_t *region,
					   const Ped::Tagent *agent, int x, int y)
{
	if (!region)
		return;

	addAgentToOneRegion(region, agent);

	if (!region->isSplit)
		return;

	Ped::Model::PED_MODEL_REGION_t *children[4] = {
		region->child1, region->child2, region->child3, region->child4};

	for (auto *child : children)
	{
		if (child && contains(child, x, y))
		{
			addAgentToRegions(child, agent, x, y);
			return;
		}
	}
}

void moveAgent(Ped::Model::PED_MODEL_REGION_t *baseRegion, const Ped::Tagent *agent)
{
	const int newX = agent->getX();
	const int newY = agent->getY();
	Ped::Model::PED_MODEL_REGION_t *addRegion = removeAgentFromRegions(baseRegion, agent, newX, newY);
	addAgentToRegions(addRegion, agent, newX, newY);
	return;
}

static int cellIndex(const Ped::Model::PED_MODEL_REGION_t *region, int x, int y)
{
	const int width = region->xr - region->xl + 1;
	return (y - region->yu) * width + (x - region->xl);
}

void Ped::Model::setup(std::vector<Ped::Tagent *> agentsInScenario, std::vector<Twaypoint *> destinationsInScenario, IMPLEMENTATION implementation)
{
#ifndef NOCUDA
	// Convenience test: does CUDA work on this machine?
	cuda_test();
#else
	std::cout << "Not compiled for CUDA" << std::endl;
#endif

	// Set
	agents = std::vector<Ped::Tagent *>(agentsInScenario.begin(), agentsInScenario.end());

	// Set up destinations
	destinations = std::vector<Ped::Twaypoint *>(destinationsInScenario.begin(), destinationsInScenario.end());

	// Sets the chosen implemenation. Standard in the given code is SEQ
	this->implementation = implementation;

	// Set up heatmap (relevant for Assignment 4)
	setupHeatmapSeq();

	auto alloc_aligned = [&](void **p, size_t bytes)
	{
		if (bytes == 0)
		{
			*p = nullptr;
			return;
		}
		int rc = posix_memalign(p, ALIGN_BYTES, bytes);
		if (rc != 0)
		{
			std::cerr << "posix_memalign failed (rc=" << rc << ")\n";
			std::exit(1);
		}
	};
	const size_t nAgents = agents.size();
	alloc_aligned((void **)&posX, nAgents * sizeof(float));
	alloc_aligned((void **)&posY, nAgents * sizeof(float));
	alloc_aligned((void **)&desiredX, nAgents * sizeof(float));
	alloc_aligned((void **)&desiredY, nAgents * sizeof(float));
	alloc_aligned((void **)&destX, nAgents * sizeof(float));
	alloc_aligned((void **)&destY, nAgents * sizeof(float));
	alloc_aligned((void **)&active, nAgents * sizeof(uint8_t));

	// Initialize from agent objects.
	// IMPORTANT: do NOT call getNextDestination()/computeNextDesiredPosition() here,
	// because it mutates waypoint queues and changes simulation state.
	for (size_t i = 0; i < nAgents; ++i)
	{
		const float initialX = static_cast<float>(agents[i]->getX());
		const float initialY = static_cast<float>(agents[i]->getY());
		posX[i] = initialX;
		posY[i] = initialY;
		desiredX[i] = initialX;
		desiredY[i] = initialY;

		// Will be filled in tick() after deciding each agent's destination
		destX[i] = 0.0f;
		destY[i] = 0.0f;
		active[i] = 0;

		agents[i]->setModel(this);
		agents[i]->setIndex(i);
	}

	this->baseRegion = new PED_MODEL_REGION_t;
	this->baseRegion->xl = 0;
	this->baseRegion->yu = 0;
	this->baseRegion->xr = 160;
	this->baseRegion->yl = 120;
	this->baseRegion->nAgents = agents.size();
	for (const Ped::Tagent *agent : agents)
	{
		this->baseRegion->agents.push_back(agent);
	}

	const int width = this->baseRegion->xr - this->baseRegion->xl + 1;
	const int height = this->baseRegion->yl - this->baseRegion->yu + 1;
	borderOwnerSize = width * height;
	borderOwner = new std::atomic<int>[borderOwnerSize];
	for (int i = 0; i < borderOwnerSize; ++i)
	{
		borderOwner[i].store(-1, std::memory_order_relaxed);
	}

	// mark every occupied cell with the agentId
	for (const Ped::Tagent *a : agents)
	{
		const int idx = cellIndex(this->baseRegion, a->getX(), a->getY());
		borderOwner[idx].store((int)a->getIndex(), std::memory_order_relaxed);
	}

	leafRegions.push_back(this->baseRegion);
	splitRegion(this->baseRegion, this);

	const int nAgentsInt = (int)agents.size();

	// Allocate host desired arrays (ints)
	h_desX_int = (int*)malloc(nAgentsInt * sizeof(int));
	h_desY_int = (int*)malloc(nAgentsInt * sizeof(int));

	// Pin host memory used by async CUDA copies.
	if (h_desX_int)
	{
		cudaHostRegister(h_desX_int,
										nAgentsInt * sizeof(int),
										cudaHostRegisterDefault),
					   "cudaHostRegister(h_desX_int)";
	}
	if (h_desY_int)
	{
		cudaHostRegister(h_desY_int,
										nAgentsInt * sizeof(int),
										cudaHostRegisterDefault),
					   "cudaHostRegister(h_desY_int)";
	}
	if (blurred_flat)
	{
		cudaHostRegister(blurred_flat,
										SCALED_SIZE * SCALED_SIZE * sizeof(int),
										cudaHostRegisterDefault),
					   "cudaHostRegister(blurred_flat)";
	}

	// Init CUDA-side heatmap buffers
	heatmapCudaInit(nAgentsInt);

}

void thread_func(Ped::Tagent *agent, Ped::Model *model,
				 std::vector<RegionTransfer> &localTransfers)
{
	int oldX = agent->getX();
	int oldY = agent->getY();

	// Use 'this' instead of 'model' because we are inside Ped::Model::tick()
	agent->computeNextDesiredPosition();
	model->move(agent);

	if (agent->getX() != oldX || agent->getY() != oldY)
	{
		// Re-find regions to ensure we are using the current Quadtree state
		auto *oldReg = findLeafRegion(model->baseRegion, oldX, oldY);
		auto *newReg = findLeafRegion(model->baseRegion, agent->getX(), agent->getY());

		if (oldReg && newReg && oldReg != newReg)
		{
			// Explicitly create the struct to avoid initializer list ambiguity
			RegionTransfer tr;
			tr.agent = agent;
			tr.oldRegion = oldReg;
			tr.newRegion = newReg;
			localTransfers.push_back(tr);
		}
	}
}

void vector_func(__m128 &x, __m128 &y, __m128 &diffX, __m128 &diffY, __m128 &len, __m128 &desposX, __m128 &desposY, float *posX, float *posY, float *desiredX, float *desiredY, float *destX, float *destY, int i, const std::vector<Ped::Tagent *> &agents)
{

	Ped::Twaypoint *destinations[4];

	for (int j = 0; j < 4; ++j)
	{
		destinations[j] = agents[i + j]->getNextDestination();
		agents[i + j]->destination = destinations[j];
		if (destinations[j] != nullptr)
		{
			destX[i + j] = (float)destinations[j]->getx();
			destY[i + j] = (float)destinations[j]->gety();
		}
		else
		{
			// Keep current position for inactive lanes
			destX[i + j] = posX[i + j];
			destY[i + j] = posY[i + j];
		}
	}

	// build integer mask lanes
	int m0 = (destinations[0] != nullptr) ? -1 : 0;
	int m1 = (destinations[1] != nullptr) ? -1 : 0;
	int m2 = (destinations[2] != nullptr) ? -1 : 0;
	int m3 = (destinations[3] != nullptr) ? -1 : 0;

	// pack into SIMD register
	__m128i mask_i = _mm_set_epi32(m3, m2, m1, m0);

	// reinterpret as float mask (for blend / and / andnot)
	__m128 mask_ps = _mm_castsi128_ps(mask_i);

	__m128 wpx = _mm_load_ps(&destX[i]);
	__m128 wpy = _mm_load_ps(&destY[i]);

	x = _mm_load_ps(&posX[i]);
	y = _mm_load_ps(&posY[i]);

	// if (destination == nullptr)
	// {
	// 	return;
	// }

	diffX = _mm_sub_ps(wpx, x);
	diffY = _mm_sub_ps(wpy, y);

	len = _mm_add_ps(_mm_mul_ps(diffX, diffX), _mm_mul_ps(diffY, diffY));
	len = _mm_sqrt_ps(len);

	desposX = _mm_div_ps(diffX, len);
	desposY = _mm_div_ps(diffY, len);

	desposX = _mm_add_ps(x, desposX);
	desposY = _mm_add_ps(y, desposY);

	// Match sequential rounding (round to nearest int)
	__m128 roundX = _mm_cvtepi32_ps(_mm_cvtps_epi32(desposX));
	__m128 roundY = _mm_cvtepi32_ps(_mm_cvtps_epi32(desposY));

	__m128 oldX = _mm_load_ps(&posX[i]);
	__m128 oldY = _mm_load_ps(&posY[i]);
	__m128 oldDesiredX = _mm_load_ps(&desiredX[i]);
	__m128 oldDesiredY = _mm_load_ps(&desiredY[i]);

	// new = (mask & computed) | (~mask & old)
	__m128 newX = _mm_or_ps(_mm_and_ps(mask_ps, roundX),
							_mm_andnot_ps(mask_ps, oldX));

	__m128 newY = _mm_or_ps(_mm_and_ps(mask_ps, roundY),
							_mm_andnot_ps(mask_ps, oldY));

	// Keep desired position unchanged for inactive lanes.
	__m128 newDesiredX = _mm_or_ps(_mm_and_ps(mask_ps, roundX),
								   _mm_andnot_ps(mask_ps, oldDesiredX));
	__m128 newDesiredY = _mm_or_ps(_mm_and_ps(mask_ps, roundY),
								   _mm_andnot_ps(mask_ps, oldDesiredY));

	// Store new positions in SoA buffers
	_mm_store_ps(&posX[i], newX);
	_mm_store_ps(&posY[i], newY);
	_mm_store_ps(&desiredX[i], newDesiredX);
	_mm_store_ps(&desiredY[i], newDesiredY);
}

void Ped::Model::tick()
{

	int minThreshhold = 15;
	int maxThreshhold = 60;
	std::vector<PED_MODEL_REGION_t *> regionsToSplit;
	std::unordered_set<PED_MODEL_REGION_t *> parentsToMerge;

	for (auto *region : leafRegions)
	{
		// Split condition (leaf only)
		if (!region->isSplit && region->nAgents > maxThreshhold)
		{
			regionsToSplit.push_back(region);
		}

		// Merge condition (check parent)
		PED_MODEL_REGION_t *parent = region->parent;
		if (parent && parent->isSplit && parent->nAgents < minThreshhold)
		{
			parentsToMerge.insert(parent);
		}
	}

	for (auto *parent : parentsToMerge)
	{
		if (parent->isSplit &&
			parent->child1 && parent->child2 &&
			parent->child3 && parent->child4 &&
			!parent->child1->isSplit &&
			!parent->child2->isSplit &&
			!parent->child3->isSplit &&
			!parent->child4->isSplit &&
			parent->nAgents < minThreshhold)
		{
			mergeRegions(parent, this);
		}
	}

	for (auto *region : regionsToSplit)
	{
		if (!region->isSplit && region->nAgents > maxThreshhold)
		{
			splitRegion(region, this);
		}
	}

	if (implementation == SEQ)
	{
		std::vector<RegionTransfer> transfers;
		transfers.reserve(agents.size());

		for (int i = 0; i < (int)agents.size(); i++)
			thread_func(agents[i], this, transfers);

		for (auto &tr : transfers)
			moveAgent(tr.oldRegion, tr.agent);

		auto start = std::chrono::high_resolution_clock::now();

		updateHeatmapSeq();

		auto end = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double> elapsed = end - start;

		std::cout << "Time: " << elapsed.count() << " seconds\n";

	}
	else if (implementation == OMP)
	{
		int nt = 8;
		if (const char *s = std::getenv("PEDSIM_THREADS"))
		{
			int v = std::atoi(s);
			if (v > 0)
				nt = v;
		}

		omp_set_num_threads(nt);

		// ---- CUDA heatmap update (sequential host step + GPU work) ----
		// Build desired position arrays (ints)
		const int nAgentsInt = (int)agents.size();
		for (int i = 0; i < nAgentsInt; ++i) {
			h_desX_int[i] = agents[i]->getDesiredX();
			h_desY_int[i] = agents[i]->getDesiredY();
		}
		heatmapCudaTick(h_desX_int, h_desY_int, nAgentsInt, blurred_flat);

		// Immediately check if GPU finished
		cudaError_t status = cudaStreamQuery(heatmapCudaStream());

		if (status == cudaErrorNotReady) {
			std::cout << "GPU is still running while CPU continues -> overlap confirmed\n";
		} else {
			std::cout << "GPU already finished\n";
		}

		// Create nt actual vectors
		std::vector<std::vector<const Ped::Tagent *>> vectors(nt);

		std::vector<const Ped::Tagent *> *allVectors[nt];
		for (int i = 0; i < nt; ++i)
			allVectors[i] = &vectors[i];

		int nLeaves = leafRegions.size();
		int base = nLeaves / nt;
		int rest = nLeaves % nt;

		auto it = leafRegions.begin();

		for (int t = 0; t < nt; ++t)
		{
			int count = base;
			if (t == nt - 1)
				count += rest; // last thread gets leftovers

			for (int k = 0; k < count && it != leafRegions.end(); ++k, ++it)
			{
				PED_MODEL_REGION_t *region = *it;

				// Append agents of this region to existing vector
				allVectors[t]->insert(
					allVectors[t]->end(),
					region->agents.begin(),
					region->agents.end());
			}
		}

		// thread-local buffers for region transfers
		std::vector<std::vector<RegionTransfer>> transfers(nt);

#pragma omp parallel num_threads(nt)
		{
			int thread_id = omp_get_thread_num();
			std::vector<const Ped::Tagent *> &myVector = *allVectors[thread_id];
			auto &localTransfers = transfers[thread_id];

			for (size_t i = 0; i < myVector.size(); ++i)
			{
				// Cast to non-const so we can call move() and modify coordinates
				Ped::Tagent *agent = const_cast<Ped::Tagent *>(myVector[i]);
				thread_func(agent, this, localTransfers);
			}
		}
		// With: Automatic region splitting but agents get stuck
		// Without: No agents get stuck but no automatic region splitting
	
		for (auto &vec : transfers)
		{
			for (auto &t : vec)
			{
				// moveAgent uses current agent->(x,y), which is already updated by CAS move()
				moveAgent(t.oldRegion, t.agent);
			}
		}
		
	}
	else if (implementation == PTHREAD)
	{
		int num_threads = 19;
		if (const char *s = std::getenv("PEDSIM_THREADS"))
		{
			int v = std::atoi(s);
			if (v > 0)
				num_threads = v;
		}

		// Thread-local transfer buffers
		std::vector<std::vector<RegionTransfer>> transfers(num_threads);

		std::vector<std::thread> threads;
		threads.reserve(num_threads);

		for (int t = 0; t < num_threads; ++t)
		{
			threads.emplace_back([&, t]()
								 {
				auto &localTransfers = transfers[t];

				for (int i = t; i < (int)agents.size(); i += num_threads)
				{
					thread_func(agents[i], this, localTransfers);
				} });
		}

		for (auto &th : threads)
			th.join();

		// Apply region moves sequentially (safe)
		for (auto &vec : transfers)
		{
			for (auto &tr : vec)
			{
				moveAgent(tr.oldRegion, tr.agent);
			}
		}
	}
	else if (implementation == VECTOR)
	{
		const int nAgents = agents.size();
		const int vec_end = nAgents - (nAgents % 4);

		for (int i = 0; i < vec_end; i += 4)
		{
			__m128 x, y, diffX, diffY, len, desposX, desposY;
			vector_func(x, y, diffX, diffY, len, desposX, desposY,
						posX, posY, desiredX, desiredY, destX, destY, i, agents);
		}

		std::vector<RegionTransfer> transfers;
		transfers.reserve(nAgents - vec_end);

		for (int i = vec_end; i < nAgents; i++)
		{
			thread_func(agents[i], this, transfers);
		}

		for (auto &tr : transfers)
		{
			moveAgent(tr.oldRegion, tr.agent);
		}
	}
}

////////////
/// Everything below here relevant for Assignment 3.
/// Don't use this for Assignment 1!
///////////////////////////////////////////////

static inline bool inWorld(const Ped::Model::PED_MODEL_REGION_t *r, int x, int y)
{
	return x >= r->xl && x <= r->xr && y >= r->yu && y <= r->yl;
}

static inline bool ownerIdValid(const Ped::Model *model, int ownerId)
{
	return ownerId >= 0 && ownerId < (int)model->agents.size();
}

// Claim destination cell with CAS, then release source cell.
bool Ped::Model::tryMoveCAS(Ped::Tagent *agent, int nx, int ny)
{
	const int id = (int)agent->getIndex();
	const int ox = agent->getX();
	const int oy = agent->getY();

	const int src = cellIndex(baseRegion, ox, oy);
	const int dst = cellIndex(baseRegion, nx, ny);

	if (src == dst)
		return true;

	int expectedDst = -1;
	// Attempt to take the new cell
	if (borderOwner[dst].compare_exchange_strong(expectedDst, id, std::memory_order_acq_rel))
	{
		// Success! Now release the old cell
		int expectedSrc = id;
		borderOwner[src].store(-1, std::memory_order_release);

		agent->setX(nx);
		agent->setY(ny);
		return true;
	}

	// Single-try behavior: if destination owner is stale, clear it for a future tick.
	if (ownerIdValid(this, expectedDst))
	{
		const Ped::Tagent *owner = agents[expectedDst];
		if (owner->getX() != nx || owner->getY() != ny)
		{
			int stale = expectedDst;
			(void)borderOwner[dst].compare_exchange_strong(stale, -1, std::memory_order_acq_rel);
		}
	}
	return false;
}

// Moves the agent to the next desired position. If already taken, it will
// be moved to a location close to it.

void Ped::Model::move(Ped::Tagent *agent)
{
	const int ox = agent->getX();
	const int oy = agent->getY();

	const int dx = agent->getDesiredX();
	const int dy = agent->getDesiredY();

	// Primary target: desired position
	std::pair<int, int> c0(dx, dy);

	int diffX = dx - ox;
	int diffY = dy - oy;

	std::pair<int, int> c1, c2;

	// Straight movement
	if (diffX == 0 || diffY == 0)
	{
		c1 = {dx + diffY, dy + diffX};
		c2 = {dx - diffY, dy - diffX};
	}
	// Diagonal movement
	else
	{
		c1 = {dx, oy};
		c2 = {ox, dy};
	}

	// Try desired cell first
	if (inWorld(baseRegion, c0.first, c0.second) &&
		tryMoveCAS(agent, c0.first, c0.second))
		return;
	// Try first alternative
	if (inWorld(baseRegion, c1.first, c1.second) &&
		tryMoveCAS(agent, c1.first, c1.second))
		return;
	// Try second alternative
	if (inWorld(baseRegion, c2.first, c2.second) &&
		tryMoveCAS(agent, c2.first, c2.second))
		return;

	// All options occupied → stay in place
}

/// Returns the list of neighbors within dist of the point x/y. This
/// can be the position of an agent, but it is not limited to this.
/// \date    2012-01-29
/// \return  The list of neighbors
/// \param   x the x coordinate
/// \param   y the y coordinate
/// \param   dist the distance around x/y that will be searched for agents (search field is a square in the current implementation)
set<const Ped::Tagent *> Ped::Model::getNeighbors(int x, int y, int dist) const
{

	// create the output list
	// ( It would be better to include only the agents close by, but this programmer is lazy.)
	return set<const Ped::Tagent *>(agents.begin(), agents.end());
}

void Ped::Model::cleanup()
{
	if (borderOwner)
	{
		delete[] borderOwner;
		borderOwner = nullptr;
		borderOwnerSize = 0;
	}


	if (h_desX_int)
	{
		cudaHostUnregister(h_desX_int), "cudaHostUnregister(h_desX_int)";
	}
	if (h_desY_int)
	{
		cudaHostUnregister(h_desY_int), "cudaHostUnregister(h_desY_int)";
	}
	if (blurred_flat)
	{
		cudaHostUnregister(blurred_flat), "cudaHostUnregister(blurred_flat)";
	}

	// Nothing to do here right now.
	// if (posX)
	// {
	// 	free(posX);
	// 	posX = nullptr;
	// }
	// if (posY)
	// {
	// 	free(posY);
	// 	posY = nullptr;
	// }
	// if (destX)
	// {
	// 	free(destX);
	// 	destX = nullptr;
	// }
	// if (destY)
	// {
	// 	free(destY);
	// 	destY = nullptr;
	// }
	// if (active)
	// {
	// 	free(active);
	// 	active = nullptr;
	// }
}

Ped::Model::~Model()
{
    heatmapCudaFree();

    if (h_desX_int) { free(h_desX_int); h_desX_int = nullptr; }
    if (h_desY_int) { free(h_desY_int); h_desY_int = nullptr; }

	cleanup();
	std::for_each(agents.begin(), agents.end(), [](Ped::Tagent *agent)
				  { delete agent; });
	std::for_each(destinations.begin(), destinations.end(), [](Ped::Twaypoint *destination)
				  { delete destination; });
}
