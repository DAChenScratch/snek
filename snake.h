#include <string>
#include <iostream>
#include <vector>
#include <assert.h>
#include "path.h"

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

