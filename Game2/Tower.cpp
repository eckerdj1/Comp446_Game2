#include "Tower.h"

Tower::Tower()
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

Tower::~Tower()
{
	delete base;
	delete column;
	delete eye;
	delete attackBeam;
}

void Tower::init(string n, Vector3 pos, float spd, float height, float width, float depth, ID3D10Device* d)
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
	buildBody();

	//Tower::Body::init(n, pos, spd, height, width, depth, d);
}

void Tower::setEffectVariables(ID3D10EffectMatrixVariable* wvpVar, ID3D10EffectMatrixVariable* worldVar)
{
	mfxWVPVar = wvpVar;
	mfxWorldVar = worldVar;
	
}

void Tower::buildBody() {
	base = new BodyPart();
	column = new BodyPart();
	eye = new BodyPart();
	attackBeam = new BodyPart();

	Vector3 bPos = position;
	bPos += Vector3(height*-0.05f, width, depth);
	Box * b = new Box();
	b ->init(device, width, height, depth, DarkRed, DarkRed);
	b->setDiffuseMap(diffuseMapVar);
	base->init("base", b, bPos, direction, Vector3(width, height, depth), speed);
	base->setBody(this);

	Vector3 cPos = position;
	//cPos += height //offset
	b = new Box();
	b ->init(device, width, height, depth, DarkRed, DarkRed);
	b->setDiffuseMap(diffuseMapVar);
	column->init("column", b, cPos, direction, Vector3(width, height, depth), speed);
	column->setBody(this);

	Vector3 hPos = position;
	//hPos += height //offset
	b = new Box();
	b ->init(device, width, height, depth, DarkRed, DarkRed);
	b->setDiffuseMap(diffuseMapVar);
	eye->init("eye", b, hPos, direction, Vector3(width, height, depth), speed);
	eye->setBody(this);

	Vector3 aPos = position;
	//aPos += height //offset
	b = new Box();
	b ->init(device, width, height, depth, DarkRed, DarkRed);
	b->setDiffuseMap(diffuseMapVar);
	attackBeam->init("attackBeam", b, aPos, direction, Vector3(width, height, depth), speed);
	attackBeam->setBody(this);

	base->addChild(column);
	base->addChild(eye);
	base->addChild(attackBeam);

	b = 0;
}

void Tower::update(float dt) {
	gameTime += dt;
	elapsed += dt;

	dirTheta += turnSpeed * dt;
	direction.x = sinf(dirTheta);
	direction.z = cosf(dirTheta);
	eye->setDirection(direction);


	attackBeam->update(dt);
	eye->update(dt);
	column->update(dt);
	base->update(dt);
}

void Tower::draw(Matrix mVP)
{
	D3D10_TECHNIQUE_DESC techDesc;
    mTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		base->draw(mVP);
    }


}


void Tower::setDiffuseMap(ID3D10EffectShaderResourceVariable* var)
{
	diffuseMapVar = var;
}