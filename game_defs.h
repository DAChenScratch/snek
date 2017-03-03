#include <string>
#include <iostream>
#include <vector>
#include "json/src/json.hpp"
#include "crow/amalgamate/crow_all.h"
#include <assert.h>
#include <queue>
#include <stack>
#include <time.h>

using namespace std;
using namespace crow;
using JSON = nlohmann::json;

#define CLASSIC 0
#define ADVANCED 1

#define DEAD 0
#define ALIVE 1

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

const vector<int> moveslist = {NORTH, SOUTH, EAST, WEST};

#define EMPTY -1
#define WALL -2
#define FOOD -3
#define GOLD -4
#define BUFFER -5

#define DEFAULT 0
#define EAT 1
#define FINDFOOD 2
#define ORBIT 3

#define ASTARTIMEOUT 50
#define FREESQUARESDEPTH 6


class profile {
public:
	const char* fname;
	int line;
	clock_t t;
	profile(const char* fn, int l);
	~profile();
};

profile::profile(const char* fn, int l) {
	t = clock();
	fname = fn;
	line = l;

}

profile::~profile() {
	cout << fname << " executed in " << ((float) clock() -  t ) / CLOCKS_PER_SEC << endl;
}

class Point {
public:
	int x;
	int y;
	Point();
	Point(int x1, int y1);
	Point addMove(int move);
	Point convert();
	float distance(Point p);
	vector<Point> expand();
	bool compare(Point b);
	Point(const Point & obj);
};

Point::Point(const Point & obj)
	: x(obj.x), y(obj.y)
{
}

class Path {
public:
	vector<Point> path;
	int target;
	Path();
	int getStepDir(int step);
};

class GameBoard {
public:
	vector<vector<int>> board;
	vector<vector<bool>> visited;
	GameBoard();
	GameBoard(int width, int height);
	int getCoord(Point p);
	bool isValid(Point p);
	void print();
	void markVisited(Point p);
	bool isVisited(Point p);
	void clearVisited();
	GameBoard( const GameBoard & obj);
};

GameBoard::GameBoard(const GameBoard & obj)
	:board(obj.board) , visited(obj.visited)
{
}

class Snake {
public:
	string id;
	string name;
	int health;
	vector<Point> coords;
	Snake();
	Snake(JSON j);
	Point getHead();
	Point getTail();
	Snake(const Snake & obj);
};

Snake::Snake(const Snake & obj)
	:id(obj.id),
	name(obj.name),
	health(obj.health),
	coords(obj.coords)
{
}

class GameInfo {
public:
	int index;
	string id;
	string game_id;
	int turn;
	int height;
	int width;
	vector<Snake> snakes;
	vector<Point> food;
	Snake snake;
	GameBoard board;
	GameInfo();
	GameInfo(string body);
	void addSnakeWall();
	Path breadthFirstSearch(Point start, vector<int> targets, bool geq);
	int getFreeSquares(Point start, int maxdepth);
	Path astarGraphSearch(Point start, Point end);
	vector<Point> fillDeadEnds(Point start);
	vector<float> lookahead();
	vector<float> lookaheadRec(GameInfo& state,  int depth, int maxdepth);
	void makeMove(int move, int sindex);
	int defaultMove(int snake);
	void makeSnakeMove(int snake, int move);
	float evaluate();
	GameInfo( const GameInfo & obj);

private:
	int parseMode(string str);
	void updateBoard();
	void getMySnake();
};

GameInfo::GameInfo(){
	index = -1;
	id = "";
	game_id = "";
	turn = 0;
	height = 0;
	width = 0;
	snakes = vector<Snake>();
	food = vector<Point>();
	snake = Snake();
	board = GameBoard();
}

GameInfo::GameInfo( const GameInfo & obj)
	:index(obj.index),
	id(obj.id),
	game_id(obj.game_id),
	turn(obj.turn),
	height(obj.height),
	width(obj.width),
	snakes(obj.snakes),
	food(obj.food),
	snake(obj.snake),
	board(obj.board)
{
}



