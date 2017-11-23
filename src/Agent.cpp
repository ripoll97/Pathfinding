#include "Agent.h"

using namespace std;

Agent::Agent() : sprite_texture(0),
                 position(Vector2D(100, 100)),
	             target(Vector2D(1000, 100)),
	             velocity(Vector2D(0,0)),
	             mass(0.1f),
	             max_force(150),
	             max_velocity(200),
	             orientation(0),
	             color({ 255,255,255,255 }),
				 sprite_num_frames(0),
	             sprite_w(0),
	             sprite_h(0),
	             draw_sprite(false)
{
	steering_behavior = new SteeringBehavior;
}

Agent::~Agent()
{
	if (sprite_texture)
		SDL_DestroyTexture(sprite_texture);
	if (steering_behavior)
		delete (steering_behavior);
}

SteeringBehavior * Agent::Behavior()
{
	return steering_behavior;
}

Vector2D Agent::getPosition()
{
	return position;
}

Vector2D Agent::getTarget()
{
	return target;
}

Vector2D Agent::getVelocity()
{
	return velocity;
}

float Agent::getMaxVelocity()
{
	return max_velocity;
}

void Agent::setPosition(Vector2D _position)
{
	position = _position;
}

void Agent::setTarget(Vector2D _target)
{
	target = _target;
}

void Agent::setVelocity(Vector2D _velocity)
{
	velocity = _velocity;
}

void Agent::setMass(float _mass)
{
	mass = _mass;
}

void Agent::setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	color = { r, g, b, a };
}

void Agent::update(Vector2D steering_force, float dtime, SDL_Event *event)
{

	//cout << "agent update:" << endl;

	switch (event->type) {
		/* Keyboard & Mouse events */
	case SDL_KEYDOWN:
		if (event->key.keysym.scancode == SDL_SCANCODE_SPACE)
			draw_sprite = !draw_sprite;
		break;
	default:
		break;
	}


	Vector2D acceleration = steering_force / mass;
	velocity = velocity + acceleration * dtime;
	velocity = velocity.Truncate(max_velocity);

	position = position + velocity * dtime;


	// Update orientation
	if (velocity.Length() > 0)
		orientation = (float)(atan2(velocity.y, velocity.x) * RAD2DEG);


	// Trim position values to window size
	if (position.x < 0) position.x = TheApp::Instance()->getWinSize().x;
	if (position.y < 0) position.y = TheApp::Instance()->getWinSize().y;
	if (position.x > TheApp::Instance()->getWinSize().x) position.x = 0;
	if (position.y > TheApp::Instance()->getWinSize().y) position.y = 0;
}

void Agent::draw()
{
	if (draw_sprite)
	{
		Uint32 sprite;

		if (velocity.Length() < 5.0)
			sprite = 1;
		else
			sprite = (int)(SDL_GetTicks() / (max_velocity)) % sprite_num_frames;

		SDL_Rect srcrect = { (int)sprite * sprite_w, 0, sprite_w, sprite_h };
		SDL_Rect dstrect = { (int)position.x - (sprite_w / 2), (int)position.y - (sprite_h / 2), sprite_w, sprite_h };
		SDL_Point center = { sprite_w / 2, sprite_h / 2 };
		SDL_RenderCopyEx(TheApp::Instance()->getRenderer(), sprite_texture, &srcrect, &dstrect, orientation + 90, &center, SDL_FLIP_NONE);
	}
	else
	{
		// Pintar la frontera
		SDL_SetRenderDrawColor(TheApp::Instance()->getRenderer(), 100, 149, 237, 255);
		for (int i = 0; i < fronteraPintada.size(); i++) {
			SDL_RenderFillRect(TheApp::Instance()->getRenderer(), &fronteraPintada[i]);
		}

		draw_circle(TheApp::Instance()->getRenderer(), (int)position.x, (int)position.y, 15, color.r, color.g, color.b, color.a);
		SDL_RenderDrawLine(TheApp::Instance()->getRenderer(), (int)position.x, (int)position.y, (int)(position.x + 15 * cos(orientation*DEG2RAD)), (int)(position.y + 15 * sin(orientation*DEG2RAD)));

	}
}

