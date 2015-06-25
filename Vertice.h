#ifndef _VERTICE
#define _VERTICE

#include <string>
#include <limits>

class Vertice {
	
private:
	int id;				// vertice ID, according to its original number
	float x;  		// abscissa
	float y;			// ordinate
	int score;		// score obtained by passing through this vertice
	int capacity;	// maximum capacity that each car can carry from this vertice

public:
	
	Vertice();
	Vertice(int id, float x, float y, int score, int capacity);

	void setID(int id);
	void setX(float x);
	void setY(float y);
	void setScore(int score);
	void setCapacity(int capacity);
	int getID();
	float getX();
	float getY();
	int getScore();
	int getCapacity();
	std::pair<float, float> getCoord();
	std::string toString();
};

#endif