GameBoard::GameBoard() {
	board = vector<vector<int>>();
	visited = vector<vector<bool>>();
}

Point::Point() {
	x = 0;
	y = 0;
}

Point::Point(int x1, int y1) {
	x = x1;
	y = y1;
}

Point Point::addMove(int move) {
	assert(move == NORTH || move == EAST || move == WEST || move == SOUTH);
	Point p = Point(x, y);
	switch (move) {
	case NORTH:
		p.y--;
		break;
	case EAST:
		p.x++;
		break;
	case SOUTH:
		p.y++;
		break;
	case WEST:
		p.x--;
		break;
	}
	return p;
}

Point Point::convert() {
	return Point(x + 1, y + 1);
}

float Point::distance(Point p) {
	return sqrt(pow(x - p.x, 2) + pow(y - p.y, 2));
}

vector<Point> Point::expand() {
	vector<Point> moves = vector<Point>();
	for (auto m : moveslist) {
		Point p = addMove(m);
		moves.push_back(p);
	}
	return moves;
}

bool Point::compare(Point b) {
	return (x == b.x) && (y == b.y);
}


GameBoard::GameBoard(int width, int height) {
	board = vector<vector<int>>();
	for (int i = 0; i < height + 2; i++) {
		vector<int> tmp = vector<int>();
		vector<bool> vtmp = vector<bool>();
		for (int j = 0; j < width + 2; j++) {
			if (i == 0 || i == height + 1 || j == 0 || j == width + 1 ) {
				tmp.push_back(WALL);
			} else {
				tmp.push_back(EMPTY);
			}
			vtmp.push_back(false);
		}
		board.push_back(tmp);
		visited.push_back(vtmp);
	}
}
int GameBoard::getCoord(Point p) {
	assert((p.y >= 0) && (p.y <= board.size()));
	assert((p.x >= 0) && (p.x <= board[p.y].size()));
	return board[p.y][p.x];
}

bool GameBoard::isValid(Point p) {
	if (!((p.y >= 0) && (p.y <= board.size() ))) {
		return false;
	}
	if (!((p.x >= 0) && (p.x <= board[p.y].size()))) {
		return false;
	}
	return board[p.y][p.x] != WALL && board[p.y][p.x] != BUFFER && board[p.y][p.x] < 0;
}

void GameBoard::print() {
	for (auto v : board) {
		for (auto i : v) {
			printf("%3d", i);
		}
		cout << endl;
	}
}

void GameBoard::markVisited(Point p) {
	visited[p.y][p.x] = true;
}
bool GameBoard::isVisited(Point p) {
	return  visited[p.y][p.x];
}

void GameBoard::clearVisited() {
	for (auto &vec : visited) {
		fill(vec.begin(), vec.end(), 0);
	}
}

Snake::Snake() {
	coords = vector<Point>();
}

Snake::Snake(JSON j) {
	id = j["id"];
	name = j["name"];
	health = j["health_points"];
	for (auto p : j["coords"]) {
		coords.push_back(Point(p[0], p[1]).convert());
	}
}

Point Snake::getHead() {
	assert(coords.size() > 0);
	return coords[0];
}

Point Snake::getTail() {
	assert(coords.size() > 0);
	return coords.back();
}

GameInfo::GameInfo(string body) {
	JSON j = JSON::parse(body);
	id = j["you"];
	game_id = j["game_id"];
	turn = j["turn"];
	height = j["height"];
	width = j["width"];

	snakes = vector<Snake>();
	for (auto snake : j["snakes"]) {
		Snake s = Snake(snake);
		snakes.push_back(s);
	}

	food = vector<Point>();
	for (auto p : j["food"]) {
		food.push_back(Point(p[0], p[1]).convert());
	}

	updateBoard();
	getMySnake();
	addSnakeWall();


	vector<Point> deadpoints = fillDeadEnds(snake.getHead());
	cout << "DEADPOINTS: " << deadpoints.size() << endl;

	for (auto p : deadpoints) {
		board.board[p.y][p.x] = BUFFER;
	}

	//board.print();
	assert(!snake.id.compare(id));
}

