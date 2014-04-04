#include "Level.h"

Level::Level() {
	playerLoc = Vector3(0.0f, 0.0f, 0.0f);
	exitLoc = Vector3(0.0f, 0.0f, 0.0f);
	levelDimensions = Vector3(0.0f,0.0f,0.0f);
}

Level::Level(ID3D10Device* device) {
	playerLoc = Vector3(0.0f, 0.0f, 0.0f);
	exitLoc = Vector3(0.0f, 0.0f, 0.0f);
	levelDimensions = Vector3(0.0f,0.0f,0.0f);
	md3dDevice = device;
}

void Level::fillLevel(string s) {

	std::ifstream fin;
	fin.open(s);

	//floor 
	fin >> levelDimensions.x;
	fin >> levelDimensions.z;

	int count = 0;
	fin >> count;
	for (int i = 0; i < count; i++) {
		// walls

	}
	fin >> count;
	for (int i = 0; i < count; i++) {
		//enemy stuff
		int posX;
		int posZ;
		fin >> posX;
		fin >> posZ;
		Enemy enemy;
		enemy.init("Enemy1", Vector3(posX, 0, posZ), 15, 17, 6, 3.3f, md3dDevice);
		enemies.push_back(enemy);
		//nothing else to do until we get patterns baked into the enemy class
	}
	fin >> count;
	for (int i = 0; i < count; i++) {
		//do parts stuff
		//not much to do here until the parts are done
	}
	for (int i = 0; i < count; i++) {
		fin >> playerLoc.x;
		fin >> playerLoc.z;
	}
	for (int i = 0; i < count; i++) {
		fin >> exitLoc.x;
		fin >> exitLoc.z;
	}

	fin.close();




}