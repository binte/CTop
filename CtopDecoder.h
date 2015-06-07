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
#include <vector>
#include <iostream>
#include <algorithm>

class CtopDecoder {
public:
	CtopDecoder();
	~CtopDecoder();

	double decode(const std::vector< double >& chromosome) const;
	double calcularDistancia(int x_1, int y_1, int x_2, int y_2) const;
	int existe(std::vector<int> visitados, int cliente) const;

private:

};

#endif
