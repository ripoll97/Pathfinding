#pragma once
#include "Connection.h"
#include <vector>
#include "Vector2D.h"

class Graph {
public:
	vector<Connection> terrainGraph;

	Graph();
	~Graph();

	void setConnection(Connection conex);
	vector<Connection> GetConnections(Vector2D fromNode);
	int getSize();
};