bool Agent::loadSpriteTexture(char* filename, int _num_frames)
{
	if (_num_frames < 1) return false;

	SDL_Surface *image = IMG_Load(filename);
	if (!image) {
		std::cout << "IMG_Load: " << IMG_GetError() << endl;
		return false;
	}
	sprite_texture = SDL_CreateTextureFromSurface(TheApp::Instance()->getRenderer(), image);

	sprite_num_frames = _num_frames;
	sprite_w = image->w / sprite_num_frames;
	sprite_h = image->h;
	draw_sprite = true;

	if (image)
		SDL_FreeSurface(image);

	return true;
}

Path Agent::PathFinding_BFS(Graph graph, Vector2D inicialNode, Vector2D endNode)
{
	Path tempPath;

	queue<Vector2D> frontier;
	frontier.push(inicialNode);
	unordered_map<Vector2D, Vector2D> came_from;
	Vector2D current;

	//cout << inicialNode.x << " " << inicialNode.y;

	vector<Connection> neighbors;

	int totalExplorationNodes = 0;
	int currentNodes = 0;

	while (!frontier.empty()) {
		current = frontier.front();
		neighbors = graph.GetConnections(current);
		if (current == endNode) {
			//cout << "     FOUND      ";
			break;
		}

		while (!neighbors.empty()) {
			if (came_from.count(neighbors.back().GetToNode()) == 0) {
				frontier.push(neighbors.back().GetToNode());

				// pushback de cada cel�la que cerca, per pinter (floodfill)
				SDL_Rect actualCel = { cell2pix(neighbors.back().GetToNode()).x - (CELL_SIZE / 2), cell2pix(neighbors.back().GetToNode()).y - (CELL_SIZE / 2), 32, 32 };
				fronteraPintada.push_back(actualCel);

				came_from[neighbors.back().GetToNode()] = current;
				neighbors.pop_back();

				currentNodes++;
			}
			else
				neighbors.pop_back();
		}
		frontier.pop();
	}

	current = endNode;
	while (current != inicialNode) {
		tempPath.points.push_back(cell2pix(current));
		current = came_from[current];
	}
	reverse(tempPath.points.begin(), tempPath.points.end());

	totalNodes.push_back(currentNodes);
	std::cout << "Monedes Trobades: " << totalNodes.size() << endl;
	std::cout << "Nodes Explorats: " << totalNodes.back() << endl;
	for (int i = 0; i < totalNodes.size(); i++) {
		totalExplorationNodes += totalNodes[i];
		if (minimNodes > totalNodes[i]) {
			minimNodes = totalNodes[i];
		}
		if (maximNodes < totalNodes[i]) {
			maximNodes = totalNodes[i];
		}
	}
	std::cout << "Exploracio Total Nodes: " << totalExplorationNodes << endl;
	std::cout << "Minim Nodes: " << minimNodes << endl;
	std::cout << "M�xim Nodes: " << maximNodes << endl;
	mitjanaNodes = totalExplorationNodes / totalNodes.size();
	std::cout << "Mitjana Nodes: " << mitjanaNodes << endl << endl;

	return tempPath;
}

