#ifndef LEVEL_H
#define LEVEL_H

#include <vector>

#include "GameObject.h"
#include <fstream>
#include "Box.h"
#include "Player.h"
#include "Enemy.h"
//#include "Tower"


using std::vector;

class Level {
private:
	vector<GameObject> wallBoxObjs;
	vector<Enemy> enemies;
	//vector<Tower> towers;

	//vector<Part> parts;

	Vector3 levelDimensions;
	Vector3 playerLoc;
	Vector3 exitLoc;
	Player* player;
	ID3D10Device* md3dDevice;
public:
	Level();
	Level(ID3D10Device* device); 

	void fillLevel(string s);






};



#endif