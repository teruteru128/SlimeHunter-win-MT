
#include "pch.h"
#define CL_TARGET_OPENCL_VERSION 300
#include "mcSlimeChunkOracle.h"
#include "rnd.h"
#include "search.hpp"
#include <algorithm>
#include <atomic>
#include <bitset>
#include <CL/opencl.h>
#include <inttypes.h>
#include <iostream>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <time.h>
using std::bitset;

volatile std::atomic_int cont = 1;

Result* task(Config* config) {
	int skip = 1;
	int x = 0;
	int z = 0;
	uint64_t worldSeed = 0;
	std::atomic_uint64_t* ptr = config->getSeed();
	while (cont) {
		worldSeed = std::atomic_fetch_add(ptr, 1);
		for (z = 620; z >= 0; z--) {
			skip = 1;
			for (x = 620; x >= 0; x--)
			{
				if (!isSlimeChunk(worldSeed, x - 312, z - 312)) {
					x -= 4;
					continue;
				}
				if (!isSlimeChunk(worldSeed, x - 312 + 1, z - 312)) {
					x -= 3;
					continue;
				}
				if (!isSlimeChunk(worldSeed, x - 312 + 2, z - 312)) {
					x -= 2;
					continue;
				}
				if (!isSlimeChunk(worldSeed, x - 312 + 3, z - 312)) {
					x -= 1;
					continue;
				}
				if (!isSlimeChunk(worldSeed, x - 312 + 4, z - 312)) {
					continue;
				}
				skip = 0;
				if (!(isSlimeChunk(worldSeed, x - 312, z - 312 + 1) && isSlimeChunk(worldSeed, x - 312, z - 312 + 2) && isSlimeChunk(worldSeed, x - 312, z - 312 + 3) && isSlimeChunk(worldSeed, x - 312, z - 312 + 4))) {
					x -= 4;
					continue;
				}
				if (!(isSlimeChunk(worldSeed, x - 312 + 1, z - 312 + 1) && isSlimeChunk(worldSeed, x - 312 + 1, z - 312 + 2) && isSlimeChunk(worldSeed, x - 312 + 1, z - 312 + 3) && isSlimeChunk(worldSeed, x - 312 + 1, z - 312 + 4))) {
					x -= 3;
					continue;
				}
				if (!(isSlimeChunk(worldSeed, x - 312 + 2, z - 312 + 1) && isSlimeChunk(worldSeed, x - 312 + 2, z - 312 + 2) && isSlimeChunk(worldSeed, x - 312 + 2, z - 312 + 3) && isSlimeChunk(worldSeed, x - 312 + 2, z - 312 + 4))) {
					x -= 2;
					continue;
				}
				if (!(isSlimeChunk(worldSeed, x - 312 + 3, z - 312 + 1) && isSlimeChunk(worldSeed, x - 312 + 3, z - 312 + 2) && isSlimeChunk(worldSeed, x - 312 + 3, z - 312 + 3) && isSlimeChunk(worldSeed, x - 312 + 3, z - 312 + 4))) {
					x -= 1;
					continue;
				}
				if (!(isSlimeChunk(worldSeed, x - 312 + 4, z - 312 + 1) && isSlimeChunk(worldSeed, x - 312 + 4, z - 312 + 2) && isSlimeChunk(worldSeed, x - 312 + 4, z - 312 + 3) && isSlimeChunk(worldSeed, x - 312 + 4, z - 312 + 4))) {
					std::cout << "惜しい!" << worldSeed << ", " << ((x - 312) << 4) << ", " << ((z - 312) << 4) << std::endl;
					continue;
				}
				cont = 0;
				std::cout << "見つけたー！[" << worldSeed << ", " << ((x - 312) << 4) << ", " << ((z - 312) << 4) << "]" << std::endl;
				// delete set;
				return new Result(worldSeed, (x - 312) << 4, (z - 312) << 4);
			}
			// 4個未満ならスキップ
			if (skip)
			{
				z -= 4;
				continue;
			}
		}
	}

	return NULL;
}

Config::Config(std::atomic_uint64_t* seed) {
	this->seed = seed;
}
Config::~Config() {}
std::atomic_uint64_t* Config::getSeed() {
	return this->seed;
}
Result::Result(uint64_t worldSeed, int x, int z)
{
	this->worldSeed = worldSeed;
	this->x = x;
	this->z = z;
}

Result::~Result()
{
}

uint64_t Result::getWorldSeed() const {
	return this->worldSeed;
}

int Result::getX() const {
	return this->x;
}

int Result::getZ() const {
	return this->z;
}