Path Agent::PathFinding_Dijkstra(Graph graph, Vector2D inicialNode, Vector2D endNode)
{
	Path tempPath;

	priority_queue<pair<Vector2D, int>, vector<pair<Vector2D, int>>, CompareDist> frontier;
	//frontier.emplace(make_pair(inicialNode, 0));
	frontier.push(make_pair(inicialNode, 0));
	unordered_map<Vector2D, Vector2D> cameFrom;
	unordered_map<Vector2D, int> costSoFar;
	Vector2D current;
	vector<Connection> neighbours;

	int newCost;

	cameFrom[inicialNode] = 0;
	costSoFar[inicialNode] = 0;

	int totalExplorationNodes = 0;
	int currentNodes = 0;

	while (!frontier.empty()) {
		current = frontier.top().first;
		neighbours = graph.GetConnections(current);

		if (current == endNode)
			break;

		while (!neighbours.empty()) {
			newCost = costSoFar[current] + neighbours.back().GetCost();
			//SDL_Rect actualCel = { cell2pix(neighbours.back().GetToNode()).x - (CELL_SIZE / 2), cell2pix(neighbours.back().GetToNode()).y - (CELL_SIZE / 2), 32, 32 };

			if (costSoFar.count(neighbours.back().GetToNode()) == 0 || newCost < costSoFar[neighbours.back().GetToNode()]) {
				costSoFar[neighbours.back().GetToNode()] = newCost;
				frontier.push(make_pair(neighbours.back().GetToNode(), newCost));

				// pushback de cada cel�la que cerca, per pinter (floodfill)
				SDL_Rect actualCel = { cell2pix(neighbours.back().GetToNode()).x - (CELL_SIZE / 2), cell2pix(neighbours.back().GetToNode()).y - (CELL_SIZE / 2), 32, 32 };
				fronteraPintada.push_back(actualCel);

				cameFrom[neighbours.back().GetToNode()] = current;
				neighbours.pop_back();

				currentNodes++;
			}
			else
				//fronteraPintada.push_back(actualCel);
				neighbours.pop_back();
		}
		frontier.pop();
	}
	current = endNode;
	while (current != inicialNode) {
		tempPath.points.push_back(cell2pix(current));
		current = cameFrom[current];
	}
	reverse(tempPath.points.begin(), tempPath.points.end());

	totalNodes.push_back(currentNodes);
	std::cout << "Monedes Trobades: " << totalNodes.size() << endl;
	std::cout << "Cost del cami: " << costSoFar[endNode] << endl;
	std::cout << "Nodes Explorats: " << totalNodes.back() << endl;
	for (int i = 0; i < totalNodes.size(); i++) {
		totalExplorationNodes += totalNodes[i];
		if (minimNodes > totalNodes[i]) {
			minimNodes = totalNodes[i];
		}
		if (maximNodes < totalNodes[i]) {
			maximNodes = totalNodes[i];
		}
	}
	std::cout << "Exploracio Total Nodes: " << totalExplorationNodes << endl;
	std::cout << "Minim Nodes: " << minimNodes << endl;
	std::cout << "M�xim Nodes: " << maximNodes << endl;
	mitjanaNodes = totalExplorationNodes / totalNodes.size();
	std::cout << "Mitjana Nodes: " << mitjanaNodes << endl << endl;
	return tempPath;
}

Path Agent::PathFinding_A_Estrella(Graph graph, Vector2D inicialNode, Vector2D endNode)
{
	Path tempPath;

	priority_queue<pair<Vector2D, int>, vector<pair<Vector2D, int>>, CompareDist> frontier;
	//frontier.emplace(make_pair(inicialNode, 0));
	frontier.push(make_pair(inicialNode, 0));
	unordered_map<Vector2D, Vector2D> cameFrom;
	unordered_map<Vector2D, int> costSoFar;
	Vector2D current;
	vector<Connection> neighbours;

	int newCost;
	int priority;

	cameFrom[inicialNode] = 0;
	costSoFar[inicialNode] = 0;

	int totalExplorationNodes = 0;
	int currentNodes = 0;

	while (!frontier.empty()) {
		current = frontier.top().first;
		neighbours = graph.GetConnections(current);

		if (current == endNode)
			break;

		while (!neighbours.empty()) {
			newCost = costSoFar[current] + neighbours.back().GetCost();
			//SDL_Rect actualCel = { cell2pix(neighbours.back().GetToNode()).x - (CELL_SIZE / 2), cell2pix(neighbours.back().GetToNode()).y - (CELL_SIZE / 2), 32, 32 };

			if (costSoFar.count(neighbours.back().GetToNode()) == 0 || newCost < costSoFar[neighbours.back().GetToNode()]) {
				costSoFar[neighbours.back().GetToNode()] = newCost;
				priority = newCost + heuristic(neighbours.back().GetToNode(), endNode);
				frontier.push(make_pair(neighbours.back().GetToNode(), priority));

				// pushback de cada cel�la que cerca, per pinter (floodfill)
				SDL_Rect actualCel = { cell2pix(neighbours.back().GetToNode()).x - (CELL_SIZE / 2), cell2pix(neighbours.back().GetToNode()).y - (CELL_SIZE / 2), 32, 32 };
				fronteraPintada.push_back(actualCel);

				cameFrom[neighbours.back().GetToNode()] = current;
				neighbours.pop_back();

				currentNodes++;
			}
			else
				//fronteraPintada.push_back(actualCel);
				neighbours.pop_back();
		}
		frontier.pop();

	}
	current = endNode;
	while (current != inicialNode) {
		tempPath.points.push_back(cell2pix(current));
		current = cameFrom[current];
	}
	reverse(tempPath.points.begin(), tempPath.points.end());


	totalNodes.push_back(currentNodes);
	std::cout << "Monedes Trobades: " << totalNodes.size() << endl;
	std::cout << "Cost del cami: " << costSoFar[endNode] << endl;
	std::cout << "Nodes Explorats: " << totalNodes.back() << endl;
	for (int i = 0; i < totalNodes.size(); i++) {
		totalExplorationNodes += totalNodes[i];
		if (minimNodes > totalNodes[i]) {
			minimNodes = totalNodes[i];
		}
		if (maximNodes < totalNodes[i]) {
			maximNodes = totalNodes[i];
		}
	}
	std::cout << "Exploracio Total Nodes: " << totalExplorationNodes << endl;
	std::cout << "Minim Nodes: " << minimNodes << endl;
	std::cout << "M�xim Nodes: " << maximNodes << endl;
	mitjanaNodes = totalExplorationNodes / totalNodes.size();
	std::cout << "Mitjana Nodes: " << mitjanaNodes << endl << endl;
	return tempPath;

}


