#include <string>
#include <vector>
#include <assert.h>
#include "point.h"

using namespace std;


class Path {
public:
	vector<Point> path;
	int target;
	Path();
	int getStepDir(int step);
	Point getLast();
};

Point Path::getLast(){
	return path.back();
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