int GameInfo::parseMode(string str) {
	if (!str.compare("classic")) {
		return CLASSIC;
	}
	if (!str.compare("advanced")) {
		return ADVANCED;
	}
	return -1;
}

void GameInfo::updateBoard() {
	board = GameBoard(width, height);

	for (auto p : food) {
		board.board[p.y][p.x] = FOOD;
	}

	int i = 0;
	int head = snakes.size();
	for (auto snake : snakes) {
		for (auto p : snake.coords) {
			board.board[p.y][p.x] = i + head;
		}
		i++;
	}


}

void GameInfo::getMySnake() {
	int  i = 0;
	for (auto s : snakes) {
		if (!s.id.compare(id)) {
			snake = s;
			index = i;
			break;
		}
		i++;
	}
}

void GameInfo::addSnakeWall() {
	for (auto s : snakes) {
		if (s.id.compare(id)) {
			if (s.coords.size() >= snake.coords.size()) {
				vector<Point> exp = s.getHead().expand();
				for (auto p : exp) {
					if (board.getCoord(p) < 0) {
						board.board[p.y][p.x] = BUFFER;
					}
				}
			}
		}
	}
}


vector<Point> GameInfo::fillDeadEnds(Point start) {
	profile prof(__FUNCTION__, __LINE__);
	vector<Point> deadpoints = vector<Point>();
	queue<Point> q = queue<Point>();
	vector<vector<Point>> parent(board.board.size(), vector<Point>(board.board[0].size()));
	vector<vector<int>> valids(board.board.size(), vector<int>(board.board[0].size()));

	Point curpoint = start;
	q.push(curpoint);

	int depth = 0;
	while (!q.empty()) {
		curpoint = q.front();
		q.pop();

		vector<Point> points = curpoint.expand();
		valids[curpoint.y][curpoint.x] =  0;

		for (auto point : points) {
			if (board.isValid(point) || point.compare(snake.getTail())) {
				valids[curpoint.y][curpoint.x]++;
				if (!board.isVisited(point) && board.isValid(point)) {
					parent[point.y][point.x] = curpoint;
					q.push(point);
				}
			}
			board.markVisited(point);
		}

		//cout << valid.size() << endl;
		if (valids[curpoint.y][curpoint.x] == 1) {
			//dead end
			deadpoints.push_back(curpoint);
			Point dead = parent[curpoint.y][curpoint.x];
			int loop = 0;
			while (valids[dead.y][dead.x] == 2) {
				deadpoints.push_back(dead);
				dead = parent[dead.y][dead.x];
				loop++;
				assert(loop < ((height + 2) * (width + 2)));
			}
		}

		depth++;
		//Crash if loop
		if (depth < ((height + 2) * (width + 2)));
	}
	board.clearVisited();
	return deadpoints;
}

int GameInfo::getFreeSquares(Point start, int maxdepth){
	queue<Point> q = queue<Point>();
	queue<Point> swap = queue<Point>();

	vector<vector<Point>> parent(board.board.size(), vector<Point>(board.board[0].size()));

	Point curpoint = start;
	q.push(curpoint);

	int loop = 0;
	int free = 0;
	int depth = 0;

	while (!q.empty() || !swap.empty()) {
		curpoint = q.front();
		q.pop();

		vector<Point> points = curpoint.expand();
		for (auto point : points) {
			if (!board.isVisited(point) && board.isValid(point)) {
				parent[point.y][point.x] = curpoint;
				swap.push(point);
				board.markVisited(point);
				free++;
			}
		}

		if(q.empty()){
			queue<Point> temp = q;
			q = swap;
			swap = temp;
			depth++;
		}

		if(depth >= maxdepth){
			board.clearVisited();
			return free;
		}


		loop++;
		//Crash if loop
		assert(loop < ((height + 2) * (width + 2)));
	}

	board.clearVisited();
	return free;
}


