#pragma once
#ifndef _MERSENNETWISTER_H_
#define _MERSENNETWISTER_H_

/**
 * MersenneTwister.h
 *
 * @author Makoto Matsumoto, Takuji Nishimura, modified by Tony Fox
 * @version MT19937
 */

void mt_init(int seed);
int mt_rand(int from = 0, int to = 0x7fffffff);

#endif