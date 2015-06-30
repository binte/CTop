#include "Clients.h"

namespace Clients {
	
	std::pair<float,float> getDeposit(void) {
		
		return deposit;
	}
	
	/** Filter the vertices that will never be taken, because they do not respect the following restriction:
	 * 
	 * distance(origin -> vertice) + distance(vertice -> destination)  >  Tmax
	 *
	 * Notice that the deposit is given to this fuction, 
	 * which implies that the origin is the same as the destination
	 *
	 * Those vertices will be removed from the Vertices' vector
	 */
	int filter() {
		
		std::vector<int> novo;
		
		// insert the indexes of the vertices that cannot be taken in a new vector
		for(unsigned i=0 ; i<=vertices.size() ; i++) {
			
			if( (distance(deposit, vertices[i].getCoord()) + 
					 distance(vertices[i].getCoord(), deposit)) > deadline) {
				
				novo.push_back(i);
			}
		}

		for(unsigned i=0, j=0 ; i<novo.size() ; i++, j++)
			vertices.erase(vertices.begin() + (novo[i] - j) );
			
		return novo.size();
	}
	
	/* Calculate the euclidean distance between two points */
	double distance(std::pair<int, int> p1, std::pair<int, int> p2) {
		
		return sqrt(pow((p1.first - p2.first), 2) + pow((p1.second - p2.second), 2));
	}


	/* Returns the new distance of the trip, with the newly inserted vertice in a given position */
	double calc_new_vertice(std::vector<int> viagem, int topPriorityGene, unsigned pos, double prev_time) {
	
		double new_distance;		
		
		if(viagem.size() == 0)
			new_distance = distance(deposit, vertices.at(topPriorityGene).getCoord()) + distance(vertices.at(topPriorityGene).getCoord(), deposit);
		else
			if(pos == 0)
				new_distance = prev_time - distance(deposit, vertices.at(viagem.at(pos)).getCoord())
								 		 + distance(deposit, vertices.at(topPriorityGene).getCoord())
								 		 + distance(vertices.at(topPriorityGene).getCoord(), vertices.at(viagem.at(pos)).getCoord());
			else
				if (pos == viagem.size())
					new_distance = prev_time - distance(vertices.at(viagem.at(viagem.size() - 1)).getCoord(), deposit) 
									 		 + distance(vertices.at(viagem.at(viagem.size() - 1)).getCoord(), vertices.at(topPriorityGene).getCoord())
									 		 + distance(vertices.at(topPriorityGene).getCoord(), deposit);
				else
					new_distance = prev_time - distance(vertices.at(viagem.at(pos-1)).getCoord(), vertices.at(viagem.at(pos)).getCoord())
									 		 + distance(vertices.at(viagem.at(pos-1)).getCoord(), vertices.at(topPriorityGene).getCoord())
									 		 + distance(vertices.at(topPriorityGene).getCoord(), vertices.at(viagem.at(pos)).getCoord());
		
		return new_distance;
	}
}