Path GameInfo::breadthFirstSearch(Point start, vector<int> targets, bool geq) {
	profile prof(__FUNCTION__, __LINE__);
	queue<Point> q = queue<Point>();

	vector<vector<Point>> parent(board.board.size(), vector<Point>(board.board[0].size()));

	Point curpoint = start;
	q.push(curpoint);

	int depth = 0;

	while (!q.empty()) {
		curpoint = q.front();
		q.pop();

		vector<Point> points = curpoint.expand();
		for (auto point : points) {
			if (!board.isVisited(point) && board.isValid(point)) {
				parent[point.y][point.x] = curpoint;
				q.push(point);
				board.markVisited(point);

				for (auto target : targets) {
					if ((!geq && (board.getCoord(point) == target)) || (geq && (board.getCoord(point) >= target))) {
						Path path = Path();
						path.target = target;

						int loop = 0;
						//walk path
						while (start.x != point.x || start.y != point.y ) {
							path.path.insert(path.path.begin(), point);
							point = parent[point.y][point.x];

							loop++;
							assert(loop < ((height + 2) * (width + 2)));
						}
						path.path.insert(path.path.begin(), start);
						board.clearVisited();
						return path;
					}
				}
			}

		}

		depth++;
		//Crash if loop
		assert(depth < ((height + 2) * (width + 2)));
	}
	board.clearVisited();
	return Path();
}


class Node {
public:
	vector<Point> path;
	Point point;
	float f;
	float g;
	float h;
	Node();
	Node(Point p);
	vector<Node> expand(Point target);
};

Node::Node() {
	path = vector<Point>();
	point = Point();
	f = 0;
	g = 0;
	h = 0;
}

Node::Node(Point p) {
	path = vector<Point>();
	path.push_back(p);
	point = p;
	f = 0;
	g = 0;
	h = 0;
}


bool compareF(const Node& a, const Node& b) {
	return a.f < b.f;
}

bool isBetter(vector<Node> vec, Node node) {
	bool same = false;
	for (auto n : vec) {
		if (node.point.compare(n.point)) {
			same = true;
			if (node.f < n.f) {
				return true;
			}
		}
	}
	return !same;
}

vector<Node> Node::expand(Point target) {
	vector<Node> nodes;
	vector<Point> points = point.expand();
	for (auto p : points) {
		Node node;
		node.point = p;
		node.path = vector<Point>(path);
		node.path.push_back(p);
		node.g = g + 1;
		node.h = p.distance(target);
		node.f = node.g + node.h;
		nodes.push_back(node);
	}
	return nodes;
}

Path GameInfo::astarGraphSearch(Point start, Point end) {
	profile prof(__FUNCTION__, __LINE__);
	Path path = Path();
	Node first = Node(start);
	vector<Node> open;
	vector<Node> closed;
	open.push_back(first);

	while (!open.empty()) {
		auto it = min_element(open.begin() , open.end(), compareF);
		Node best = *it;
		open.erase(it);

		vector<Node> exp = best.expand(end);
		for (auto node : exp) {
			if (node.point.compare(end)) {
				path.path = node.path;
				cout << "A* path found and size is " << path.path.size() << endl;
				return path;
			}

			if (board.isValid(node.point)) {
				if (isBetter(open, node) && isBetter(closed, node)) {
					open.push_back(node);
				}
			}
		}

		closed.push_back(end);

		///timeout a*
		if (((float) clock() - prof.t) / CLOCKS_PER_SEC * 1000 > ASTARTIMEOUT) {
			cout << "AStar Timeout" << endl;
			return Path();
		}
	}
	cout << "Astar Did not find a path" << endl;

	return path;
}



int GameInfo::defaultMove(int snake) {
	Point head = snakes[snake].getHead();

	vector<int> posmoves = vector<int>();
	for (auto m : moveslist) {
		Point p = head.addMove(m);

		//if we can move into tail
		if (p.compare(snakes[snake].getTail()) && snakes[snake].coords.size() > 3) {
			return m;
		}

		if (board.isValid(p)) {
			posmoves.push_back(m);
		}
	}

	//else we are fucked anyways yolo
	if (!posmoves.size()) {
		return 0;
	}

	return posmoves[rand() % posmoves.size()];
}


