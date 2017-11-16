#include "Connection.h"

Connection::Connection(Vector2D from, Vector2D to, int costInt) {
	fromNode = from;
	toNode = to;
	cost = costInt;
}
Connection::~Connection() {}

int Connection::GetCost() {
	return cost;
}

Vector2D Connection::GetFromNode() {
	return fromNode;
}

Vector2D Connection::GetToNode() {
	return toNode;
}
