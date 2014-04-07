#include "Player.h"

Player::Player()
{
	speed = 0;
	normalSpeed = 0;
	sprintBoost = 0;
	direction = Vector3(0,0,1);
	position = Vector3(0,0,0);
	dirTheta = 0;
	turnSpeed = 5;
	gameTime = 0;
}

Player::~Player()
{
	delete torso;
	delete head;
	delete rightArm;
	delete leftArm;
	delete rightLeg;
	delete leftLeg;
}

void Player::init(string n, Vector3 pos, float spd, float height, float width, float depth, ID3D10Device* d)
{
	device = d;

	//HR(D3DX10CreateShaderResourceViewFromFile(device, 
	//	L"face.jpg", 0, 0, &textures.rv[0], 0 ));
	//HR(D3DX10CreateShaderResourceViewFromFile(device, 
	//	L"side.jpg", 0, 0, &textures.rv[1], 0 ));
	//HR(D3DX10CreateShaderResourceViewFromFile(device, 
	//	L"side.jpg", 0, 0, &textures.rv[2], 0 ));
	//HR(D3DX10CreateShaderResourceViewFromFile(device, 
	//	L"side.jpg", 0, 0, &textures.rv[3], 0 ));
	//HR(D3DX10CreateShaderResourceViewFromFile(device, 
	//	L"headTop.jpg", 0, 0, &textures.rv[4], 0 ));
	//HR(D3DX10CreateShaderResourceViewFromFile(device, 
	//	L"noTex.jpg", 0, 0, &textures.rv[5], 0 ));

	name = n;
	position = pos;
	speed = spd;
	normalSpeed = speed;
	sprintBoost = speed * 3.3f;
	limbSpeed = speed * 0.5f;
	this->height = height;
	this->width = width;
	this->depth = depth;
	buildBody();
}

void Player::setEffectVariables(ID3D10EffectMatrixVariable* wvpVar, ID3D10EffectMatrixVariable* worldVar)
{
	mfxWVPVar = wvpVar;
	mfxWorldVar = worldVar;
}

void Player::buildBody()
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
	b->init(device, width, height * 0.36f, depth, LightBlue, LightBlue);
	b->setDiffuseMap(diffuseMapVar);
	torso->init("torso", b, tPos, direction, Vector3(width, height * 0.36f, depth), speed);
	torso->setBody(this);

	// head
	b = new Box();
	b->init(device, width * 0.45f, height * 0.12f, depth * 0.9f, LightBlue, LightBlue);
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
	b->init(device, width * 0.2f, height * 0.37f, depth * 0.5f, LightBlue, LightBlue);
	b->setDiffuseMap(diffuseMapVar);
	rightArm->init("rightArm", b, raPos, direction, Vector3(width * 0.2f, height * 0.37f, depth * 0.5f), speed);
	rightArm->setBody(this);
	rightArm->setRoot(torso);

	// left arm
	b = new Box();
	Vector3 laPos = position;
	laPos += Vector3(width * -0.65f, torsoSize.y * 0.8f, 0);
	b->init(device, width * 0.2f, height * 0.37f, depth * 0.5f, LightBlue, LightBlue);
	b->setDiffuseMap(diffuseMapVar);
	leftArm->init("leftArm", b, laPos, direction, Vector3(width * 0.2f, height * 0.37f, depth * 0.5f), speed);
	leftArm->setBody(this);
	leftArm->setRoot(torso);

	// right leg
	b = new Box();
	Vector3 rlPos = position;
	rlPos += Vector3(width * 0.35f, height * -0.02f, 0);
	b->init(device, width * 0.25f, height * 0.21f, depth * 0.5f, LightBlue, LightBlue);
	b->setDiffuseMap(diffuseMapVar);
	rightLeg->init("rightLeg", b, rlPos, direction, Vector3(width * 0.25f, height * 0.21f, depth * 0.5f), speed);
	rightLeg->setBody(this);
	rightLeg->setRoot(torso);

	// left leg
	b = new Box();
	Vector3 llPos = position;
	llPos += Vector3(width * -0.35f, height * -0.02f, 0);
	b->init(device, width * 0.25f, height * 0.21f, depth * 0.5f, LightBlue, LightBlue);
	b->setDiffuseMap(diffuseMapVar);
	leftLeg->init("leftLeg", b, llPos, direction, Vector3(width * 0.25f, height * 0.21f, depth * 0.5f), speed);
	leftLeg->setBody(this);
	leftLeg->setRoot(torso);

	//right shin
	b = new Box();
	Vector3 rsPos = position;
	rsPos += Vector3(0, height * .22f, 0);
	b->init(device, width * 0.24f, height * 0.20f, depth * 0.5f, LightBlue, LightBlue);
	b->setDiffuseMap(diffuseMapVar);
	rightShin->init("rightShin", b, rsPos, direction, Vector3(width * 0.24f, height * 0.20f, depth * 0.5f), speed);
	rightShin->setBody(this);
	rightShin->setRoot(rightLeg);

	//left shin
	b = new Box();
	Vector3 lsPos = position;
	lsPos += Vector3(0, height * .22f, 0);
	b->init(device, width * 0.24f, height * 0.20f, depth * 0.5f, LightBlue, LightBlue);
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

void Player::update(float dt)
{
	gameTime += dt;
	elapsed += dt;
	//Take care of input
	//torso->setSpeed(0.0f);
	bool moving = false;
	bool sprinting = false;
	if (keyPressed(PlayerSprintKey))
	{
		speed = sprintBoost;
		torso->setRotX(ToRadian(15));
		sprinting = true;
	}
	else
	{
		speed = normalSpeed;
		torso->setRotX(ToRadian(0));
	}
	if (keyPressed(PlayerForwardKey))
	{
		//torso->setSpeed(speed);
		position += direction * speed * dt;
		moving = true;
	}
	if (keyPressed(PlayerBackKey))
	{
		position -= direction * speed * dt;
		moving = true;
	}
	if (!moving)
		elapsed = 0;
	if (keyPressed(PlayerRightKey))
	{
		dirTheta += turnSpeed * dt;
		if (dirTheta > 180 || dirTheta < -180)
			dirTheta = -dirTheta;
	}
	if (keyPressed(PlayerLeftKey))
	{
		dirTheta -= turnSpeed * dt;
		if (dirTheta > 180 || dirTheta < -180)
			dirTheta = -dirTheta;
	}
	direction.x = sinf(dirTheta);
	direction.z = cosf(dirTheta);
	torso->setDirection(direction);
	torso->setPosition(Vector3(position.x, position.y + height * 0.5f, position.z));
	

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

void Player::draw(Matrix mVP)
{
	D3D10_TECHNIQUE_DESC techDesc;
    mTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		torso->draw(mVP);
    }


}


void Player::setDiffuseMap(ID3D10EffectShaderResourceVariable* var)
{
	diffuseMapVar = var;
}