#ifndef LEVEL_H
#define LEVEL_H

#include <vector>

#include "GameObject.h"
#include "Box.h"
#include "Player.h"
//#include "Enemy"
//#include "Tower"


using std::vector;

class Level {
private:
	vector<GameObject> wallBoxObjs;
	//vector<Enemy> enemies;
	//vector<Tower> towers;

	//vector<Part> parts;

	Vector2 levelDimensions;
	Vector3 playerLoc;
	Player* player;

public:
	Level();

	void fillLevel(string s);






};



#endif