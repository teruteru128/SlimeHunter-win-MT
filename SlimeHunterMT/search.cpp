
#define CL_TARGET_OPENCL_VERSION 300
#include "pch.h"
#include "rnd.h"
#include "mcSlimeChunkOracle.h"
#include "search.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <math.h>
#include <bitset>
#include <atomic>
#include <algorithm>
#include <iostream>
#include <string>
#include <omp.h>
#include <CL/cl.h>
using std::bitset;

bool extracted(bitset<625 * 625>* set, int x, int z);

volatile std::atomic_int cont = 1;
volatile std::atomic_uint64_t seed;

Result* task(Config* config) {
	bitset<625 * 625>* set = new bitset<625 * 625>();
	int x = 0;
	int z = 0;
	int tempZ0;
	int lineCombo = 0;
	int lineComboMax = 0;
	Result* result = NULL;
	uint64_t worldSeed = 0;
	bool found = false;
	while (cont) {
		worldSeed = seed++;
		for (z = 0, tempZ0 = 0; z < 625; z++, tempZ0 += 625) {
			for (x = 0; x < 625; x++) {
				set->set(tempZ0 + x, isSlimeChunk(worldSeed, x - 312, z - 312));
			}
		}
		found = false;
		for (z = 621, tempZ0 = 388125; z >= 0; z--, tempZ0 -= 625) {
			// 時間と空間のトレードオフ
			// 連続でスライムチャンクが並んでる個数を数える
			lineComboMax = 0;
			lineCombo = 0;
			for (x = 0; x < 625; x++) {
				lineCombo = (set->test(tempZ0 + x)) ? (lineCombo + 1) : 0;
				lineComboMax = std::max(lineComboMax, lineCombo);
			}
			// 4個未満ならスキップ
			if (lineComboMax < 4)
			{
				z -= 3;
				continue;
			}
			for (x = 621; x >= 0; x--)
			{
				found |= extracted(set, x, z);
			}
		}
		if (found) {
			cont = 0;
			std::cout << "見つけたー！[" << worldSeed << "]" << std::endl;
			result = new Result(worldSeed, x - 312, z - 312);
		}
		if ((worldSeed & 0x3fffL) == 0x3fffL) {
			std::cout << "done: " << worldSeed << std::endl;
		}
	}

	return result;
}

// TODO &&で繋いだ短絡評価と&で繋いだ投機的実行でどっちのほうが早くなるんだろうか->ベンチマーク
// TODO ベンチマーク->特定のシードではなくランダムなシードを使って計測されなければならない
// TODO 全スレッドで1シードを検査すべきなのか、各スレッドでそれぞれ別のシードを探索すべきなのか(計算粒度の問題)
inline bool extracted(std::bitset<625 * 625>* set, int x, int z) {
	return set->test((z + 3) * 625 + x + 0) && set->test((z + 3) * 625 + x + 1) && set->test((z + 3) * 625 + x + 2) && set->test((z + 3) * 625 + x + 3) &&
		set->test((z + 2) * 625 + x + 0) && set->test((z + 2) * 625 + x + 1) && set->test((z + 2) * 625 + x + 2) && set->test((z + 2) * 625 + x + 3) &&
		set->test((z + 1) * 625 + x + 0) && set->test((z + 1) * 625 + x + 1) && set->test((z + 1) * 625 + x + 2) && set->test((z + 1) * 625 + x + 3) &&
		set->test((z + 0) * 625 + x + 0) && set->test((z + 0) * 625 + x + 1) && set->test((z + 0) * 625 + x + 2) && set->test((z + 0) * 625 + x + 3);
}

int mpsample(void) {
	bitset<625 * 625>* set = new bitset<625 * 625>();
	int pos = 0;
	for (pos = 0; pos < 390625; pos++) {
		set->set(pos, isSlimeChunk(263622805221400ULL, (pos % 625) - 312, (pos / 625) - 312));
	}
	int z = 0;
	int x = 0;
	bool found = false;
	for (z = 621; z >= 0; z--) {
#pragma omp parallel for reduction(| : found)
		for (x = 621; x >= 0; x--) {
			found |= extracted(set, x, z);
		}
	}
	std::cout << found << std::endl;
	return 0;
}

Config::Config() {
}
Config::~Config() {}

Result::Result(uint64_t worldSeed, int x, int z)
{
	this->worldSeed = worldSeed;
	this->x = x;
	this->z = z;
}

Result::~Result()
{
}

uint64_t Result::getWorldSeed() {
	return this->worldSeed;
}
