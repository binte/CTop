#include "CtopDecoder.h"


CtopDecoder::CtopDecoder() {}
CtopDecoder::~CtopDecoder() {}


double CtopDecoder::decode(const std::vector< double >& chromosome) const {
	
	int random, topPriorityGene = -1, trip = -1, pos = -1, counter;  // counter: number of positions to ignore, due to blacklisting
	bool flag = true, fl = false;  // fl flags the choice of a gene, and flag indicates if it's possible to insert more vertices
	double fitness = 0, min_dist = 0;
	MTRand r;
	std::vector<int> blackList;  // indexes of the discarded genes during a trip
	std::vector<int> auxblackList;  // indexes (within blackList) of the high priority genes that can't be taken
	std::vector<int> viagens; // visited genes
	std::vector<int> minValues;  // indexes of the high priority genes
	std::vector<std::vector<int>> routes(cars); 	// contains the routes for each iteration, including visiting the deposits
	std::vector<int> times(cars);  // current duration of each of the routes
	
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
		while( !fl && blackList.size() + viagens.size() < ranking.size() ) {
  
			//std::cout << "blackList.size(): " << blackList.size() << std::endl;
			//std::cout << "viagens.size(): " << viagens.size() << std::endl;
			//std::cout << "ranking.size(): " << ranking.size() << std::endl;

			unsigned ind_min=0;

			//std::cout << "ind_min: " << ind_min << std::endl;

			// Find the gene with the highest priority that is neither visited nor blacklisted (...)
			while (std::find(blackList.begin(), blackList.end(), ranking.at(ind_min).second) != blackList.end() || 
				  	 std::find(viagens.begin(), viagens.end(), ranking.at(ind_min).second) != viagens.end() )
				ind_min++;

			// (...) and save its index
			minValues.push_back(ranking.at(ind_min).second);
      
			// Percorrer os genes do cromossoma de forma a obter uma lista com os índices dos genes de valor mínimo (maior prioridade) (...)
			double min = ranking.at(ind_min++).first;
      
			// (...) the array with the genes' values is ordered, so it's just necessary to iterate over it until a new maximum is found
			if( ind_min < ranking.size() ) {
			
				for ( ; ranking.at(ind_min).first == min; ind_min++) {
				
					if( std::find(viagens.begin(), viagens.end(), ranking.at(ind_min).second) != viagens.end() || 
					  	std::find(blackList.begin(), blackList.end(), ranking.at(ind_min).second) != blackList.end() )
						minValues.push_back(ranking.at(ind_min).second);  // if the vertice is neither marked to be visited, nor blacklisted, add its index in the minimum values list
				}
			}
			
			// se existirem vários genes com o mesmo valor (máximo)
			if(minValues.size() > 1) {

				// escolher um gene de forma aleatória
				random = r.randInt(minValues.size());
				topPriorityGene = minValues.at(random);
			}
			else { // se apenas existir um gene com o valor máximo
				
				topPriorityGene = minValues.at(0);
				random = 0;
			}
			
  
			// variável auxiliar utilizada para guardar a distância mínima calculada ao iterar o próximo ciclo 
			min_dist = deadline + 1;
  
			// variável auxiliar utilizada para guardar a viagem na qual ocorreu a distância mínima
			trip = -1;
  
			// variável auxiliar utilizada para guardar a posição da viagem na qual ocorreu a distância mínima
			pos = -1;
  

			/*
			 * Ciclo utilizado para implementar a segunda restrição, que impôe que o vértice escolhido para 
			 * entrar nas viagens vai ser colocado no local que mais favorece a viagem. O ciclo externo itera
			 * as viagens e o interno itera as posições de cada viagem.  
			 */
			for(int j=0; j<cars; j++) {
  
				//std::cout << "routes.at(j).size(): " << routes.at(j).size() << std::endl;
				//getchar();

					for(unsigned i=0 ; i<routes.at(j).size() + 1 ; i++) {
  
						double aux = Clients::calc_new_vertice(routes.at(j), topPriorityGene, i, times.at(j));
  
						//std::cout << "topPriorityGene: " << topPriorityGene << "\ntimes.get(j): "<< times.at(j) << "\ncalculated: " << aux << "\nmin_dist: " << min_dist << std::endl;
  
						if( aux <= deadline && aux < min_dist ) {
          	
							min_dist = aux;
							trip = j;
							pos = i;
						}
				}
			}

			 // se o gene escolhido couber numa rota
	    if (pos != -1)
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
  
  
					min_dist = deadline + 1;
					trip = -1;
					pos = -1;			
  
  
					/*
					 * Ciclo utilizado para implementar a segunda restrição, que impôe que o vértice escolhido para 
					 * entrar nas viagens vai ser colocado no local que mais favorece a viagem. O ciclo externo itera
					 * as viagens e o interno itera as posições de cada viagem.  
					 */
					for(int j=0 ; j<cars ; j++) {
  
						for(unsigned i=0 ; i<routes.at(j).size() + 1 ; i++) {
  
							double aux = Clients::calc_new_vertice(routes.at(j), topPriorityGene, i, times.at(j));
		//System.out.println("topPriorityGene: " + topPriorityGene + "\ntimes.at(j): " + times.at(j) + "\ncalculated: " + aux);
  
							if( aux <= deadline && aux <= min_dist ) {
  
								min_dist = aux;
								trip = j;
								pos = i;
							}
						}
					}
  
					// se o gene escolhido couber numa rota
			    if (pos != -1)							
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
			
		}  //	while( !fl && blackList.size() + viagens.size() < ranking.size() )
  
		
		/* Clear the contents of the blackList, so that this structure is empty in the next cycle iteration */  
		blackList.clear();
		blackList.shrink_to_fit();

  
//std::cout << "fl: " << fl << std::endl;

	// se um gene tiver sido seleccionado para ser colocado na viagem
		if(fl) {

	//System.out.println("times.size(): " + times.size() + "\ntrip: " + trip + "\nmin_dist: " + min_dist);
  
			times.at(trip) = min_dist; //atualizar a variável times com o novo tempo de duração da rota
  
			int prev = topPriorityGene;
  
			// atualizar rotas com o novo gene
			for(unsigned j=pos ; j<routes.at(trip).size() ; j++) {
  
				int next = routes.at(trip).at(j);
  
				routes.at(trip).at(j) = prev;
				prev = next;
			}
  
			// terminar de atualizar array routes com o novo gene
			routes.at(trip).push_back(prev);
  

			// atualizar array viagens com o novo gene, sendo que é descurada a ordenação
			viagens.push_back(topPriorityGene);

			// atualizar o peso do alelo
			// topInd(ranking).setAllele(topPriorityGene, viagens.size());
  
			// mudar o valor da flag que assinala a escolha dum gene para colocar na viagem para recomeçar novo processo
			fl = false;
  /*
			mutex.lock();	
		std::cout << "top priority gene: " << topPriorityGene << std::endl;
		std::cout << "SCORE: " + vertices.at(topPriorityGene).getScore() << std::endl;
  	
		std::cout << "--------------- VIAGENS ---------------" << std::endl;
		for (unsigned h=0 ; h < viagens.size() ; h++)
			std::cout << "indice " << h << ": " << viagens.at(h) << std::endl;
		std::cout << "---------------------------------------" << std::endl;
		getchar();
			mutex.unlock();
  */
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
