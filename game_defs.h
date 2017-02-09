#include <string>
#include <iostream>
#include <vector>
#include "json/src/json.hpp"
#include "crow/amalgamate/crow_all.h"

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

class Point {
public:
	int x;
	int y;
	Point(int x1, int y1) {
		x = x1;
		y = y1;
	}
};

class Snake {
public:
	string id;
	string name;
	int status;
	int age;
	int health;
	vector<Point> coords;
	int kills;
	int food;
	int gold;
	Snake(JSON j);
	int parseStatus(string str); 
};

class GameInfo {
public:
	string game;
	int mode;
	int turn;
	int height;
	int width;
	vector<Snake> snakes;
	vector<Point> food;
	vector<Point> walls;
	vector<Point> gold;
	GameInfo(string body);
	int parseMode(string str);
};


Snake::Snake(JSON j){
		id = j["id"];
		name = j["name"];
		status = parseStatus(j["status"]);
		age = j["age"];
		health = j["health"];
		kills = j["kills"];
		food = j["food"];
		gold = j["gold"];

		for (auto p : j["coords"]) {
			coords.push_back(Point(p[0], p[1]));
		}
	}

int Snake::parseStatus(string str) {
	if (!str.compare("alive")) {
		return ALIVE;
	}
	if (!str.compare("dead")) {
		return DEAD;
	}
	return -1;
}

GameInfo::GameInfo(string body) {
	JSON j = JSON::parse(body);
	game = j["game"];
	mode = parseMode(j["mode"]);
	turn = j["turn"];
	height = j["height"];
	width = j["width"];

	for (auto snake : j["snakes"]) {
		Snake s = Snake(snake);
		snakes.push_back(s);
	}

	for (auto p : j["food"]) {
		food.push_back(Point(p[0], p[1]));
	}
	for (auto p : j["walls"]) {
		walls.push_back(Point(p[0], p[1]));
	}
	for (auto p : j["gold"]) {
		gold.push_back(Point(p[0], p[1]));
	}
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