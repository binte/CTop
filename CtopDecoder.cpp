#include <iostream>
#include <sstream>  
#include <fstream>
#include <string>
#include <ctime>

#include "CtopDecoder.h"

int extern CAPACIDADEMAX;
int extern DISTANCIAMAX;
int extern XCOORDENADA[100];
int extern YCOORDENADA[100];
int extern CAPACIDADE[100];
int extern PREMIO[100];
int extern CARS;
int extern NCLIENTES;
int extern evolve;
int extern p;
int extern totalpremio;
double extern totalBest;

std::mutex extern mutex;
std::vector<std::vector<int>> extern bestRoutes;


CtopDecoder::CtopDecoder() {}
CtopDecoder::~CtopDecoder() {}


double CtopDecoder::decode(const std::vector< double >& chromosome) const {
	
	/* Vector of pairs (double, unsigned) with size chromosome.size() */
	std::vector< std::pair< double, unsigned > > ranking(chromosome.size());
	
	/* Local vector with the routes, for each iteration of the decoder */
	std::vector<std::vector<int>> routes;
	
	for (unsigned i = 0; i < chromosome.size(); ++i)
		ranking[i] = std::pair< double, unsigned >(chromosome[i], i);
		
	std::sort(ranking.begin(), ranking.end());

	double totalTodasRotas = 0;
	std::vector<int> visitadosRotas;

	
	// Iterate the cars
	for (int j = 0; j < CARS; j++) {

		double totalDistancia = 0;
		double totalCapacidade = 0;
		double totalRota = 0;

		std::vector<int> visitados;
		
		//SE ESTA A INICIAR SAI DO DEPOSITO INICIAL
		for (unsigned i = 0; i < ranking.size(); i++) {

			int clienteDestino = ranking[i].second;
			double depositoInicial = calcularDistancia(35, 35, XCOORDENADA[clienteDestino], YCOORDENADA[clienteDestino]);
			double depositoFinal = calcularDistancia(XCOORDENADA[clienteDestino], YCOORDENADA[clienteDestino], 35, 35);
			
/*			std::cout << "\ndepositoInicial: " << depositoInicial << 
									 "\nDISTANCIAMAX: " << DISTANCIAMAX <<
			 						 "\ndepositoFinal: " << depositoFinal <<
									 "\ntotalCapacidade: " << totalCapacidade <<
									 "\nCAPACIDADE[clienteDestino]: " << CAPACIDADE[clienteDestino] <<
									 "\nclienteDestino: " << clienteDestino <<
									 "\nCAPACIDADEMAX: " << CAPACIDADEMAX;
			
			getchar();*/
			
			
			if (depositoInicial <= (DISTANCIAMAX - depositoFinal)
				&& (totalCapacidade + CAPACIDADE[clienteDestino]) <= CAPACIDADEMAX
				&& (visitadosRotas.size() <= 0 || existe(visitadosRotas, clienteDestino) != 1)) {
					
				totalDistancia += depositoInicial;
				totalCapacidade += CAPACIDADE[clienteDestino];
				totalRota += PREMIO[clienteDestino];
				visitados.push_back(clienteDestino);
				visitadosRotas.push_back(clienteDestino);
				
				break;
			}
		}

		if (visitadosRotas.size() > 0) {

			//ATE DISTANCIA MAXIMA MENOS DEPOSITO FINAL OU CAPACIDADE MAXIMA SER ULTRAPASSADA
			for (unsigned int i = 0; i < ranking.size(); i++) {

				int clienteDestino = ranking[i].second;
				
				double distancia = calcularDistancia(XCOORDENADA[visitadosRotas.back()], YCOORDENADA[visitadosRotas.back()],
					XCOORDENADA[clienteDestino], YCOORDENADA[clienteDestino]);
				double depositoFinal = calcularDistancia(XCOORDENADA[clienteDestino], YCOORDENADA[clienteDestino], 35, 35);

				if ((totalDistancia + distancia) <= (DISTANCIAMAX - depositoFinal)
					&& (totalCapacidade + CAPACIDADE[clienteDestino]) <= CAPACIDADEMAX
					&& existe(visitadosRotas, clienteDestino) != 1){
				
					totalDistancia += distancia;
					totalCapacidade += CAPACIDADE[clienteDestino];
					totalRota += PREMIO[clienteDestino];
					visitados.push_back(clienteDestino);
					visitadosRotas.push_back(clienteDestino);
				}
			}

			//SE ULTRAPASSA DISTANCIA MAXIMA MENOS A DISTANCIA PARA O DEPOSITO FINAL
			double depositoFinal = calcularDistancia(XCOORDENADA[visitadosRotas.back()], YCOORDENADA[visitadosRotas.back()], 35, 35);
			totalDistancia += depositoFinal;
		}
		
		// Insert new route at the tail of the routes vector for this iteration
		routes.push_back(visitados);

		totalTodasRotas += totalRota;
	}

	// A mutex guarantees that the code between lock() and unlock() methods is atomic
	mutex.lock();
		
	// If a new maximum fitness was found, update the global routes vector, and print the fitness
	if (totalTodasRotas > totalBest) {
	
		/* Clear the contents of the bestRoutes vector, thus allowing new best routes, if found, 
		to be inserted after being calculated in this iteration of the constructive algorithm  */
		for ( unsigned int i=0 ; i<bestRoutes.size() ; i++ ) {
  
			bestRoutes.at(i).clear();
			bestRoutes.at(i).shrink_to_fit();
		}
  
		if(bestRoutes.size() != 0) {
  
			bestRoutes.clear();
			bestRoutes.shrink_to_fit();
		}
	
		// copy the routes obtained in this iteration of the decoder, replacing the previous best routes
		for ( unsigned int i=0 ; i<routes.size() ; i++ )
			bestRoutes.push_back(routes.at(i)); 
	
		totalBest = totalTodasRotas;
  
		std::cout << totalBest << std::endl;
	}
		
	mutex.unlock();

	return totalpremio - totalTodasRotas;
}

double CtopDecoder::calcularDistancia(int x_1, int y_1, int x_2, int y_2) const {
	
	return sqrt(pow((x_1 - x_2), 2) + pow((y_1 - y_2), 2));
}

int CtopDecoder::existe(std::vector<int> visitados, int cliente) const {
	
	for (std::vector<int>::iterator it = visitados.begin(); it != visitados.end(); ++it)
		if ( (*it) == cliente ) 
			return 1;
	
	return 0;
}
