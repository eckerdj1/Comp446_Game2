#include "Enemy.h"
#include <random>

Enemy::Enemy()
{
	speed = 0;
	normalSpeed = 0;
	sprintBoost = 0;
	direction = Vector3(0,0,1);
	position = Vector3(0,0,0);
	dirTheta = 0;
	turnSpeed = 5;
	gameTime = 0;

	elapsed = 0;

	aiMode = RANDOM;
	xBounds = Vector2(-10.0f, 10.0f);
	zBounds = Vector2(-10.0f, 10.0f);
	pathIndex = 0;
}

Enemy::~Enemy()
{
	delete torso;
	delete head;
	delete rightArm;
	delete leftArm;
	delete rightLeg;
	delete leftLeg;

}

void Enemy::init(string n, Vector3 pos, float spd, float height, float width, float depth, ID3D10Device* d, Light* light)
{
	device = d;

	name = n;
	position = pos;
	speed = spd;
	normalSpeed = speed;
	sprintBoost = speed * 3.3f;
	limbSpeed = speed * 0.5f;
	this->height = height;
	this->width = width;
	this->depth = depth;

	spotLight = light;

	buildBody();
}

void Enemy::setEffectVariables(ID3D10EffectMatrixVariable* wvpVar, ID3D10EffectMatrixVariable* worldVar)
{
	mfxWVPVar = wvpVar;
	mfxWorldVar = worldVar;
}

void Enemy::buildBody()
{

	torso = new BodyPart();
	head = new BodyPart();
	rightArm = new BodyPart();
	leftArm = new BodyPart();
	rightLeg = new BodyPart();
	leftLeg = new BodyPart();
	rightShin = new BodyPart();
	leftShin = new BodyPart();

	// torso
	Vector3 tPos = position;
	tPos.y += height * 0.5f;
	tPos = Vector3(0, 0, 0);
	Box* b = new Box();
	b->init(device, width, height * 0.36f, depth, DarkRed, DarkRed);
	b->setDiffuseMap(diffuseMapVar);
	torso->init("torso", b, tPos, direction, Vector3(width, height * 0.36f, depth), speed);
	torso->setBody(this);

	// head
	b = new Box();
	b->init(device, width * 0.45f, height * 0.12f, depth * 0.9f, DarkRed, DarkRed);
	b->setDiffuseMap(diffuseMapVar);
	Vector3 hPos = position;
	hPos.y += height * 0.38f;
	head->init("head", b, hPos, direction, Vector3(width * 0.45f, height * 0.12f, depth * 0.9f), speed);
	head->setBody(this);
	head->setRoot(torso);

	// right arm
	b = new Box();
	Vector3 raPos = position;
	Vector3 torsoSize = torso->getSize();
	raPos += Vector3(width * 0.65f, torsoSize.y * 0.8f, 0);
	b->init(device, width * 0.2f, height * 0.37f, depth * 0.5f, DarkRed, DarkRed);
	b->setDiffuseMap(diffuseMapVar);
	rightArm->init("rightArm", b, raPos, direction, Vector3(width * 0.2f, height * 0.37f, depth * 0.5f), speed);
	rightArm->setBody(this);
	rightArm->setRoot(torso);

	// left arm
	b = new Box();
	Vector3 laPos = position;
	laPos += Vector3(width * -0.65f, torsoSize.y * 0.8f, 0);
	b->init(device, width * 0.2f, height * 0.37f, depth * 0.5f, DarkRed, DarkRed);
	b->setDiffuseMap(diffuseMapVar);
	leftArm->init("leftArm", b, laPos, direction, Vector3(width * 0.2f, height * 0.37f, depth * 0.5f), speed);
	leftArm->setBody(this);
	leftArm->setRoot(torso);

	// right leg
	b = new Box();
	Vector3 rlPos = position;
	rlPos += Vector3(width * 0.35f, height * -0.02f, 0);
	b->init(device, width * 0.25f, height * 0.21f, depth * 0.5f, DarkRed, DarkRed);
	b->setDiffuseMap(diffuseMapVar);
	rightLeg->init("rightLeg", b, rlPos, direction, Vector3(width * 0.25f, height * 0.21f, depth * 0.5f), speed);
	rightLeg->setBody(this);
	rightLeg->setRoot(torso);

	// left leg
	b = new Box();
	Vector3 llPos = position;
	llPos += Vector3(width * -0.35f, height * -0.02f, 0);
	b->init(device, width * 0.25f, height * 0.21f, depth * 0.5f, DarkRed, DarkRed);
	b->setDiffuseMap(diffuseMapVar);
	leftLeg->init("leftLeg", b, llPos, direction, Vector3(width * 0.25f, height * 0.21f, depth * 0.5f), speed);
	leftLeg->setBody(this);
	leftLeg->setRoot(torso);

	//right shin
	b = new Box();
	Vector3 rsPos = position;
	rsPos += Vector3(0, height * .22f, 0);
	b->init(device, width * 0.24f, height * 0.20f, depth * 0.5f, DarkRed, DarkRed);
	b->setDiffuseMap(diffuseMapVar);
	rightShin->init("rightShin", b, rsPos, direction, Vector3(width * 0.24f, height * 0.20f, depth * 0.5f), speed);
	rightShin->setBody(this);
	rightShin->setRoot(rightLeg);

	//left shin
	b = new Box();
	Vector3 lsPos = position;
	lsPos += Vector3(0, height * .22f, 0);
	b->init(device, width * 0.24f, height * 0.20f, depth * 0.5f, DarkRed, DarkRed);
	b->setDiffuseMap(diffuseMapVar);
	leftShin->init("leftShin", b, lsPos, direction, Vector3(width * 0.24f, height * 0.20f, depth * 0.5f), speed);
	leftShin->setBody(this);
	leftShin->setRoot(leftLeg);

	// connect body parts
	torso->addChild(head);
	torso->addChild(rightArm);
	torso->addChild(leftArm);
	torso->addChild(rightLeg);
	torso->addChild(leftLeg);
	rightLeg->addChild(rightShin);
	leftLeg->addChild(leftShin);
	b = 0;
}

