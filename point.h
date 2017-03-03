#include <string>
#include <iostream>
#include <vector>
#include <assert.h>
#include "game_defs.h"

using namespace std;

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

Point::Point() {
	x = 0;
	y = 0;
}

Point::Point(int x1, int y1) {
	x = x1;
	y = y1;
}

