#ifndef ENEMY_H
#define ENEMY_H

#include "Player.h"

class Enemy : public Player {
public:
	Enemy();
	~Enemy();

	void update(float dt);
	//void init(string n, Vector3 pos, float spd, float height, float width, float depth, ID3D10Device* d);

private:
	bool aiMode;
	vector<Vector3> aiPath;


};




#endif