#include <list>
#include <fstream>
#include <iostream>
#include <string>

#include "CtopDecoder.h"
#include "Vertice.h"
#include "Clients.h"
#include "MTRand.h"
#include "BRKGA.h"


int maxCapacity = 0;
int deadline = 0;
int cars = 0;
int maxFit = 0;
double totalBest = 0;

bool scriptVersion;

MTRand rng;

std::vector<Vertice> vertices;

std::mutex mutex;
std::vector<std::vector<int>> bestRoutes;


int main(int argc, char* argv[]) {

	if(argc != 3) {

		std::cerr << "\nWrong parameter number\n" << std::endl;

		return EXIT_FAILURE;
	}

	int p = 0;
	int evols = 0;
	int clients = 0;
	bool found = false;
	time_t     inicio = time(0);
	struct tm  tstructInicio;
	char       bufInicio[80];
	tstructInicio = *localtime(&inicio);
	strftime(bufInicio, sizeof(bufInicio), "%Y-%m-%d %X", &tstructInicio);

	const double pe = 0.20;		// fraction of population to be the elite-set
	const double pm = 0.10;		// fraction of population to be replaced by mutants
	const double rhoe = 0.70;	// probability that offspring inherit an allele from elite parent
	const unsigned K = 3;		// number of independent populations 3
	const unsigned MAXT = 1;	// number of threads for parallel decoding 2

	double x, y;
	std::string line;
	std::string stringcut;
	CtopDecoder decoder;	// initialise the decoder
	
	std::ifstream myfile;
	myfile.open(argv[1]);
	
	scriptVersion = argv[2];

	if (myfile.is_open()) {

		int lineIdx = 0;
		
		while (!myfile.eof()) {  // iterate over the contents of the file line by line

			lineIdx++;
			std::string value;
			std::size_t pos;

			switch (lineIdx)
			{
				case(1) :  // line 1 (population size)
					std::getline(myfile, line);
					value = line;
					pos = value.find(";");
				
					if (pos != std::string::npos)
						value = value.substr(0, pos);
				
					p = std::stoi(value);
					
					break;
				
				case(2) :  // line 2 (number of evolutions)
					std::getline(myfile, line);
					value = line;
					pos = value.find(";");
				
					if (pos != std::string::npos)
						value = value.substr(0, pos);
				
					evols = std::stoi(value);
				
					break;
				
				case(3) :  // line 3 (number of vertices)
					std::getline(myfile, line);
					value = line;
					pos = value.find(";");
				
					if (pos != std::string::npos)
						value = value.substr(0, pos);
				
					clients = std::stoi(value) - 2;  // the number of clients does not include the origin and deposit
					vertices.reserve(clients);  // allocate space for the vertices, representing the clients
				
					break;
				
				case(4) :  // line 4 (number of cars)
					std::getline(myfile, line);
					value = line;
					pos = value.find(";");
				
					if (pos != std::string::npos)
						value = value.substr(0, pos);
				
					cars = std::stoi(value);
				
					break;
				
				case(5) :  // line 5 (maximum time taken during each car trip)
					std::getline(myfile, line);
					value = line;
					pos = value.find(";");
				
					if (pos != std::string::npos)
						value = value.substr(0, pos);
				
					deadline = std::stoi(value);
				
					break;
					
				case(6) :  // line 6 (maximum car capacity)
					std::getline(myfile, line);
					value = line;
					pos = value.find(";");
				
					if (pos != std::string::npos)
						value = value.substr(0, pos);
				
					maxCapacity = std::stoi(value);
				
					break;
					
				case(7) :  // line 7 (vertice corresponding to the origin)
					std::getline(myfile, line);
					stringcut = line;
					value = stringcut;
					
					pos = stringcut.find(";");
					value.replace(pos, value.length(), "");
					x = std::stof(value);
					stringcut = stringcut.substr(pos + 1);
					value = stringcut;
					
					pos = stringcut.find(";");								
					value.replace(pos, value.length(), "");
					y = std::stof(value);
      	
					decoder.setOrigin(std::make_pair(x,y));
      	
					break;
				
				default:  // line 8 and beyond (vertices and deposit)
					std::getline(myfile, line);
					stringcut = line;
					value = stringcut;
      	
					pos = stringcut.find(";");
					value.replace(pos, value.length(), "");
      	
					if( lineIdx - 8 == clients ) {
						
						x = std::stof(value);
						stringcut = stringcut.substr(pos + 1);
						value = stringcut;
      	
						pos = stringcut.find(";");								
						value.replace(pos, value.length(), "");
						y = std::stof(value);
      	
						decoder.setDeposit(std::make_pair(x,y));
					}
					else {
      	
						Vertice v;
						v.setID(lineIdx-8);
      	
						v.setX(std::stof(value));
						stringcut = stringcut.substr(pos + 1);
						value = stringcut;
      	
						pos = stringcut.find(";");								
						value.replace(pos, value.length(), "");
						v.setY(std::stof(value));
						stringcut = stringcut.substr(pos + 1);
						value = stringcut;
      	
						pos = stringcut.find(";");
						value.replace(pos, value.length(), "");				
						v.setCapacity(std::stoi(value));
						stringcut = stringcut.substr(pos + 1);
						value = stringcut;
      	
						v.setScore(std::stoi(value));
						maxFit += v.getScore();
      	
						vertices.push_back(v);					
					}
			}

		}  // while (!myfile.eof())

		int filtered = Clients::filter(decoder.getOrigin(), decoder.getDeposit());  // filter vertices that cannot be taken		

		myfile.close();

		rng.seed();  // initialise the random number generator

		BRKGA< CtopDecoder, MTRand > genAlg(clients-filtered, p, pe, pm, rhoe, decoder, rng, K, MAXT); // initialise the genetic algorithm
		
		for(unsigned i = 0; i < genAlg.getK(); i++)  // check if the best route has already been found
			if( genAlg.getBestFitness() == 0 )
				found = true;

		if (!found)  // If the best route has not been found during the initialisation (...)
			genAlg.evolve(evols);  // (...) do the evolutions

		time_t     nowFim = time(0);
		struct tm  tstructFim;
		char       bufFim[80];
		tstructFim = *localtime(&nowFim);
		strftime(bufFim, sizeof(bufFim), "%Y-%m-%d %X", &tstructFim);

		std::cout << std::endl;
		std::cout << "Began at " << bufInicio << std::endl;
		std::cout << "Ended at " << bufFim << std::endl << std::endl;
		std::cout << "Best fitness = " << (maxFit - genAlg.getBestFitness()) << std::endl << std::endl;
		
		
		if(scriptVersion) { // if the user has flagged the programme, so that the routes are also printed to the stdout
		
			/* Print the routes, each of which has been incremented, as well as each client, for them not to start by 0 */
			for (unsigned i = 0; i < bestRoutes.size(); i++) {
				
				std::cout << "Route " << i + 1 << ": ";
				
				for (unsigned j = 0; j < bestRoutes[i].size(); j++) {
				
					std::cout << (bestRoutes[i][j] + 1);
				
					if (j != bestRoutes[i].size() - 1)
						std::cout << "->";
				}
				
				std::cout << std::endl;
			}
		
		}
		
		
		std::ofstream output;
		output.open("resultado.txt", std::fstream::trunc); // remove the previous content of the output file, and open it

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
