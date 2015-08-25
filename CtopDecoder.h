/*
 * CtopDecoder.h
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
 */

#ifndef CtopDecoder_H
#define CtopDecoder_H

/*#include <sstream>  
#include <fstream>
#include <string>
#include <ctime>*/

#include <list>
#include <mutex>
#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>

#include <unistd.h>
#include <sys/time.h>

#include "Vertice.h"
#include "Clients.h"
#include "MTRand.h"


int extern maxCapacity;
int extern deadline;
int extern cars;
int extern maxFit;

MTRand extern rng;

std::vector<Vertice> extern vertices;

std::mutex extern mutex;
double extern totalBest;
std::vector<std::vector<int>> extern bestRoutes;

bool extern scriptVersion;


/* struct used to change the order of sorting, so that the given chromosome can be processed in descending order as well */
struct greater
{
    template<class T>
    
	bool operator() (T const &a, T const &b) const { 
		return a > b; 
	}
};


class CtopDecoder {
	
private:
	std::pair<float,float> origin;
	std::pair<float,float> deposit;
		
	int exist(std::vector<int> visited, int client) const;
	std::tuple<int, int, double> where2insert(int topPriorityGene, std::vector<std::vector<int>> routes, 
																						std::vector<double> times) const;

public:
	CtopDecoder(void);
	~CtopDecoder(void);

	void setOrigin(std::pair<float,float> origin);
	void setDeposit(std::pair<float,float> deposit);
	std::pair<float,float> getOrigin(void);
	std::pair<float,float> getDeposit(void);

	double decode(std::vector< double >& chromosome) const;
};

#endif
