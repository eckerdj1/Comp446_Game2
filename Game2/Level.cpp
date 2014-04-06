#include "Level.h"

Level::Level() {
	playerLoc = Vector3(0.0f, 0.0f, 0.0f);
	exitLoc = Vector3(0.0f, 0.0f, 0.0f);
	levelDimensions = Vector3(0.0f,0.0f,0.0f);
	enlargeByC = 5;
}

Level::Level(ID3D10Device* device) {
	playerLoc = Vector3(0.0f, 0.0f, 0.0f);
	exitLoc = Vector3(0.0f, 0.0f, 0.0f);
	levelDimensions = Vector3(0.0f,0.0f,0.0f);
	md3dDevice = device;
	enlargeByC = 5;
}

void Level::setEffectVariables(ID3D10EffectMatrixVariable* wvpVar, ID3D10EffectMatrixVariable* worldVar)
{
	mfxWVPVar = wvpVar;
	mfxWorldVar = worldVar;
}

void Level::fillLevel(string s) {

	std::ifstream fin;
	fin.open(s);

	//floor 
	fin >> levelDimensions.x;
	fin >> levelDimensions.z;
	levelDimensions.x*=enlargeByC;
	levelDimensions.z*=enlargeByC;

	int count = 0;
	fin >> count;
	Wall* wall;
	for (int i = 0; i < count; i++) {
		// walls
		float x1, x2, z1, z2;
		fin >> x1 >> z1 >> x2 >> z2;
		float xLength = x2 - x1;
		if (xLength < 0) {
			xLength *= -1;
		}
		float zLength = z2 - z1;
		if (zLength < 0) {
			zLength *= -1;
		}
		if (zLength < xLength) {
			xLength*=enlargeByC;
		} else {
			zLength*=enlargeByC;
		}
		//assuming the x,z position is at the center of the box
		Vector3 position = Vector3(xLength/2, 0, zLength/2);
		wall = new Wall;
		wall->init(md3dDevice, position, Vector3(xLength, 20.0f, zLength));
		walls.push_back(*wall);
		delete wall;
	}
	fin >> count;
	for (int i = 0; i < count; i++) {
		int segments;
		fin >> segments;
		//enemy stuff
		for (int i = 0; i < segments; i++) {
			float posX;
			float posZ;
			fin >> posX;
			fin >> posZ;
			//Enemy enemy;
			//enemy.init("Enemy1", Vector3(posX, 0, posZ), 15, 17, 6, 3.3f, md3dDevice);
			//enemies.push_back(enemy);
		}
	}
	fin >> count;
	for (int i = 0; i < count; i++) {
		float posX;
		float posZ;
		fin >> posX;
		fin >> posZ;
		//do parts stuff
		//not much to do here until the parts are done
	}
	fin >> playerLoc.x;
	fin >> playerLoc.z;
	fin >> exitLoc.x;
	fin >> exitLoc.z;
	

	fin.close();

}

void Level::update(float dt) {

	for(int i = 0; i < walls.size(); i++) {
		walls[i].update(dt);
	}
	//for (int i = 0; i < enemies.size(); i++) {
	//	enemies[i].update(dt);
	//}
}

void Level::draw(Matrix mVP) {
	
	for (int i = 0; i < walls.size(); i++) {
		walls[i].setMTech(mTech);
		mfxWVPVar->SetMatrix(walls[i].getWorldMatrix() * mVP);
		mfxWorldVar->SetMatrix(walls[i].getWorldMatrix());
		walls[i].draw();
	}

}