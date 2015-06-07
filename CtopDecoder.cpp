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
int extern NCARROS;
int extern NCLIENTES;
int extern evolve;
int extern p;
int extern totalpremio;

CtopDecoder::CtopDecoder() {}
CtopDecoder::~CtopDecoder() {}

double totalBest;

double CtopDecoder::decode(const std::vector< double >& chromosome) const {

	std::vector< std::pair< double, unsigned > > ranking(chromosome.size());
	
	for (unsigned i = 0; i < chromosome.size(); ++i)
		ranking[i] = std::pair< double, unsigned >(chromosome[i], i);

	std::sort(ranking.begin(), ranking.end());

	double totalTodasRotas = 0;
	std::vector<int> visitadosRotas;
	std::vector<std::vector<int>> visitadosPorRotas;


	//PARA TODOS OS CARROS
	for (int j = 0; j < NCARROS; j++) {

		double totalDistancia = 0;
		double totalCapacidade = 0;
		double totalRota = 0;

		std::vector<int> visitados;
		
		//SE ESTA A INICIAR SAI DO DEPOSITO INICIAL
		for (unsigned i = 0; i < ranking.size(); i++){

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
			for (unsigned int i = 0; i < ranking.size(); i++){

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

			//SE ULTRAPASSA DISTANCIA MAXIMA MENOS A DISTANCIA PRA O DEPOSITO FINAL
			double depositoFinal = calcularDistancia(XCOORDENADA[visitadosRotas.back()], YCOORDENADA[visitadosRotas.back()], 35, 35);
			totalDistancia += depositoFinal;
		}

		visitadosPorRotas.push_back(visitados);
		totalTodasRotas += totalRota;

	}

	if (totalTodasRotas > totalBest) {
		totalBest = totalTodasRotas;

		int totalCapacidade = 0;
		for (unsigned j = 0; j < ranking.size(); j++)
			totalCapacidade += CAPACIDADE[j];

		int totalx = 0;
		for (unsigned j = 0; j < ranking.size(); j++)
			totalx += XCOORDENADA[j];

		int totaly = 0;
		for (unsigned j = 0; j < ranking.size(); j++)
			totaly += YCOORDENADA[j];

		std::ofstream output;
		output.open("resultado.txt");

		time_t     now = time(0);
		struct tm  tstruct;
		char       buf[80];
		tstruct = *localtime(&now);
		strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
		output << buf << std::endl << std::endl;
		

		output << "POPULACAO: " << p << std::endl;
		output << "EVOLUCOES: " << evolve << std::endl;
		output << "CLIENTES: " << ranking.size() << std::endl;
		output << "VIATURAS: " << NCARROS << std::endl;
		output << "DISTANCIAMAX: " << DISTANCIAMAX << std::endl;
		output << "TOTAL XCOORDENADA: " << totalx << std::endl;
		output << "TOTAL YCOORDENADA: " << totaly << std::endl;
		output << "CAPACIDADEMAX: " << CAPACIDADEMAX << std::endl;
		output << "TOTAL CAPACIDADE: " << totalCapacidade << std::endl;
		output << "MAXPREMIO POSSIVEL: " << totalpremio << std::endl << std::endl;

		for (unsigned i = 0; i < visitadosPorRotas.size(); i++) {
			
			output << "Rota " << i + 1 << ": ";
			
			for (unsigned j = 0; j < visitadosPorRotas[i].size(); j++) {
			
				output << (visitadosPorRotas[i][j] + 1);
			
				if (j != visitadosPorRotas[i].size() - 1)
					output << "->";
			}
			
			output << std::endl;
		}
		
		output << "Total Premio: " << totalBest << std::endl << std::endl;
		output.close();
		std::cout << "NOVA SOLUCAO ENCONTRADA: " << totalBest << std::endl;
	}

	return totalpremio - totalTodasRotas;
}

double CtopDecoder::calcularDistancia(int x_1, int y_1, int x_2, int y_2) const {
	return sqrt(pow((x_1 - x_2), 2) + pow((y_1 - y_2), 2));
}

int CtopDecoder::existe(std::vector<int> visitados, int cliente) const {
	
	for (std::vector<int>::iterator it = visitados.begin(); it != visitados.end(); ++it)
		if ((*it) == cliente) return 1;
	
	return 0;
}
