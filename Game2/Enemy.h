#ifndef ENEMY_H
#define ENEMY_H

#include "Player.h"

class Enemy : Player {
public:
	Enemy();
	~Enemy();

	void update(float dt);


private:
	bool aiMode;
	vector<Vector3> aiPath;


};




#endif