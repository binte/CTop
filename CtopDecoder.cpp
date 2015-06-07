#include <iostream>
#include <sstream>  
#include <fstream>
#include <string>
#include "CtopDecoder.h"
#include <ctime>

CtopDecoder::CtopDecoder() {}
CtopDecoder::~CtopDecoder() {}

double totalBest;

double CtopDecoder::decode(const std::vector< double >& chromosome) const {

	int CAPACIDADEMAX = 0;
	int DISTANCIAMAX = 0;
	int XCOORDENADA[100];
	int YCOORDENADA[100];
	int CAPACIDADE[100];
	int PREMIO[100];
	int NCARROS = 0;
	int NCLIENTES = 0;
	int evolve = 0;
	int p = 0;

	std::string line;
	std::ifstream myfile;
	myfile.open("dadosctop.dat");
	int lineIdx = 0;
	while (!myfile.eof())
	{
		lineIdx++;
		std::string value;
		std::size_t pos;

		switch (lineIdx)
		{
		case(1) :
			std::getline(myfile, line);
			value = line;
			pos = value.find(";");
			if (pos != std::string::npos)
				value = value.substr(0, pos);
			p = std::stoi(value);
			break;
		case(2) :
			std::getline(myfile, line);
			value = line;
			pos = value.find(";");
			if (pos != std::string::npos)
				value = value.substr(0, pos);
			evolve = std::stoi(value);
			break;
		case(3) :
			std::getline(myfile, line);
			value = line;
			pos = value.find(";");
			if (pos != std::string::npos)
				value = value.substr(0, pos);
			NCLIENTES = std::stoi(value);
			break;
		case(4) :
			std::getline(myfile, line);
			value = line;
			pos = value.find(";");
			if (pos != std::string::npos)
				value = value.substr(0, pos);
			NCARROS = std::stoi(value);
			break;
		case(5) :
			std::getline(myfile, line);
			value = line;
			pos = value.find(";");
			if (pos != std::string::npos)
				value = value.substr(0, pos);
			DISTANCIAMAX = std::stoi(value);
			break;
		case(6) :
			std::getline(myfile, line);
			value = line;
			pos = value.find(";");
			if (pos != std::string::npos)
				value = value.substr(0, pos);
			CAPACIDADEMAX = std::stoi(value);
			break;
		default:
			std::getline(myfile, line);
			std::string stringcut = line;
			value = stringcut;

			for (int i = 0; i < 3; i++){
				pos = stringcut.find(";");
				if (pos != std::string::npos){
					value.replace(pos, value.length(), "");

					int xc = 0;

					switch (i)
					{
					case 0:
						xc = std::stoi(value);
						XCOORDENADA[lineIdx - 7] = xc;
						break;
					case 1:
						xc = std::stoi(value);
						YCOORDENADA[lineIdx - 7] = xc;
						break;
					case 2:
						xc = std::stoi(value);
						CAPACIDADE[lineIdx - 7] = xc;
						break;
					default:
						break;
					}

					stringcut = stringcut.substr(pos + 1);
					value = stringcut;
				}
			}

			int v = std::stoi(value);
			PREMIO[lineIdx - 7] = v;
			break;
		}

	}
	myfile.close();

	int totalPremio = 0;
	for (int j = 0; j < chromosome.size(); j++){
		totalPremio += PREMIO[j];
	}

	std::vector< std::pair< double, unsigned > > ranking(chromosome.size());
	for (unsigned i = 0; i < chromosome.size(); ++i) {
		ranking[i] = std::pair< double, unsigned >(chromosome[i], i);
	}

	std::sort(ranking.begin(), ranking.end());

	double totalTodasRotas = 0;
	std::vector<int> visitadosRotas;
	std::vector<std::vector<int>> visitadosPorRotas;

	//PARA TODOS OS CARROS
	for (int j = 0; j < NCARROS; j++){

		double totalDistancia = 0;
		double totalCapacidade = 0;
		double totalRota = 0;

		std::vector<int> visitados;
		//SE ESTA A INICIAR SAI DO DEPOSITO INICIAL
		for (int i = 0; i < ranking.size(); i++){

			int clienteDestino = ranking[i].second;
			double depositoInicial = calcularDistancia(35, 35, XCOORDENADA[clienteDestino], YCOORDENADA[clienteDestino]);
			double depositoFinal = calcularDistancia(XCOORDENADA[clienteDestino], YCOORDENADA[clienteDestino], 35, 35);
			if (depositoInicial <= (DISTANCIAMAX - depositoFinal)
				&& (totalCapacidade + CAPACIDADE[clienteDestino]) <= CAPACIDADEMAX
				&& (visitadosRotas.size() <= 0 || existe(visitadosRotas, clienteDestino) != 1)
				){
				totalDistancia += depositoInicial;
				totalCapacidade += CAPACIDADE[clienteDestino];
				totalRota += PREMIO[clienteDestino];
				visitados.push_back(clienteDestino);
				visitadosRotas.push_back(clienteDestino);
				break;
			}
		}

		if (visitadosRotas.size() > 0){

			//ATE DISTANCIA MAXIMA MENOS DEPOSITO FINAL OU CAPACIDADE MAXIMA SER ULTRAPASSADA
			for (int i = 0; i < ranking.size(); i++){

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

	if (totalTodasRotas > totalBest){
		totalBest = totalTodasRotas;

		int totalCapacidade = 0;
		for (int j = 0; j < ranking.size(); j++){
			totalCapacidade += CAPACIDADE[j];
		}

		int totalx = 0;
		for (int j = 0; j < ranking.size(); j++){
			totalx += XCOORDENADA[j];
		}

		int totaly = 0;
		for (int j = 0; j < ranking.size(); j++){
			totaly += YCOORDENADA[j];
		}

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
		output << "MAXPREMIO POSSIVEL: " << totalPremio << std::endl << std::endl;

		for (int i = 0; i < visitadosPorRotas.size(); i++){
			output << "Rota " << i + 1 << ": ";
			for (int j = 0; j < visitadosPorRotas[i].size(); j++){
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

	return totalPremio - totalTodasRotas;
}

double CtopDecoder::calcularDistancia(int x_1, int y_1, int x_2, int y_2) const {
	return sqrt(pow((x_1 - x_2), 2) + pow((y_1 - y_2), 2));
}

int CtopDecoder::existe(
	std::vector<int> visitados, int cliente) const {
	for (std::vector<int>::iterator it = visitados.begin(); it != visitados.end(); ++it){
		if ((*it) == cliente) return 1;
	}
	return 0;
}