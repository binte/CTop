#include "CtopDecoder.h"


CtopDecoder::CtopDecoder() {}
CtopDecoder::~CtopDecoder() {}


double CtopDecoder::decode(const std::vector< double >& chromosome) const {
	
	double fitAllRoutes = 0;
	std::vector<int> visitedRoutes;  // vector that contains the clients visited in all car routes
	
	/* Declare ranking as a vector of pairs (double, unsigned) with size chromosome.size() */
	std::vector< std::pair< double, unsigned > > ranking(chromosome.size());
	
	/* Local vector with the routes, for each iteration of the decoder */
	std::vector<std::vector<int>> routes;
	
	
	/* Fill each position i of ranking with pairs of the type (chromosome[i], i) */
	for (unsigned i = 0; i < chromosome.size(); ++i)
		ranking[i] = std::pair< double, unsigned >(chromosome[i], i);
		
	/* Sorts the elements in ranking into descending order, based on their gene value */
	std::sort(ranking.begin(), ranking.end(), greater() );
	
	
	// Iterate the cars
	for (int j = 0; j < cars; j++) {

		double totalDist = 0;
		double sumCapacities = 0;
		double routeFit = 0;

		std::vector<int> visited;  // vector that contains the clients visited in one car route

		
		/* Iterate all the clients, choosing the ones that, at the time of their choice (...)
		 		=> A) Are suitable to fit in the current trip starting from the deposit, 
		passing through the clients that have been chosen so far and ending in the deposit, all within the deadline;
				=> B) The sum of its capacity with the total capacity used, is less or equal to the max capacity;
				=> C) Haven't been visited yet
		*/
		for (unsigned i = 0; i < ranking.size(); i++) {

			int client = ranking[i].second;
			std::pair <int,int> previous;
			
			if ( visited.size() == 0 )
				previous = Clients::getDeposit();
			else
				previous = vertices[visitedRoutes.back()].getCoord();
			
			double dist = Clients::distance(previous, vertices[client].getCoord());
			double dist2deposit = Clients::distance(vertices[client].getCoord(), Clients::getDeposit());

			if ( (totalDist + dist) <= (deadline - dist2deposit)									// Condition A
				&& (sumCapacities + vertices[client].getCapacity()) <= maxCapacity  // Condition B
				&& exist(visitedRoutes, client) != 1) {								  						// Condition C

					totalDist += dist;																// update the total distance covered by the current car
					sumCapacities += vertices[client].getCapacity();	// update the total capacity reached by the current car
					routeFit += vertices[client].getScore();					// update the fitness of the current trip
					visited.push_back(client);												// insert the newly visited client in the current trip
					visitedRoutes.push_back(client);									// insert the newly visited client in the trip done by al the cars
			}
		}

		totalDist += Clients::distance(vertices[visitedRoutes.back()].getCoord(), Clients::getDeposit());

		// Insert new route at the tail of the routes vector for this iteration
		routes.push_back(visited);

		fitAllRoutes += routeFit;
	}

	// A mutex guarantees that the code between lock() and unlock() methods is atomic
	mutex.lock();
		
	// If a new maximum fitness was found, update the global routes vector, and print the fitness
	if (fitAllRoutes > totalBest) {
	
		/* Clear the contents of the bestRoutes vector, thus allowing new best routes, if found, 
		to be inserted after being calculated in this iteration of the constructive algorithm  */
		for ( unsigned i=0 ; i<bestRoutes.size() ; i++ ) {
  
			bestRoutes.at(i).clear();
			bestRoutes.at(i).shrink_to_fit();
		}
  
		if(bestRoutes.size() != 0) {
  
			bestRoutes.clear();
			bestRoutes.shrink_to_fit();
		}
	
		// copy the routes obtained in this iteration of the decoder, replacing the previous best routes
		for ( unsigned i=0 ; i<routes.size() ; i++ )
			bestRoutes.push_back(routes.at(i)); 
	
		totalBest = fitAllRoutes;
  
		std::cout << totalBest << std::endl;
	}

	mutex.unlock();

	return maxFit - fitAllRoutes;
}

/* Checks if a client has been visited */
int CtopDecoder::exist(std::vector<int> visited, int client) const {
	
	for (std::vector<int>::iterator it = visited.begin(); it != visited.end(); ++it)
		if ( (*it) == client ) 
			return 1;
	
	return 0;
}
