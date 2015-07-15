#include "CtopDecoder.h"


CtopDecoder::CtopDecoder() {}
CtopDecoder::~CtopDecoder() {}


double CtopDecoder::decode(const std::vector< double >& chromosome) const {
	
	int random, topPriorityGene = -1, counter;  // counter: number of positions to ignore, due to blacklisting
	bool flag = true, fl = false;  // fl flags the choice of a gene, and flag indicates if it's possible to insert more vertices
	double fitness = 0;
	std::tuple<int, int, double> data2change;
	MTRand r;
	std::vector<int> blackList;  // indexes of the discarded genes during a trip
	std::vector<int> auxblackList;  // indexes (within blackList) of the high priority genes that can't be taken
	std::vector<int> trips; // visited genes
	std::vector<int> minValues;  // indexes of the high priority genes
	std::vector<std::vector<int>> routes(cars); 	// contains the routes for each iteration, including visiting the deposits
	std::vector<double> times(cars);  // current duration of each of the routes
	
	r.seed();  // initialize the random number generator
	
	/* Declare ranking as a vector of pairs (double, unsigned) with size chromosome.size() */
	std::vector< std::pair< double, unsigned > > ranking(chromosome.size());
	
	/* Fill each position i of ranking with pairs of the type (chromosome[i], i) */
	for (unsigned i = 0; i < chromosome.size(); ++i)
		ranking[i] = std::pair< double, unsigned >(chromosome[i], i);
		
	/* Sorts the elements in ranking into ascending order, based on their gene value */
	std::sort(ranking.begin(), ranking.end());
	
	
	while(flag)  // while it's possible to insert new vertices
	{

		/* Ciclo utilizado para escolher o vértice de maior prioridade que caiba numa viagem, executando os seguintes passos até
		um vértice ser escolhido, ou até todos serem percorridos
				-> Escolher os vértices de maior prioridade
				-> Aleatoriamente ir escolhendo um desses vértices, até um encaixar numa viagem, ou até todos terem sido percorridos							
		*/
		while( !fl && blackList.size() + trips.size() < ranking.size() ) {
  
			//std::cout << "blackList.size(): " << blackList.size() << std::endl;
			//std::cout << "trips.size(): " << trips.size() << std::endl;
			//std::cout << "ranking.size(): " << ranking.size() << std::endl;

			unsigned ind_min=0;

			//std::cout << "ind_min: " << ind_min << std::endl;

			// Find the gene with the highest priority that is neither visited nor blacklisted (...)
			while (std::find(blackList.begin(), blackList.end(), ranking.at(ind_min).second) != blackList.end() || 
				  	 std::find(trips.begin(), trips.end(), ranking.at(ind_min).second) != trips.end() )
				ind_min++;

			// (...) and save its index
			minValues.push_back(ranking.at(ind_min).second);
      
			// Percorrer os genes do cromossoma de forma a obter uma lista com os índices dos genes de valor mínimo (maior prioridade) (...)
			double min = ranking.at(ind_min++).first;
      
			// (...) the array with the genes' values is ordered, so it's just necessary to iterate over it until a new maximum is found
			if( ind_min < ranking.size() ) {
			
				for ( ; ranking.at(ind_min).first == min; ind_min++) {
				
					if( std::find(trips.begin(), trips.end(), ranking.at(ind_min).second) != trips.end() || 
					  	std::find(blackList.begin(), blackList.end(), ranking.at(ind_min).second) != blackList.end() )
						minValues.push_back(ranking.at(ind_min).second);  /* if the vertice is neither marked to be visited, nor blacklisted, 
																																add its index in the minimum values list */
				}
			}
			
			// if there are several genes with the same (maximum) value
			if(minValues.size() > 1) {

				// chose a gene randomly
				random = r.randInt(minValues.size());
				topPriorityGene = minValues.at(random);
			}
			else { // if there's only one gene with the maximum value
				
				topPriorityGene = minValues.at(0);
				random = 0;
			}
			
			/* Tuple with the data necessary to update the selected routes at the end of an iteration of the decoder 
			
					1st position: car
					2nd position: position of the car's trip
					3rd position: new distance od the trip
			*/
			data2change = data2change = where2insert(topPriorityGene, routes, times);

			 // se o gene escolhido couber numa rota
	    if ( std::get<1>(data2change) != -1 )
				fl = true;  // indicar a escolha dum gene
			else {
  
				counter = 0;
  
				// percorrer todos os vértices de prioridade máxima ainda não escolhidos até um caber na viagem
				for(unsigned k=0 ; !fl && (k+1)<minValues.size() ; k++) {
  				
					//std::cout << "k: " << k << std::endl;
				  //std::cout << "minValues.size: " << minValues.size() << std::endl;
					unsigned kk;

					// colocar o seu índice no array de genes com prioridade máxima, na blacklist
					auxblackList.push_back(random + counter);
					
					// std::cout << "random: " << random << std::endl;
					// std::cout << "counter: " << counter << std::endl;
  				// std::cout << "minValues.size: " << minValues.size() << std::endl;
					blackList.push_back(minValues.at(random + counter));

					// escolher novo gene com prioridade máxima, de forma aleatória
					random = r.randInt(minValues.size() - (k+1));

					// ordenar os arrays blackList e auxblackList
					std::sort(auxblackList.begin(), auxblackList.end());
					std::sort(blackList.begin(), blackList.end());

					// ciclo para contar o número de posições que é preciso saltar do array minValues, por estarem na blacklist
					for(counter=0, kk=0 ; kk <= k ; kk++)
						if(auxblackList.at(kk) <= random+counter)
							counter++;

  				// std::cout << "random: " << random << std::endl;
  				// std::cout << "kk: " << kk << std::endl;
					topPriorityGene = minValues.at(random + counter);	// guardar o índice do novo gene escolhido aleatoriamente
					// std::cout << "TOP PRIORITY GENE: " << topPriorityGene << std::endl;
  
/*  
					min_add = deadline + 1;
					trip = -1;
					pos = -1;
					new_dist = -1;	
*/  
  
					data2change = where2insert(topPriorityGene, routes, times);
					/*
					 * Ciclo utilizado para implementar a segunda restrição, que impôe que o vértice escolhido para 
					 * entrar nas viagens vai ser colocado no local que mais favorece a viagem. O ciclo externo itera
					 * as viagens e o interno itera as posições de cada viagem.  
					 */
/*					for(int j=0 ; j<cars ; j++) {
  
						for(unsigned i=0 ; i<routes.at(j).size() + 1 ; i++) {
  
						// extra distance covered by the car due to the insertion of topPriorityGene
							double  add = Clients::addedTime(routes.at(j), topPriorityGene, i);
							double n = times.at(j) + add;  // new distance covered by the car, with the insertion of the topPriorityGene
  
							if( n <= deadline && add < min_add ) {
              
								min_add = add;
								std::get<0>(data2change) = j;
								std::get<1>(data2change) = i;
								std::get<2>(data2change) = n;
							}
						}
					}
 */
					// se o gene escolhido couber numa rota
			    if (std::get<1>(data2change) != -1)							
						fl = true;  // assinalar a escolha dum gene	
				}  // for(unsigned k=0 ; !fl && (k+1)<minValues.size() ; k++)
  
				/* Clear the contents of the auxblackList */
				auxblackList.clear();
				auxblackList.shrink_to_fit();
  
				blackList.push_back(minValues.at(random + counter));
			}  // else
			
			// Clear the contents of the vector with the indexes of high priority
			minValues.clear();
			minValues.shrink_to_fit();
			
		}  //	while( !fl && blackList.size() + trips.size() < ranking.size() )
  
		
		/* Clear the contents of the blackList, so that this structure is empty in the next cycle iteration */  
		blackList.clear();
		blackList.shrink_to_fit();

  
//std::cout << "fl: " << fl << std::endl;

	// se um gene tiver sido seleccionado para ser colocado na viagem
		if(fl) {

	//System.out.println("times.size(): " + times.size() + "\ntrip: " + trip + "\nmin_add: " + min_add);
	
			times.at( std::get<0>(data2change) ) = std::get<2>(data2change); // update times with the new duration of the route		

			int prev = topPriorityGene; 	// update routes with the new selected vertice to be visited (step 1)
//std::cout << "pos: " << pos << std::endl;  

			// update routes with the new selected vertice to be visited (step 2)
			for(unsigned j=std::get<1>(data2change) ; j<routes.at(std::get<0>(data2change)).size() ; j++) {
  
				int next = routes.at(std::get<0>(data2change)).at(j);
  
				routes.at(std::get<0>(data2change)).at(j) = prev;
				prev = next;
			}
  
			// update routes with the new selected vertice to be visited (step 3)
			routes.at(std::get<0>(data2change)).push_back(prev);
  

			// atualizar array trips com o novo gene, sendo que é descurada a ordenação
			trips.push_back(topPriorityGene);

			// atualizar o peso do alelo
			// topInd(ranking).setAllele(topPriorityGene, trips.size());
  
			// mudar o valor da flag que assinala a escolha dum gene para colocar na viagem para recomeçar novo processo
			fl = false;
  
/*		mutex.lock();	
		std::cout << "top priority gene: " << topPriorityGene << std::endl;
		std::cout << "SCORE: " + vertices.at(topPriorityGene).getScore() << std::endl;
  	
		std::cout << "--------------- VIAGENS ---------------" << std::endl;
		for (unsigned i = 0; i < routes.size(); i++) {
			
			std::cout << "Route " << i << ": ";
			
			for (unsigned j = 0; j < routes[i].size(); j++) {
			
				std::cout << (routes[i][j]);
			
				if (j != routes[i].size() - 1)
					std::cout << "->";
			}
			
			std::cout << std::endl;
		}
		getchar();
		mutex.unlock(); */
  
			// atualiza o valor do fitness
			fitness += vertices.at(topPriorityGene).getScore();
		}
		else  // caso contrário (se nenhum dos vértices de alta prioridade couber no fim da viagem)
			flag = false;  // atualizar a flag para sair do ciclo
	

			
	}  // while(flag)

	// A mutex guarantees that the code between lock() and unlock() methods is atomic
	mutex.lock();
		
	// If a new maximum fitness was found, update the global routes vector, and print the fitness
	if (fitness > totalBest) {
	
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
	
		totalBest = fitness;
  
		std::cout <<  totalBest << std::endl;
	}

	mutex.unlock();

	return maxFit - fitness;
}