//evalaute our position
float GameInfo::evaluate(){	
	Point head = snake.getHead();

	/*
	//check snake collision
	int t = board.getCoord(head);

	if(t >= 0){
		//collision
		if(snakes[t].getHead().compare(head)){
			//head on collision
			if(snake.coords.size() > snakes[t].coords.size()){
				//good

			}
		}

	} 

	if(!board.isValid(head)){
		//bad
		
	}*/
	/*

	//TODO update the board
	//THERE IS ERROR HERE
	int i = 0;
	int h = snakes.size();
	for (auto snake : snakes) {
		for (auto p : snake.coords) {
			if(board.isValid(p)){
				board.board[p.y][p.x] = i + h;
			}
		}
		i++;
	}

	*/

	//get avg euclidian distance away from food
	float food_score = 0;
	for(auto fp: food){
		food_score += fp.distance(snake.getHead());
	}
	food_score /= food.size();
	//cout << "Food score " << food_score << endl;

	//amount of direct free moves
	float dmove_score = 0;
	for(auto m: moveslist){
		Point p = snake.getHead().addMove(m);
		if(board.isValid(p)){
			dmove_score++;
		}
	}
	//cout << "Dmove score " << dmove_score << endl;

	//free squares 
	int free = getFreeSquares(snake.getHead(), FREESQUARESDEPTH);

	//cout << "FreeSquares " << free << endl;

	if(dmove_score == 0){
		return -75.0;
	}

	return dmove_score * 10 + (30 - food_score) + (free);
}

void GameInfo::makeSnakeMove(int snake, int move) {
	assert(snake  < snakes.size());
	if (move < 0) {
		move = defaultMove(snake);
	}
	Point nhead = snakes[snake].getHead().addMove(move);
	snakes[snake].coords.insert(snakes[snake].coords.begin(), nhead);
	snakes[snake].coords.pop_back();
}


void GameInfo::makeMove(int move, int sindex) {
	assert(move >= 0 && move <= 3);
	int i = 0;

	for (auto s : snakes) {
		if (i == sindex) {
			makeSnakeMove(sindex, move);
		}else{
			makeSnakeMove(i, -1);
		}
		i++;
	}
}

vector<float> GameInfo::lookaheadRec(GameInfo& state, int depth, int maxdepth) {
	vector<float> vals;
	for(auto m: moveslist){
		GameInfo newstate = state;

		newstate.makeMove(m, index);
		float val = newstate.evaluate();
				
		int sum = 0;

		//Pruning
		if(depth < maxdepth && val > 0){		
			vector<float> add = lookaheadRec(newstate, depth + 1, maxdepth);
			for(auto a: add){
				sum += a;
			}
		}

		vals.push_back(val + sum);			
	}

	return vals;
}


void benchmark(GameInfo game){
	profile prof(__FUNCTION__, __LINE__);
	vector<GameInfo> states;

	while(states.size() < 4000){
		for(auto m: moveslist){
			GameInfo newstate = game;
			newstate.makeMove(m, 0);
			states.push_back(newstate);
		}
	}
}

vector<float> GameInfo::lookahead() {
	profile prof(__FUNCTION__, __LINE__);
	return lookaheadRec(*this, 0, 5);
}

Path::Path() {
	path = vector<Point>();
	target = EMPTY;
}

int Path::getStepDir(int step) {
	assert(step >= 0 && step < path.size() - 1);
	Point one = path[step];
	Point two = path[step + 1];

	int x = two.x - one.x;
	int y = two.y - one.y;

	assert(x != 0 || y != 0);

	if (y == -1) {
		return NORTH;
	} else if (y == 1) {
		return SOUTH;
	} else if (x == 1) {
		return EAST;
	}

	return WEST;
}