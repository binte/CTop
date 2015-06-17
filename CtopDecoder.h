/*
 * SampleDecoder.h
 *
 * Any decoder must have the format below, i.e., implement the method decode(std::vector< double >&)
 * returning a double corresponding to the fitness of that vector. If parallel decoding is to be
 * used in the BRKGA framework, then the decode() method _must_ be thread-safe; the best way to
 * guarantee this is by adding 'const' to the end of decode() so that the property will be checked
 * at compile time.
 *
 * The chromosome inside the BRKGA framework can be changed if desired. To do so, just use the
 * first signature of decode() which allows for modification. Please use double values in the
 * interval [0,1) when updating, thus obeying the BRKGA guidelines.
 *
 *  Created on: Jan 14, 2011
 *      Author: rtoso
 */

#ifndef CtopDecoder_H
#define CtopDecoder_H

#include <list>
#include <mutex>
#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>

#include <unistd.h>
#include <sys/time.h>

#include "MTRand.h"


int extern maxCapacity;
int extern deadline;
int extern cars;
int extern maxFit;

MTRand extern rng;

std::vector< std::pair<int, int> > extern coordenadas;
std::vector<int> extern capacities;
std::vector<int> extern prizes;

std::mutex extern mutex;
double extern totalBest;
std::vector<std::vector<int>> extern bestRoutes;


class CtopDecoder {
public:
	CtopDecoder(void);
	~CtopDecoder(void);

	double decode(const std::vector< double >& chromosome) const;

private:
	double distance(std::pair<int, int> p1, std::pair<int, int> p2) const;
	double genRandom(void);
	int existe(std::vector<int> visited, int cliente) const;
};

#endif