void Enemy::update(float dt)
{
	//No keyboard input, just preset patterns

	gameTime += dt;
	elapsed += dt;
	
	bool moving = false;
	bool sprinting = false;


	switch(aiMode) {
	case RANDOM:
		//Check if first time through. If so, select random waypoint within bounds
		if(aiPath.size() == 0) {
			/*float xRange;
			float zRange;
			float tempX1, tempX2, xOffset;
			float tempZ1, tempZ2, zOffset;
			
			if(xBounds.x < 0) {tempX1 = - xBounds.x;} else {tempX1 = xBounds.x;}
			if(xBounds.y < 0) {tempX2 = - xBounds.y;} else {tempX2 = xBounds.y;}
			if(zBounds.x < 0) {tempZ1 = - zBounds.x;} else {tempZ1 = zBounds.x;}
			if(zBounds.y < 0) {tempZ2 = - zBounds.y;} else {tempZ2 = zBounds.y;}

			xRange = tempX1 + tempX2;
			zRange = tempZ1 + tempZ2;*/

			float xRand, zRand;
			std::random_device rseed;
			std::mt19937 rng(rseed());
			std::uniform_int<int> dist1(xBounds.x,xBounds.y);
			xRand = dist1(rng);
			std::uniform_int<int> dist2(zBounds.x,zBounds.y);
			zRand = dist2(rng);

			aiPath.push_back(Vector3(xRand, position.y, zRand));
			direction = aiPath[0] - position;
			D3DXVec3Normalize(&direction, &direction);
		}

		//Check if close to destination
		//	If so, calculate and set new destination
		if(((position.x < (aiPath[pathIndex].x + 1.0f)) && (position.x > (aiPath[pathIndex].x - 1.0f)))
		&&((position.z < (aiPath[pathIndex].z + 1.0f)) && (position.z > (aiPath[pathIndex].z - 1.0f))))
		{
			float xRand, zRand;

			std::random_device rseed;
			std::mt19937 rng(rseed());

			std::uniform_int<int> dist1(xBounds.x, xBounds.y);
			xRand = dist1(rng);
			std::uniform_int<int> dist2(zBounds.x, zBounds.y);
			zRand = dist2(rng);

			aiPath[0] = Vector3(xRand, position.y, zRand);
			direction = aiPath[0] - position;
			D3DXVec3Normalize(&direction, &direction);

			elapsed = 0;
		}

		//Move toward waypoint, updating both position and direction
		if(pathIndex == 0)
			OutputDebugString(L"0\n");
		else
			OutputDebugString(L"1\n");


		position += direction * speed * dt;
		moving = true;

		break;
	case PATH:


		//Check if close to waypoint
		//	If so, calculate and set new waypoint
		if(((position.x < (aiPath[pathIndex].x + 1.0f)) && (position.x > (aiPath[pathIndex].x - 1.0f)))
		&&((position.z < (aiPath[pathIndex].z + 1.0f)) && (position.z > (aiPath[pathIndex].z - 1.0f))))
		{
			pathIndex ++;
			if(pathIndex == aiPath.size())
				pathIndex = 0;

			direction = aiPath[pathIndex] - position;
			D3DXVec3Normalize(&direction, &direction);

			elapsed = 0;
		}

		//Move toward waypoint, updating position
		direction = aiPath[pathIndex] - position;
		D3DXVec3Normalize(&direction, &direction);
		position += direction * speed * dt;
		moving = true;

		break;
	}
	

	//direction.x = sinf(dirTheta);
	//direction.z = cosf(dirTheta);
	torso->setDirection(direction);
	torso->setPosition(Vector3(position.x, position.y + height * 0.5f, position.z));
	
	spotLight->pos = torso->getPosition();
	//spotLight->pos.y += 10.0f;
	//Vector3 normalizedDir = (torso->getDirection()*12)-torso->getPosition();
	D3DXVec3Normalize(&spotLight->dir, &(torso->getDirection()));
	
	//	leg movement
	float normPos = 175;
	float legRot = sin(elapsed * limbSpeed);
	float phase = 0;
	float shinRot = sin(elapsed * limbSpeed + phase);
	float legRange = 0;
	float legOffset = 0;
	float shinRange = 0;
	float shinOffset = 10;
	if (moving && !sprinting) // set the leg position variables
	{	//swing legs back and forth if moving
		normPos = 175;
		legRot = sin(elapsed * limbSpeed);
		phase = 1.414f;
		shinRot = sin(elapsed * limbSpeed + phase);
		legRange = 20;
		legOffset = 0;
		shinRange = 25;
		shinOffset = 15;
	}
	else if (moving && sprinting)
	{
		normPos = 175;
		legRot = sin(elapsed * limbSpeed);
		phase = 1.414f;
		shinRot = sin(elapsed * limbSpeed + phase);
		legRange = 90;
		legOffset = -45;
		shinRange = 80;
		shinOffset = 55;
	}
	// set the leg positions
	rightLeg->setRotX(ToRadian(normPos + (-legRot * legRange + legOffset)));
	leftLeg->setRotX(ToRadian(normPos + (legRot * legRange + legOffset)));

	rightShin->setRotX(ToRadian(shinRot * shinRange + shinOffset));
	leftShin->setRotX(ToRadian(-shinRot * shinRange + shinOffset));


	if (keyPressed(PlayerJumpKey))
	{
		torso->reduceScaleByFactor(1.01f);
	}
	
	//	arm movement
	// rotate arms down
	rightArm->setRotX(ToRadian(180));
	leftArm->setRotX(ToRadian(180));
	if (moving && !sprinting)
	{	//swing arms back and forth if moving
		float normPos = 180;
		float armRot = sin(elapsed * limbSpeed);
		float armRange = 10;
		rightArm->setRotX(ToRadian(normPos + (armRot * armRange)));
		leftArm->setRotX(ToRadian(normPos + (-armRot * armRange)));
	}
	else if (moving && sprinting)
	{
		float normPos = 180;
		float armRot = sin(elapsed * limbSpeed);
		float armRange = 30;
		float armOffset = 10;
		rightArm->setRotX(ToRadian(normPos + (armRot * armRange) + armOffset));
		leftArm->setRotX(ToRadian(normPos + (-armRot * armRange) + armOffset));
	}
	else
	{	//move arms by side if not moving
		rightArm->setRotX(ToRadian(180));
		leftArm->setRotX(ToRadian(180));
	}

	//Update the bodyparts
	head->update(dt);
	rightArm->update(dt);
	leftArm->update(dt);
	rightLeg->update(dt);
	leftLeg->update(dt);
	rightShin->update(dt);
	leftShin->update(dt);
	torso->update(dt);
	
}

void Enemy::draw(Matrix mVP)
{
	D3D10_TECHNIQUE_DESC techDesc;
    mTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		torso->draw(mVP);
    }
}


void Enemy::setDiffuseMap(ID3D10EffectShaderResourceVariable* var)
{
	diffuseMapVar = var;
}