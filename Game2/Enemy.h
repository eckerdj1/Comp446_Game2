#pragma once

#include "Body.h"

class Enemy : public Body {
public:
	Enemy();
	~Enemy();

	void init(string n, Vector3 pos, float spd, float height, float width, float depth, ID3D10Device* device, Light* light);
	void buildBody();

	void setDiffuseMap(ID3D10EffectShaderResourceVariable* var);

	virtual void update(float dt);
	void draw(Matrix);
	
	void setMTech(ID3D10EffectTechnique* m) { mTech = m;}
	void setEffectVariables(ID3D10EffectMatrixVariable*, ID3D10EffectMatrixVariable*);

	Vector3 getPosition() {return position;}
	Vector3 getDirection() {return direction;}

	void setBounds(Vector2 xLim, Vector2 zLim) {xBounds = xLim; xBounds = zLim;}
	void addPathPoint(Vector3 p) {aiPath.push_back(p);}

private:
	string name;
	BodyPart* torso;
	BodyPart* head;
	BodyPart *rightArm, *leftArm;
	BodyPart *rightLeg, *leftLeg, *rightShin, *leftShin;
	Light* spotLight;
	
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

	enum AiMode {RANDOM, PATH};

	AiMode aiMode;
	Vector2 xBounds;
	Vector2 zBounds;

	vector<Vector3> aiPath;
	int pathIndex;
};