#pragma once
#include <iostream>
#include <minmax.h>
#include <SDL.h>
#include <SDL_image.h>
#include "SDL_SimpleApp.h"
#include "Vector2D.h"
#include "utils.h"
#include "SteeringBehavior.h"
#include "Path.h"
#include "Graph.h"
#include <queue>
#include <unordered_map>
#include <algorithm>

class Agent
{
	friend class SteeringBehavior;

private:
	SteeringBehavior *steering_behavior;
	Vector2D position;
	Vector2D velocity;
	Vector2D target;

	float mass;
	float orientation;
	float max_force;
	float max_velocity;

	SDL_Color color;

	SDL_Texture *sprite_texture;
	bool draw_sprite;
	int sprite_num_frames;
	int sprite_w;
	int sprite_h;

public:
	Agent();
	~Agent();
	SteeringBehavior *Behavior();
	Vector2D getPosition();
	Vector2D getTarget();
	Vector2D getVelocity();
	float getMaxVelocity();
	void setPosition(Vector2D position);
	void setTarget(Vector2D target);
	void setVelocity(Vector2D velocity);
	void setMass(float mass);
	void setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
	void update(Vector2D steering_force, float dtime, SDL_Event *event);
	void draw();
	bool Agent::loadSpriteTexture(char* filename, int num_frames=1);
	//Pathfinding (Graph, node inici i node final, enum quin algoritme). 
	Path PathFinding_BFS(Graph graph, Vector2D inicialNode, Vector2D endNode);
	Path PathFinding_Dijkstra(Graph graph, Vector2D inicialNode, Vector2D endNode);
	Path PathFinding_A_Estrella(Graph graph, Vector2D inicialNode, Vector2D endNode);
	Path PathFinding_Greedy_BFG(Graph graph, Vector2D inicialNode, Vector2D endNode);
	// Per pintar
	vector<SDL_Rect> fronteraPintada;


	Vector2D cell2pix(Vector2D cell)
	{
		int offset = CELL_SIZE / 2;
		return Vector2D(cell.x*CELL_SIZE + offset, cell.y*CELL_SIZE + offset);
	}

};


class CompareDist
{
public:
	bool operator()(std::pair<Vector2D, int > n1, std::pair<Vector2D, int> n2) {
		return n1.second > n2.second;
	}
};

struct CompareDist2 {
	bool operator()(int u1, int u2) {
		return u1 < u2;
	}
};