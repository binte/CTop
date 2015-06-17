#include <iostream>
#include <sstream>  
#include <fstream>
#include <string>
#include <ctime>

#include "CtopDecoder.h"


CtopDecoder::CtopDecoder() {}
CtopDecoder::~CtopDecoder() {}


double CtopDecoder::decode(const std::vector< double >& chromosome) const {
	
	double totalAllRoutes = 0;
	std::vector<int> visitedRoutes;
	std::pair<int,int> deposit = std::make_pair(35,35);
	
	/* Declare ranking as a vector of pairs (double, unsigned) with size chromosome.size() */
	std::vector< std::pair< double, unsigned > > ranking(chromosome.size());
	
	/* Local vector with the routes, for each iteration of the decoder */
	std::vector<std::vector<int>> routes;
	
	
	/* Fill each position i of ranking with pairs of the type (chromosome[i], i) */
	for (unsigned i = 0; i < chromosome.size(); ++i)
		ranking[i] = std::pair< double, unsigned >(chromosome[i], i);
		
	std::sort(ranking.begin(), ranking.end());  // Sorts the elements in ranking into ascending order, based on their gene value

	
	// Iterate the cars
	for (int j = 0; j < cars; j++) {

		double totalDistancia = 0;
		double totalCapacidade = 0;
		double totalRoute = 0;

		std::vector<int> visited;
		
		/* Iterate all the clients, until finding one that: 
		 		=> Is suitable to be visited in a roundtrip from the deposit within the deadline; 
				=> The sum of its capacity with the total capacity used, is less or equal to the max capacity;
				=> 
		*/
		for (unsigned i = 0; i < ranking.size(); i++) {

// rng.randInt(coordenadas.size()) Onde utilizar aleatoriedade no algoritmo construtivo?

			int client = ranking[i].second;
			double dist = distance(deposit, coordenadas[client]);
			
/*			std::cout << "\ndepositoInicial: " << depositoInicial << 
									 "\ndeadline: " << deadline <<
			 						 "\ndepositoFinal: " << depositoFinal <<
									 "\ntotalCapacidade: " << totalCapacidade <<
									 "\ncapacities[client]: " << capacities[client] <<
									 "\nclient: " << client <<
									 "\nMAX CAPACITY: " << maxCapacity;
			
			getchar();*/
			
			
			if (2*dist <= deadline
				&& (totalCapacidade + capacities[client]) <= maxCapacity
				&& (visitedRoutes.size() <= 0 || existe(visitedRoutes, client) != 1)) {
					
				totalDistancia += dist;
				totalCapacidade += capacities[client];
				totalRoute += prizes[client];
				visited.push_back(client);
				visitedRoutes.push_back(client);
				
				break; // exit the innermost FOR cycle
			}
		}

		if (visitedRoutes.size() > 0) {

			//ATE DISTANCIA MAXIMA MENOS DEPOSITO FINAL OU CAPACITY MAXIMA SER ULTRAPASSADA
			for (unsigned i = 0; i < ranking.size(); i++) {

				int client = ranking[i].second;
				
				double distancia = distance(coordenadas[visitedRoutes.back()], coordenadas[client]);
				double depositoFinal = distance(coordenadas[client], deposit);

				if ((totalDistancia + distancia) <= (deadline - depositoFinal)
					&& (totalCapacidade + capacities[client]) <= maxCapacity
					&& existe(visitedRoutes, client) != 1) {
				
					totalDistancia += distancia;
					totalCapacidade += capacities[client];
					totalRoute += prizes[client];
					visited.push_back(client);
					visitedRoutes.push_back(client);
				}
			}

			//SE ULTRAPASSA DISTANCIA MAXIMA MENOS A DISTANCIA PARA O DEPOSITO FINAL
			double depositoFinal = distance(coordenadas[visitedRoutes.back()], deposit);
			totalDistancia += depositoFinal;
		}
		
		// Insert new route at the tail of the routes vector for this iteration
		routes.push_back(visited);

		totalAllRoutes += totalRoute;
	}

	// A mutex guarantees that the code between lock() and unlock() methods is atomic
	mutex.lock();
		
	// If a new maximum fitness was found, update the global routes vector, and print the fitness
	if (totalAllRoutes > totalBest) {
	
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
	
		totalBest = totalAllRoutes;
  
		std::cout << totalBest << std::endl;
	}

	mutex.unlock();

	return maxFit - totalAllRoutes;
}

/* Calculate the euclidean distance between two points */
double CtopDecoder::distance(std::pair<int, int> p1, std::pair<int, int> p2) const {
	
	return sqrt(pow((p1.first - p2.first), 2) + pow((p1.second - p2.second), 2));
}

/* Seed and generate new random number */
double CtopDecoder::genRandom(void) {
	
	timeval t;
	pid_t pid = getpid(); // only works in unix based machines
	// DWORD WINAPI pid =  GetCurrentProcessId(); // only works in windows machines

	gettimeofday(&t, NULL);

	/* Intializes a random number generator, based in the current time and the process pid */
	srand(t.tv_usec * t.tv_sec * pid);
	
	return rand();
}

int CtopDecoder::existe(std::vector<int> visited, int cliente) const {
	
	for (std::vector<int>::iterator it = visited.begin(); it != visited.end(); ++it)
		if ( (*it) == cliente ) 
			return 1;
	
	return 0;
}
