#include <list>
#include <fstream>
#include <iostream>
#include <string>

#include "CtopDecoder.h"
#include "MTRand.h"
#include "BRKGA.h"


int maxCapacity = 0;
int deadline = 0;
int cars = 0;
int maxFit = 0;
double totalBest = 0;

MTRand rng;

std::vector< std::pair<float, float> > coordenadas;
std::vector<int> capacities;
std::vector<int> prizes;

std::mutex mutex;
std::vector<std::vector<int>> bestRoutes;


int main(int argc, char* argv[]) {

	if(argc != 2) {

		std::cerr << "\nWrong parameter number\n" << std::endl;

		return EXIT_FAILURE;
	}

	int p = 0;
	int evols = 0;
	int clients = 0;
	time_t     inicio = time(0);
	struct tm  tstructInicio;
	char       bufInicio[80];
	tstructInicio = *localtime(&inicio);
	strftime(bufInicio, sizeof(bufInicio), "%Y-%m-%d %X", &tstructInicio);

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
			
				evols = std::stoi(value);
			
				break;
			
			case(3) :
				std::getline(myfile, line);
				value = line;
				pos = value.find(";");
			
				if (pos != std::string::npos)
					value = value.substr(0, pos);
			
				clients = std::stoi(value);
				coordenadas.reserve(clients);  // allocate space for the coordinates of each vertice/client
				capacities.reserve(clients);	// allocate space for the capacities of each client
				prizes.reserve(clients);  // allocate space for the prizes given by reaching each client
			
				break;
			
			case(4) :
				std::getline(myfile, line);
				value = line;
				pos = value.find(";");
			
				if (pos != std::string::npos)
					value = value.substr(0, pos);
			
				cars = std::stoi(value);
			
				break;
			
			case(5) :
				std::getline(myfile, line);
				value = line;
				pos = value.find(";");
			
				if (pos != std::string::npos)
					value = value.substr(0, pos);
			
				deadline = std::stoi(value);
			
				break;
				
			case(6) :
				std::getline(myfile, line);
				value = line;
				pos = value.find(";");
			
				if (pos != std::string::npos)
					value = value.substr(0, pos);
			
				maxCapacity = std::stoi(value);
			
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
								coordenadas[lineIdx - 7].first = xc;
								
								break;
							
							case 1:
								xc = std::stoi(value);
								coordenadas[lineIdx - 7].second = xc;
							
								break;
							
							case 2:
								xc = std::stoi(value);
								capacities[lineIdx - 7] = xc;
							
								break;
							
							default:
							
								break;
						}

						stringcut = stringcut.substr(pos + 1);
						value = stringcut;
					}  // if (pos != std::string::npos)
				}  // for (int i = 0; i < 3; i++)

				int v = std::stoi(value);
				prizes[lineIdx - 7] = v;
				maxFit += v;
			
				break;
			}  // switch (lineIdx)
		}  // while (!myfile.eof())
		
		myfile.close();


		rng.seed();  // initialize the random number generator
		CtopDecoder decoder;	// initialize the decoder
		BRKGA< CtopDecoder, MTRand > algorithm(clients, p, pe, pm, rhoe, decoder, rng, K, MAXT);

		algorithm.evolve(evols);

		time_t     nowFim = time(0);
		struct tm  tstructFim;
		char       bufFim[80];
		tstructFim = *localtime(&nowFim);
		strftime(bufFim, sizeof(bufFim), "%Y-%m-%d %X", &tstructFim);

		std::cout << std::endl;
		std::cout << "Begins at " << bufInicio << std::endl;
		std::cout << " Ends at  " << bufFim << std::endl << std::endl;
		std::cout << "Best fitness = " << (maxFit - algorithm.getBestFitness()) << std::endl << std::endl;
		
		std::ofstream output;
		output.open("resultado.txt", std::fstream::trunc); // remove the content of the output file, and open it

		output << "POPULATION: " << p << std::endl;
		output << "EVOLUTIONS: " << evols << std::endl;
		output << "CLIENTS: " << clients << std::endl;
		output << "CARS: " << cars << std::endl;
		output << "MAX DISTANCE: " << deadline << std::endl;
		output << "MAX CAPACITY: " << maxCapacity << std::endl;
		output << "MAX FITNESS: " << maxFit << std::endl << std::endl;

		/* Print the routes, each of which has been incremented, as well as each client, for them not to start by 0 */
		for (unsigned i = 0; i < bestRoutes.size(); i++) {
			
			output << "Route " << i + 1 << ": ";
			
			for (unsigned j = 0; j < bestRoutes[i].size(); j++) {
			
				output << (bestRoutes[i][j] + 1);
			
				if (j != bestRoutes[i].size() - 1)
					output << "->";
			}
			
			output << std::endl;
		}
		
		output << "Fitness: " << totalBest << std::endl << std::endl;
		output << "Begins at " << bufInicio << std::endl;
		output << "Ends at " << bufFim << std::endl;
		output.close();
	}
	else
		std::cout << "File " << argv[1] << " does not exist." << std::endl;

	return EXIT_SUCCESS;
}
