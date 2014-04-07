#pragma once

#include "BodyPart.h"
#include "GameObject.h"
#include "Box.h"
#include "Body.h"

class Tower : public Body{
public:
	Tower();
	~Tower(void);

	void init(string n, Vector3 pos, float spd, float height, float width, float depth, ID3D10Device* device);
	void buildBody();

	void setDiffuseMap(ID3D10EffectShaderResourceVariable* var);

	void update(float dt);
	void draw(Matrix);
	
	void setMTech(ID3D10EffectTechnique* m) {mTech = m;}
	void setEffectVariables(ID3D10EffectMatrixVariable*, ID3D10EffectMatrixVariable*);

	Vector3 getPosition() {return position;}
	Vector3 getDirection() {return direction;}
	
	D3DXMATRIX mWVP;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10EffectMatrixVariable* mfxWorldVar;
	ID3D10EffectTechnique* mTech;
	ID3D10EffectShaderResourceVariable* diffuseMapVar;

private:
	string name;

	BodyPart* base;
	BodyPart* column;
	BodyPart* eye;
	BodyPart* attackBeam;

	Vector3 position;
	Vector3 direction;

	float height;
	float width;
	float depth;

	float dirTheta;
	float turnSpeed;

	float gameTime;
	float elapsed;
	
	float speed;
	float normalSpeed;
	float sprintBoost;

	float limbSpeed;
	ID3D10Device* device;

};