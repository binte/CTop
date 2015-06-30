#ifndef CLIENTS_H
#define CLIENTS_H

#include <vector>

#include "CtopDecoder.h"
#include "Vertice.h"


namespace Clients {
	
	const std::pair<float,float> deposit = std::make_pair(35, 35);

	std::pair<float,float> getDeposit(void);
	
	int filter();
	double distance(std::pair<int, int> p1, std::pair<int, int> p2);
	double calc_new_vertice(std::vector<int> viagem, int topPriorityGene, unsigned pos, double prev_time);
}

#endif
