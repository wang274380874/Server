/**
 * MersenneTwister.cpp
 *
 * @author Makoto Matsumoto, Takuji Nishimura, modified by Tony Fox
 * @version MT19937
 */
#include "StdAfx.h"
#include "MersenneTwister.h"

int mt[624];
int index = 0;

bool initialized = false;

void mt_init(int seed) {
	int i;
	mt[0] = seed;
	for(i = 1; i < 624; i++) {
		mt[i] = (int)(1812433253 * (mt[i - 1] ^ (mt[i - 1] >> 30)) + i);
	}
}

void mt_generate() {
	if (!initialized) {
		time_t timestamp;
		time(&timestamp);
		clock_t clockstamp;
		clockstamp = clock() * 1000 / CLOCKS_PER_SEC;
		mt_init((int)timestamp + (int)clockstamp);
		initialized = true;
	}
	int i = 0;
	for(i = 0; i < 624; i++) {
		int y = (mt[i] & 0x80000000) + (mt[(i + 1) % 624] & 0x7fffffff);
		mt[i] = mt[(i + 397) % 624] ^ (y >> 1);
		if (y % 2)
			mt[i] = mt[i] ^ 2567483615;
	}
}

int mt_rand(int from, int to) {
	int y;
	if (index == 0)
		mt_generate(); // call mt_generate() every 624 times.
	y = mt[index];
	y ^= (y >> 11);
	y ^= ((y << 7) & 2636928640);
	y ^= ((y << 15) & 4022730752);
	y ^= (y >> 18);

	index = (index + 1) % 624;

	if (from == 0 && to == 0x7fffffff)
		return y;

	int delta = to - from;
	int result = from + y % delta;
	return result;
}
