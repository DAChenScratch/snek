#include <string>
#include <iostream>
#include <vector>
#include <assert.h>
#include <queue>
#include "json/src/json.hpp"
#include "gameboard.h"

using namespace std;
using JSON = nlohmann::json;


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
	assert(board.board.size() == height + 2);
	assert(board.board[0].size() == width + 2);
}



GameInfo::GameInfo(string body) {
	JSON j = JSON::parse(body);
	id = j["you"];
	game_id = j["game_id"];
	turn = j["turn"];
	height = j["height"];
	width = j["width"];
	board = GameBoard(width, height);

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
	for (auto p : food) {
		board.board[p.y][p.x] = FOOD;
	}

	int i = 0;
	int head = snakes.size();
	for (auto snake : snakes) {
		for (auto p : snake.coords) {
			board.board[p.y][p.x] = i;
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


/*

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
		
	}


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
	int free = 0; //getFreeSquares(snake.getHead(), FREESQUARESDEPTH);

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
		newstate.getMySnake();
		newstate.makeMove(m, index);
		newstate.updateBoard();
		newstate.board.print();
	
		float val = newstate.evaluate();

		float val = 1.0;		
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
*/