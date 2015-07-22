#ifndef CLIENTS_H
#define CLIENTS_H

#include <vector>

#include "CtopDecoder.h"
#include "Vertice.h"


namespace Clients {

	int filter(std::pair<float,float> origin, std::pair<float,float> deposit);
	double distance(std::pair<int, int> p1, std::pair<int, int> p2);
	double addedTime(std::vector<int> viagem, int topPriorityGene, unsigned pos, std::pair<float,float> origin, std::pair<float,float> deposit);
}

#endif
