#pragma once
#include "Box.h"
#include "GameObject.h"
#include "constants.h"
#include "d3dUtil.h"
#include "input.h"
#include <math.h>
#include <vector>
#include <string>
#include "audio.h"
#include "Player.h"
using std::vector;
using std::string;

class Player;

class BodyPart : public GameObject
{
public:
	BodyPart();

	void init(string _name, Box *b, Vector3 pos, Vector3 dir, Vector3 size, float speed);

	void setTransX(float x) {transX = x;}
	void setTransY(float y) {transY = y;}
	void setTransZ(float z) {transZ = z;}
	
	void setScaleX(float x) {scaleX = x;}
	void setScaleY(float y) {scaleY = y;}
	void setScaleZ(float z) {scaleZ = z;}
	
	void setRotX(float rho) {rotX = rho;}
	void setRotY(float theta) {rotY = theta;}
	void setRotZ(float phi) {rotZ = phi;}

	void addRotX(float rho) {rotX += rho;}
	void addRotY(float theta) {rotY += theta;}
	void addRotZ(float phi) {rotZ += phi;}

	void traverse(Matrix m);
	Matrix getWorld();

	void update(float dt);
	void draw(Matrix mVP);

	void addChild(BodyPart* part);
	void setPlayer(Player* p);
	void setRoot(BodyPart* part);

private:
	Player* player;
	string name;
	BodyPart* root;
	vector<BodyPart*> children;
	Matrix world;
	
	float transX, transY, transZ;
	float scaleX, scaleY, scaleZ;
	float rotX, rotY, rotZ;
};