Path Agent::PathFinding_Greedy_BFG(Graph graph, Vector2D inicialNode, Vector2D endNode)
{
	Path tempPath;

	priority_queue<pair<Vector2D, int>, vector<pair<Vector2D, int>>, CompareDist> frontier;
	//frontier.emplace(make_pair(inicialNode, 0));
	frontier.push(make_pair(inicialNode, 0));
	unordered_map<Vector2D, Vector2D> cameFrom;
	Vector2D current;
	vector<Connection> neighbours;

	int priority;

	cameFrom[inicialNode] = 0;

	int totalExplorationNodes = 0;
	int currentNodes = 0;


	while (!frontier.empty()) {
		current = frontier.top().first;
		neighbours = graph.GetConnections(current);
		frontier.pop();

		if (current == endNode)
			break;

		while (!neighbours.empty()) {
			//SDL_Rect actualCel = { cell2pix(neighbours.back().GetToNode()).x - (CELL_SIZE / 2), cell2pix(neighbours.back().GetToNode()).y - (CELL_SIZE / 2), 32, 32 };

			if (cameFrom.count(neighbours.back().GetToNode()) == 0) {
				priority = heuristic(neighbours.back().GetToNode(), endNode);
				frontier.push(make_pair(neighbours.back().GetToNode(), priority));

				// pushback de cada cel�la que cerca, per pinter (floodfill)
				SDL_Rect actualCel = { cell2pix(neighbours.back().GetToNode()).x - (CELL_SIZE / 2), cell2pix(neighbours.back().GetToNode()).y - (CELL_SIZE / 2), 32, 32 };
				fronteraPintada.push_back(actualCel);

				cameFrom[neighbours.back().GetToNode()] = current;
				neighbours.pop_back();

				currentNodes++;
			}
			else
				//fronteraPintada.push_back(actualCel);
				neighbours.pop_back();
			
		}
		
	}
	current = endNode;
	while (current != inicialNode) {
		tempPath.points.push_back(cell2pix(current));
		current = cameFrom[current];
	}
	reverse(tempPath.points.begin(), tempPath.points.end());


	totalNodes.push_back(currentNodes);
	std::cout << "Monedes Trobades: " << totalNodes.size() << endl;
	std::cout << "Nodes Explorats: " << totalNodes.back() << endl;
	for (int i = 0; i < totalNodes.size(); i++) {
		totalExplorationNodes += totalNodes[i];
		if (minimNodes > totalNodes[i]) {
			minimNodes = totalNodes[i];
		}
		if (maximNodes < totalNodes[i]) {
			maximNodes = totalNodes[i];
		}
	}
	std::cout << "Exploracio Total Nodes: " << totalExplorationNodes << endl;
	std::cout << "Minim Nodes: " << minimNodes << endl;
	std::cout << "M�xim Nodes: " << maximNodes << endl;
	mitjanaNodes = totalExplorationNodes / totalNodes.size();
	std::cout << "Mitjana Nodes: " << mitjanaNodes << endl << endl;

	return tempPath;
}

float Agent::heuristic(Vector2D from, Vector2D to) {
	//float dist = sqrt(pow(abs(to.x - from.x),2) + pow(abs(to.y - from.y),2));
	float dist2 = abs(to.x - from.x) + abs(to.y - from.y);
	return dist2;
}