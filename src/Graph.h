#pragma once
#include "Connection.h"
#include <vector>
#include "Vector2D.h"

class Graph {
private:
	vector<Connection> terrainGraph;
public:

	Graph();
	~Graph();

	void setConnection(Connection conex);
	vector<Connection> GetConnections(Vector2D fromNode);
};