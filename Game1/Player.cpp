#include "Player.h"

Player::Player()
{
	speed = 0;
	direction = Vector3(0,0,1);
	position = Vector3(0,0,0);
	dirTheta = 0;
	turnSpeed = 5;
}

Player::~Player()
{
	delete torso;
	delete head;
}

void Player::init(string n, Vector3 pos, float spd, float height, float width, float depth, ID3D10Device* d)
{
	device = d;
	name = n;
	position = pos;
	speed = spd;
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
	b->init(device, width, height * 0.4f, depth, LightBlue, LightBlue);
	Vector3 tPos = position;
	tPos.y += height * 0.5f;
	torso = new BodyPart();
	head = new BodyPart();
	torso->init("torso", b, tPos, direction, Vector3(width, height * 0.4f, depth), speed);
	torso->setPlayer(this);
	b = new Box();
	b->init(device, width * 0.45f, height * 0.09f, depth, LightBlue, LightBlue);
	Vector3 hPos = position;
	hPos.y += height * 0.41f;
	head->init("head", b, hPos, direction, Vector3(width * 0.45f, height * 0.09f, depth), speed);
	head->setPlayer(this);
	head->setRoot(torso);
	torso->addChild(head);
}

void Player::update(float dt)
{
	//Take care of input
	//torso->setSpeed(0.0f);
	if (keyPressed(PlayerForwardKey))
	{
		//torso->setSpeed(speed);
		position += direction * speed * dt;
	}
	if (keyPressed(PlayerBackKey))
	{
		position -= direction * speed * dt;
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
	torso->setRotX(cosf(position.z));
	


	//Update the bodyparts
	head->update(dt);
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