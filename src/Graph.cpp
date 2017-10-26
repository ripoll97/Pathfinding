#include "Graph.h"


Graph::Graph() {};
Graph::~Graph() {};

void Graph::setConnection(Connection conex) {
	terrainGraph.push_back(conex);
}
vector<Connection> Graph::GetConnections(Vector2D fromNode) {
	vector<Connection> nextConnections;
	for (int i = 0; i < terrainGraph.size(); i++) {
		if (terrainGraph.at(i).GetFromNode() == fromNode) {
			nextConnections.push_back(terrainGraph.at(i));
		}
	}
	return nextConnections;
}
