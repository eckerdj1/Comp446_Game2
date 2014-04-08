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
	// make the four walls to surround the floor
	Wall* wall;
	for (int i = 0; i < 4; i++) {
		wall = new Wall;
		Vector3 position;
		Vector3 scale;
		switch(i) {
		case 0:
			position = Vector3(-levelDimensions.x/2, 0.0f, 0.0f);
			scale = Vector3(3.0f, 20.0f, levelDimensions.z);
			break;
		case 1:
			position = Vector3(0.0f, 0.0f, levelDimensions.z/2);
			scale = Vector3(levelDimensions.x, 20.0f, 3.0f);
			break;
		case 2:
			position = Vector3(levelDimensions.x/2, 0.0f, 0.0f);
			scale = Vector3(3.0f, 20.0f, levelDimensions.z);
			break;
		case 3:
			position = Vector3(0.0f, 0.0f, -levelDimensions.z/2);
			scale = Vector3(levelDimensions.x, 20.0f, 3.0f);
			break;
		}
		wall->init(md3dDevice, position, scale, Blue);
		walls.push_back(*wall);
		delete wall;
	}

	int count = 0;
	fin >> count;
	for (int i = 4; i < count+4; i++) {
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
		Vector3 position = Vector3(((x1*enlargeByC)) + (xLength/2), 0, ((z1*enlargeByC)) + (zLength/2));
		wall = new Wall;
		wall->init(md3dDevice, position, Vector3(xLength, 20.0f, zLength), Blue);
		walls.push_back(*wall);
		delete wall;
	}
	fin >> count;
	Enemy* enemy;
	for (int i = 0; i < count; i++) {
		std::string type;
		int segments;
		fin >> type;
		float posX;
		float posZ;
		//enemy stuff
		if (type == "path") {
			fin >> segments;
			fin >> posX;
			fin >> posZ;
			enemy = new Enemy;
			Light sL;
			sL.ambient  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			sL.diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			sL.specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			sL.att.x    = 1.0f;
			sL.att.y    = 0.0f;
			sL.att.z    = 0.0f;
			sL.spotPow  = 25.0f;
			sL.range    = 40.0f;
			spotLights.push_back(sL);
			enemy->setDiffuseMap(diffuseMapVar);
			enemy->init("Enemy", Vector3(posX, 0, posZ), 15, 17, 6, 3.3f, md3dDevice, &spotLights[spotLights.size()-1]);
			enemy->addPathPoint(Vector3(posX, 0, posZ));
			for (int j = 1; j < segments; j++) {
				fin >> posX;
				fin >> posZ;
				enemy->addPathPoint(Vector3(posX, 0, posZ));
			}
		} else if (type == "random") {
			/*fin >> posX;
			fin >> posZ;
			enemy = new Enemy;
			enemy->init("Enemy", Vector3(posX, 0, posZ), 15, 17, 6, 3.3f, md3dDevice);
			float boundX1, boundX2;
			float boundY1, boundY2;
			fin >> boundX1; 
			fin >> boundY1;
			fin >> boundX2;
			fin >> boundY2;
			enemy->setBounds(Vector2(boundX1, boundY1), Vector2(boundY1, boundY2));*/
		}
		enemies.push_back(enemy);
		int temp = 0;
		temp++;
	}	
	fin >> count;
	Pickup* pickup;
	for (int i = 0; i < count; i++) {
		float posX;
		float posZ;
		fin >> posX;
		fin >> posZ;
		pickup = new Pickup;
		pickup->init(md3dDevice, Vector3(posX*enlargeByC, 0.0f, posZ*enlargeByC), Vector3(2.0*enlargeByC, 2.0*enlargeByC, 2.0*enlargeByC), D3DXCOLOR(.4f, .4f, .4f, 1.0f));
		pickups.push_back(*pickup);
	}
	fin >> playerLoc.x;
	fin >> playerLoc.z;
	player->setPosition(Vector3(playerLoc.x*enlargeByC, 0.0f, playerLoc.z*enlargeByC));
	fin >> exitLoc.x;
	fin >> exitLoc.z;
	//build a landing area for the exit destination
	wall = new Wall;
	wall->init(md3dDevice, Vector3(exitLoc.x*enlargeByC, 0.0f, exitLoc.z*enlargeByC), Vector3(4.0f*enlargeByC, 1.0f, 4.0f*enlargeByC), Green);
	walls.push_back(*wall);
	delete wall;
	

	fin.close();

}

void Level::update(float dt) {
	
	for(int i = 0; i < walls.size(); i++) {
		walls[i].update(dt);
	}
	for (int i = 0; i < pickups.size(); i++) {
		pickups[i].update(dt);
	}
	/*for (int i = 0; i < enemies.size(); i++) {
		enemies[i]->update(dt);
	}*/
}

void Level::draw(Matrix mVP) {
	
	for (int i = 0; i < walls.size(); i++) {
		walls[i].setMTech(mTech);
		mfxWVPVar->SetMatrix(walls[i].getWorldMatrix() * mVP);
		mfxWorldVar->SetMatrix(walls[i].getWorldMatrix());
		walls[i].draw();
	}
	for (int i = 0; i < pickups.size(); i++) {
		pickups[i].setMTech(mTech);
		mfxWVPVar->SetMatrix(pickups[i].getWorldMatrix() * mVP);
		mfxWorldVar->SetMatrix(pickups[i].getWorldMatrix());
		pickups[i].draw();
	}
	//for (int i = 0; i < enemies.size(); i++) {
	//	enemies[i]->setMTech(mTech);
	//	enemies[i]->setEffectVariables(mfxWVPVar, mfxWorldVar);
	//	enemies[i]->draw(mVP);
	//}

}

void Level::setDiffuseMap(ID3D10EffectShaderResourceVariable* var)
{
	diffuseMapVar = var;
}