/* Checks if a client has been visited */
int CtopDecoder::exist(std::vector<int> visited, int client) const {
	
	for (std::vector<int>::iterator it = visited.begin(); it != visited.end(); ++it)
		if ( (*it) == client ) 
			return 1;
	
	return 0;
}

/* The chosen vertice (topPriorityGene) will be inserted in the place that favours the routes the most. An example is given below.
 * 
 * Consider the vertice 4 has been chosen to be inserted in a trip, 
 * and that we have two cars, having at the moment the following trips: 
 * 
 * Trip 1 : O -> 3 -> D (time: 5)
 * Trip 2 : O -> D (time: 0)
 * 
 * Its insertion will be attempted in all the positions, like explained below: 
 * 
 * Trip 1'  : Origin -> 4 -> 3 -> Destination (time: 6)
 * Trip 1'' : Origin -> 3 -> 4 -> Destination (time: 7)
 * Trip 2'  : Origin -> 4 -> Destination (time: 5)
 * 
 * Although the trip 2' is the shortest, the algorithm will choose the trip 1', because the time added to the trip is shorter:
 * 
 * 					time_added(trip 1') < time_added(trip 1'') < time_added(trip 2')
 */
std::tuple<int, int, double> CtopDecoder::where2insert(int topPriorityGene, std::vector<std::vector<int>> routes, 
																																						std::vector<double> times) const {

	int min_add = deadline + 1;	// stores the minimum distance added to the route with the insertion of one vertice
	std::tuple<int, int, double> data2change = std::make_tuple(-1, -1, -1);

	/* O ciclo externo itera as viagens e o interno itera as posições de cada viagem */
	for(int j=0; j<cars; j++) {
  
	/*				std::cout << "-------------------------------------" << std::endl;
		std::cout << "j: " << j << "   routes[j].size(): " << routes.at(j).size() << "\ntopPriorityGene: " 
									<< topPriorityGene << "\ntimes.at(j): " << times.at(j) << std::endl;
	*/
			for(unsigned i=0 ; i<routes.at(j).size() + 1 ; i++) {
  
				/* extra distance covered by the car due to the insertion of topPriorityGene */
				double  add = Clients::addedTime(routes.at(j), topPriorityGene, i);  				
				double n = times.at(j) + add;  // new distance covered by the car, with the insertion of the topPriorityGene
  
		//						std::cout << "\nn: " << n << "\nadded: " << add << std::endl << std::endl;
  
				/* In order to change the insertion algorithm, so that a new vertice is inserted in a route's position 
				so that the chosen route will have the smallest time among all the routes, 
				the 'add' variable shall be replaced by the 'n' variable */
				if( n <= deadline && add < min_add ) { 
  
					min_add = add;
					std::get<0>(data2change) = j;
					std::get<1>(data2change) = i;
					std::get<2>(data2change) = n;
				}
		}
  
		//				std::cout << "-------------------------------------" << std::endl;
	}
	
	return data2change;
}
