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
#include "BodyPart.h"
using std::vector;
using std::string;

class BodyPart;

class Player
{
public:
	Player();
	~Player(void);

	void init(string n, Vector3 pos, float spd, float height, float width, float depth, ID3D10Device* device);
	void buildBody();

	void update(float dt);
	void draw(Matrix);
	
	void setMTech(ID3D10EffectTechnique* m){ mTech = m;}
	void setEffectVariables(ID3D10EffectMatrixVariable*, ID3D10EffectMatrixVariable*);

public:
	D3DXMATRIX mWVP;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10EffectMatrixVariable* mfxWorldVar;
	ID3D10EffectTechnique* mTech;

private:
	string name;
	BodyPart* torso;
	BodyPart* head;
	BodyPart *rightArm, *leftArm, *rightLeg, *leftLeg;
	Vector3 position;
	Vector3 direction;

	float height;
	float width;
	float depth;

	float dirTheta;
	float turnSpeed;

	float gameTime;
	
	float speed;
	ID3D10Device* device;

};

