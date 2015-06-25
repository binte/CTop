#include "Vertice.h"
	
Vertice::Vertice() {
	
	this->id				= -1;
	this->x 				= std::numeric_limits<float>::max();
	this->y	 				= std::numeric_limits<float>::max();
	this->score 		= std::numeric_limits<int>::max();
	this->capacity	= std::numeric_limits<int>::max();
}

Vertice::Vertice(int id, float x, float y, int score, int capacity) {
	
	this->id				= id;
	this->x					= x;
	this->y					= y;
	this->score 		= score;
	this->capacity	= capacity;
}


/************************************************/
/********************* SETS *********************/
/************************************************/
void Vertice::setID(int id) {
	
	this->id = id;
}

void Vertice::setX(float x) {
	
	this->x = x;
}

void Vertice::setY(float y) {
	
	this->y = y;
}

void Vertice::setScore(int score) {
	
	this->score = score;
}

void Vertice::setCapacity(int capacity) {
	
	this->capacity = capacity;
}


/************************************************/
/********************* GETS *********************/
/************************************************/
int Vertice::getID() {
	
	return this->id;
}

float Vertice::getX() {
	
	return this->x;
}

float Vertice::getY() {
	
	return this->y;
}

int Vertice::getScore() {
	
	return this->score;
}

int Vertice::getCapacity() {
	
	return this->capacity;
}

/************************************************/
/******************* Methods ********************/
/************************************************/


std::pair<float, float> Vertice::getCoord() {
	
	return std::make_pair(this->x, this->y);
}

std::string Vertice::toString() {
  
	std::string s;

	s.append(std::to_string(this->id) + ":	");
	s.append("(");
	s.append(std::to_string(this->x));
	s.append(",	");
  s.append(std::to_string(this->y));
  s.append(",	");
  s.append(std::to_string(this->score));
  s.append(",	");
  s.append(std::to_string(this->capacity));
  s.append(")");

	return s;
}
