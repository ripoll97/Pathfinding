#pragma once
#include <iostream>
#include "Vector2D.h"

using namespace std;

class Connection {
private:
	Vector2D fromNode;
	Vector2D toNode;
	int cost;
public:
	
	Connection(Vector2D from, Vector2D to, int costInt);
	~Connection();

	int GetCost();
	Vector2D GetFromNode();
	Vector2D GetToNode();
};
