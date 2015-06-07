#include <fstream>
#include <iostream>
#include <string>
#include "CtopDecoder.h"
#include "MTRand.h"
#include "BRKGA.h"


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
int totalpremio = 0;


int main(int argc, char* argv[]) {

	if(argc != 2) {

		std::cerr << "\nWrong parameter number\n" << std::endl;

		return EXIT_FAILURE;
	}

	time_t     inicio = time(0);
	struct tm  tstructInicio;
	char       bufInicio[80];
	tstructInicio = *localtime(&inicio);
	strftime(bufInicio, sizeof(bufInicio), "%Y-%m-%d %X", &tstructInicio);
	std::cout << bufInicio << std::endl << std::endl;

	const double pe = 0.20;		// fraction of population to be the elite-set
	const double pm = 0.10;		// fraction of population to be replaced by mutants
	const double rhoe = 0.70;	// probability that offspring inherit an allele from elite parent
	const unsigned K = 3;		// number of independent populations 3
	const unsigned MAXT = 4;	// number of threads for parallel decoding 2

	std::string line;
	std::ifstream myfile;
	myfile.open(argv[1]);

	if (myfile.is_open()){

		int lineIdx = 0;
		
		while (!myfile.eof()) {

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

				for (int i = 0; i < 3; i++) {
					
					pos = stringcut.find(";");
			
					if (pos != std::string::npos) {

						int xc = 0;

						value.replace(pos, value.length(), "");

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
					}  // if (pos != std::string::npos)
				}  // for (int i = 0; i < 3; i++)

				int v = std::stoi(value);
				PREMIO[lineIdx - 7] = v;
				totalpremio += v;
			
				break;
			}  // switch (lineIdx)
		}  // while (!myfile.eof())
		
		myfile.close();

		const long unsigned rngSeed = 0;	// seed to the random number generator
		MTRand rng(rngSeed);				// initialize the random number generator
		CtopDecoder decoder;				// initialize the decoder
		BRKGA< CtopDecoder, MTRand > algorithm(NCLIENTES, p, pe, pm, rhoe, decoder, rng, K, MAXT);

		algorithm.evolve(evolve);

		time_t     nowFim = time(0);
		struct tm  tstructFim;
		char       bufFim[80];
		tstructFim = *localtime(&nowFim);
		strftime(bufFim, sizeof(bufFim), "%Y-%m-%d %X", &tstructFim);
		std::cout << bufFim << std::endl << std::endl;

		std::cout << std::endl;
		std::cout << "RESULTADO FINAL" << std::endl;
		std::cout << "INICIO: " << bufInicio << std::endl;
		std::cout << "FIM: " << bufFim << std::endl;
		std::cout << "O MELHOR RESULTADO PARA A FUNCAO OBJECTIVA = "
			<< (totalpremio - algorithm.getBestFitness()) << std::endl;

		std::fstream output;
		output.open("resultado.txt", std::ios::in | std::ios::out | std::ios::app);
		output << std::endl;
		output << "RESULTADO FINAL" << std::endl;
		output << "INICIO: " << bufInicio << std::endl;
		output << "FIM: " << bufFim << std::endl;
		output << "O MELHOR RESULTADO PARA A FUNCAO OBJECTIVO = "
			<< (totalpremio - algorithm.getBestFitness()) << std::endl;
		output.close();

	}
	else {
		std::cout << "dadosctop.dat EM FALTA.";
	}

	return 0;
}