#include <string>
#include <iostream>
#include <vector>
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


