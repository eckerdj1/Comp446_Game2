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
	name = n;
	position = pos;
	speed = spd;
	normalSpeed = speed;
	sprintBoost = speed * 2.3f;
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

	Box* b = new Box();
	b->init(device, width, height * 0.36f, depth, LightBlue, LightBlue);
	Vector3 tPos = position;
	tPos.y += height * 0.5f;
	torso = new BodyPart();
	head = new BodyPart();
	rightArm = new BodyPart();
	leftArm = new BodyPart();
	rightLeg = new BodyPart();
	leftLeg = new BodyPart();
	// torso
	torso->init("torso", b, tPos, direction, Vector3(width, height * 0.36f, depth), speed);
	torso->setPlayer(this);

	// head
	b = new Box();
	b->init(device, width * 0.45f, height * 0.12f, depth * 0.9f, LightBlue, LightBlue);
	Vector3 hPos = position;
	hPos.y += height * 0.38f;
	head->init("head", b, hPos, direction, Vector3(width * 0.45f, height * 0.12f, depth * 0.9f), speed);
	head->setPlayer(this);
	head->setRoot(torso);

	// right arm
	b = new Box();
	Vector3 raPos = position;
	Vector3 torsoSize = torso->getSize();
	raPos += Vector3(width * 0.65f, torsoSize.y * 0.8f, 0);
	b->init(device, width * 0.2f, height * 0.37f, depth * 0.5f, LightBlue, LightBlue);
	rightArm->init("rightArm", b, raPos, direction, Vector3(width * 0.2f, height * 0.37f, depth * 0.5f), speed);
	rightArm->setPlayer(this);
	rightArm->setRoot(torso);
	// left arm
	b = new Box();
	Vector3 laPos = position;
	laPos += Vector3(width * -0.65f, torsoSize.y * 0.8f, 0);
	b->init(device, width * 0.2f, height * 0.37f, depth * 0.5f, LightBlue, LightBlue);
	leftArm->init("leftArm", b, laPos, direction, Vector3(width * 0.2f, height * 0.37f, depth * 0.5f), speed);
	leftArm->setPlayer(this);
	leftArm->setRoot(torso);

	// right leg
	b = new Box();
	Vector3 rlPos = position;
	rlPos += Vector3(width * 0.35f, height * -0.05f, 0);
	b->init(device, width * 0.25f, height * 0.45f, depth * 0.5f, LightBlue, LightBlue);
	rightLeg->init("rightLeg", b, rlPos, direction, Vector3(width * 0.25f, height * 0.45f, depth * 0.5f), speed);
	rightLeg->setPlayer(this);
	rightLeg->setRoot(torso);
	// left leg
	b = new Box();
	Vector3 llPos = position;
	llPos += Vector3(width * -0.35f, height * -0.05f, 0);
	b->init(device, width * 0.25f, height * 0.45f, depth * 0.5f, LightBlue, LightBlue);
	leftLeg->init("leftLeg", b, llPos, direction, Vector3(width * 0.25f, height * 0.45f, depth * 0.5f), speed);
	leftLeg->setPlayer(this);
	leftLeg->setRoot(torso);

	// connect body parts
	torso->addChild(head);
	torso->addChild(rightArm);
	torso->addChild(leftArm);
	torso->addChild(rightLeg);
	torso->addChild(leftLeg);
	b = 0;
}

void Player::update(float dt)
{
	gameTime += dt;
	//Take care of input
	//torso->setSpeed(0.0f);
	bool moving = false;
	if (keyPressed(PlayerSprintKey))
	{
		speed = sprintBoost;
		torso->setRotX(ToRadian(15));
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
	torso->setPosition(position);

	//	leg movement
	// rotate legs down
	rightLeg->setRotX(ToRadian(180));
	leftLeg->setRotX(ToRadian(180));
	if (moving)
	{	//swing arms back and forth if moving
		rightLeg->addRotX(-cos(gameTime * limbSpeed) * 0.6f * (speed/normalSpeed));
		leftLeg->addRotX(cos(gameTime * limbSpeed) * 0.6f * (speed/normalSpeed));
	}
	else
	{	//move arms by side if not moving
		rightLeg->setRotX(ToRadian(180));
		leftLeg->setRotX(ToRadian(180));
	}
	
	//	arm movement
	// rotate arms down
	rightArm->setRotX(ToRadian(180));
	leftArm->setRotX(ToRadian(180));
	if (moving)
	{	//swing arms back and forth if moving
		rightArm->addRotX(cos(gameTime * limbSpeed));
		leftArm->addRotX(-cos(gameTime * limbSpeed));
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