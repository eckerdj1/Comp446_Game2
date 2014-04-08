//=============================================================================
// Color Cube App.cpp by Frank Luna (C) 2008 All Rights Reserved.
//
// Demonstrates coloring.
//
// Controls:
//		'A'/'D'/'W'/'S' - Rotate 
//
//=============================================================================


#include "d3dApp.h"
#include "Box.h"
#include "GameObject.h"
#include "Line.h"
#include "Quad.h"
#include <d3dx9math.h>
#include "LineObject.h"
#include "Score.h"
#include "Player.h"
#include "Obstacle.h"
#include "Light.h"
#include "Floor.h"
#include "Level.h"

#include <math.h>
#include <ctime>
#include <vector>
#include <string>
using std::string;
using std::vector;
using std::time;
using std::srand;
using std::rand;


#define toString(x) Text::toString(x)

// Ray-Bounding box intersection
// http://people.csail.mit.edu/amy/papers/box-jgt.pdf
class Ray 
{
public:
	Ray(Vector3 &o, Vector3 &d) {
		origin = o;
		direction = d;
		inv_direction = Vector3(1/d.x, 1/d.y, 1/d.z);
		sign[0] = (inv_direction.x < 0);
		sign[1] = (inv_direction.y < 0);
		sign[2] = (inv_direction.z < 0);
	}
	Vector3 origin;
	Vector3 direction;
	Vector3 inv_direction;
	int sign[3];
};

class Box2 {
public:
	Box2(const Vector3 &min, const Vector3 &max) {
		assert(min < max);
		bounds[0] = min;
		bounds[1] = max;
	}
	bool intersect(const Ray &, float t0, float t1);
	Vector3 bounds[2];
};

enum GameState {PLAY, TITLE, HOWTO, LEVELWIN, GAMEWIN, LEVELLOSE, GAMEOVER, CREDITS};

struct PlayState {
	int level;
	int pickUpsRemaining;
	bool completedLevel;
	int livesRemaining;
	PlayState(int l, int pUR, bool cL) {
		level = l;
		pickUpsRemaining = pUR;
		completedLevel = cL;
	}
};

class Game2App : public D3DApp
{
public:
	Game2App(HINSTANCE hInstance);
	~Game2App();

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 

private:
	void buildFX();
	void buildVertexLayouts();
	void setNewObstacleCluster();
 
private:

	vector<GameObject> fallingBlocks;
	vector<GameObject> bullets;
	Vector3 left, right, forward, back, up, down, zero;

	Quad splash;

	Box playerBox;
	Box lineBox;
	GameObject outline[11];
	Player player;
	Level* level;
	Floor floor;
	PlayState playState;
	GameState gameState;
	int numberOfObstacles;
	vector<Box*> obstacleBoxes;
	vector<Obstacle> obstacles;

	int cameraMode;
	int firstPerson;
	int topDown;


	float floorMovement;

	//Lighting
	vector<Light> lights;
	Light ambientLight;
	Light spotLight;
	int numberOfLights;
	int numberOfSpotLights;
	int lightType; // 0-parallel, 1-pointlight, 2-spotlight
	bool useTex;

	bool wallDetectionIsOn;

	float fallRatePerSecond;
	float avgFallSpeed;
	float elapsed;
	float bulletElapsed;
	float bulletsPerSecond;

	float floorSectionLength;
	int floorClusterCounter;
	int floorClusterThreshold;
	int floorSpeedIncrease;
	

	int playerBlock;
	int lives;
	int ammo;
	bool lifeGained;
	bool activeMessage;
	bool matchMade;
	std::wstring message;
	float messageTimer;


	float totalDist;
	bool distSet;
	Score score;

	bool gameOver;
	bool spotted;

	//New Spectrum HUD stuff by Andy
	Box specHudBox[6];
	Box cursorBox;
	GameObject spectrum[6];
	GameObject cursor;

	float getMultiplier();


	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10InputLayout* mVertexLayout;

	ID3D10ShaderResourceView* mDiffuseMapRV;
	ID3D10ShaderResourceView* mSpecMapRV;

	ID3D10ShaderResourceView* mFloorTex;
	ID3D10ShaderResourceView* mFloorSpec;

	ID3D10EffectShaderResourceVariable* mfxDiffuseMapVar;
	ID3D10EffectShaderResourceVariable* mfxSpecMapVar;

	ID3D10EffectMatrixVariable* mfxWVPVar;
	//light variables
	ID3D10EffectMatrixVariable* mfxWorldVar;
	ID3D10EffectVariable* mfxEyePosVar;
	vector<ID3D10EffectVariable*> mfxLightVar;
	vector<ID3D10EffectVariable*> mfxSpotVars;
	ID3D10EffectVariable* mfxSpotVar;
	ID3D10EffectVariable* mfxAmbientVar;
	ID3D10EffectScalarVariable* mfxLightType;
	ID3D10EffectScalarVariable* mfxLightCount;
	ID3D10EffectScalarVariable* mfxSpotCount;
	ID3D10EffectScalarVariable* mfxTexVar;
	ID3D10EffectMatrixVariable* mfxTexMtxVar;



	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mVP;

	Vector3 CameraDirection;
	Vector3 camPos;
	float camTheta;
	float camPhi;
	float camTurnSpeed;
	float camZoom;
	float zoomSpeed;
	float maxZoom, minZoom;
	Vector2 mousePos, lastMousePos;

	//my edits
	D3DXMATRIX worldBox1, worldBox2;

	float mTheta;
	float mPhi;


};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
//#if defined(DEBUG) | defined(_DEBUG)
//	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
//#endif


	Game2App theApp(hInstance);
	
	theApp.initApp();

	return theApp.run();
}

Game2App::Game2App(HINSTANCE hInstance)
: D3DApp(hInstance), mFX(0), mTech(0), mVertexLayout(0),
  mfxWVPVar(0), mTheta(0.0f), mPhi(PI*0.25f)
{
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mVP); 
}

Game2App::~Game2App()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
}

void Game2App::initApp()
{
	D3DApp::initApp();
	
	gameState = TITLE;
	playState.level = 1;
	playState.completedLevel = false;
	playState.livesRemaining = 3;
	audio->playCue(MAIN_TRACK);

	srand(time(0));
	left = Vector3(1,0,0);
	right = Vector3(-1,0,0);
	forward = Vector3(0,0,-1);
	back = Vector3(0,0,1);
	up = Vector3(0,1,0);
	down = Vector3(0,-1,0);
	zero = Vector3(0,0,0);

	firstPerson = 1;
	topDown = 2;
	cameraMode = topDown;
	mousePos = Vector2(0.0f, 0.0f);
	lastMousePos = mousePos;
	
	splash.init(md3dDevice, 1.0f, White);

	//initialize texture resources
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"goon.jpg", 0, 0, &mDiffuseMapRV, 0 ));

	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"defaultspec.dds", 0, 0, &mSpecMapRV, 0 ));

	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"floor.jpg", 0, 0, &mFloorTex, 0 ));

	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"floor.dds", 0, 0, &mFloorSpec, 0 ));

	wallDetectionIsOn = false;

	CameraDirection = forward;
	camPos = Vector3(0.0f, 100.0f, 0.0f);
	camZoom = 1.0f;
	camTheta = 0.0f;
	camPhi = 0.0f;
	camTurnSpeed = 5;
	zoomSpeed = 5;
	maxZoom = 10.0f;
	minZoom = .8f;

	spotted = false;


	//init lights - using pointlights
	// parallel - 0
	// point - 1
	// spotlight - 2
	lightType = 1;
	int rows = 1, cols = 1;
	numberOfLights = rows * cols;
	float startRowPos = 0;
	float startColPos = 0;
	float spacing = 20;
	for (int i=0; i<rows * cols; ++i)
	{
		Light l;
		l.pos = Vector3(((i / cols) % rows) * spacing + startRowPos,
						90,
						(i % cols) * spacing + startColPos);
		//l.pos = Vector3(0,100,0);
		l.ambient = Color(0.67f, 0.67f, 0.67f);
		l.diffuse = Color(1.0f, 1.0f, 1.0f);
		l.specular = Color(1.0f, 1.0f, 1.0f);
		l.att.x = 0.0f;
		l.att.y = 0.017f;
		l.att.z = 0.00085f;
		l.range = 500.0f;
		l.type = lightType;
		lights.push_back(l);
	}
	ambientLight.pos = Vector3(0,0,0);
	ambientLight.ambient = Color(0.17f, 0.17f, 0.17f);

	spotLight.ambient  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	spotLight.diffuse  = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	spotLight.specular = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	spotLight.att.x    = 1.0f;
	spotLight.att.y    = 0.01f;
	spotLight.att.z    = 0.0007f;
	spotLight.spotPow  = 22.0f;
	spotLight.range    = 200.0f;
	player.init("Daniel", Vector3(0, 0, 0), 15, 17, 6, 3.3f, md3dDevice, &spotLight);

	level = new Level(md3dDevice);
	level->setPlayer(&player);
	level->fillLevel("level1.txt");
	numberOfSpotLights = level->spotLights.size();
	playState.pickUpsRemaining = level->pickups.size();
	
	floor.init(md3dDevice, level->getLevelSize().x * 4, level->getLevelSize().y * 4);


	buildFX();
	buildVertexLayouts();

	player.setDiffuseMap(mfxDiffuseMapVar);
	player.syncInput(input);
	//delete spotLight;
	level->setDiffuseMap(mfxDiffuseMapVar);
	level->setSpecMap(mfxSpecMapVar);
	
	mfxLightCount->SetInt(numberOfLights);
	mfxSpotCount->SetInt(numberOfSpotLights);
	mfxTexVar->SetInt(0);
}

void Game2App::onResize()
{
	D3DApp::onResize();

	float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, 2000.0f);
}

void Game2App::updateScene(float dt)
{
	D3DApp::updateScene(dt);

	if (gameState == TITLE) {
		//change camera, to point at splash quad
		gameState = HOWTO;
	} else if (gameState == HOWTO) {
		//change texture on splash to "how to" 
		gameState = PLAY;
	} else if (gameState == LEVELWIN) {
		if (playState.level > 3) {
			gameState = GAMEWIN;
		} else {
			playState.level += 1;
			playState.livesRemaining = 3;
			//delete level
			delete level;
		}
	} else if (gameState == GAMEWIN) {
		//display Congrats Splash screen
		gameState = CREDITS;
	}else if (gameState == GAMEOVER) {
		// change camera to point at game over quad
		gameState = CREDITS;
	} else if (gameState == CREDITS) {
		//display the Credits Splash screen
	} else if (gameState == PLAY) {
		level = new Level(md3dDevice);
		level->setPlayer(&player);
		if (playState.level == 2) {
			level->fillLevel("level2.txt");
		} else if (playState.level == 3) {
			level->fillLevel("level3.txt");
		}
		numberOfSpotLights = level->spotLights.size();
		playState.pickUpsRemaining = level->pickups.size();
		level->setDiffuseMap(mfxDiffuseMapVar);
		level->setSpecMap(mfxSpecMapVar);
	}
	lastMousePos = mousePos;
	mousePos = Vector2(input->getMouseX(), input->getMouseY());

	float gameTime = mTimer.getGameTime();

	Vector3 oldPlayerPos = player.getPosition();
	vector<Vector3> oldPerimeter;
	for (int i = 0; i < player.perimeter.size(); i++) {
		oldPerimeter.push_back(player.perimeter[i]);
	}
	spotted = false;
	player.update(dt);
	floor.update(dt);
	level->update(dt);
	// enemy sight detection
	for (int i=0; i<level->enemies.size(); ++i)
	{
		Enemy* e = level->enemies[i];
		Vector3 toPlayer = e->getPosition() - player.getPosition();
		Vector3 norm;
		Normalize(&norm, &(-toPlayer));
		float r = max(D3DXVec3Dot(&(norm), &e->getDirection()), 0);
		float l = Length(&toPlayer);
		if (r > 0.5f && l < e->getRange())
		{
			spotted = true;
			Ray ray = Ray(e->getPosition(), player.getPosition());
			if (wallDetectionIsOn)
			{
				for (int i=0; i<level->walls.size(); ++i)
				{
					Wall w = level->walls[i];
					Box2 b = Box2(w.getPosition() - Vector3(w.getRadii().x, 0.0f, w.getRadii().z),
						w.getPosition() + Vector3(w.getRadii().x, 0.0f, w.getRadii().z));
				
					if (b.intersect(ray, 0.0f, 1.0f))
					{
						spotted = false;
					}
				}
			}
		}
	}
	if(spotted) {
		audio->playCue(ALARM);
	}

	//collision stuff
	for (int i = 0; i < level->pickups.size(); i++) {
		if (level->pickups[i].contains(player.getPosition())) {
			if (level->pickups[i].isActive()) {
				audio->playCue(PICKUP);
			}
			level->pickups[i].setInActive();
		}
	}
	for (int i = 0; i < level->walls.size(); i++) {
		for (int j = 0; j < player.perimeter.size(); j++) {
			if (level->walls[i].contains(player.perimeter[j])) {
				player.colliding = true;
				player.setPosition(oldPlayerPos);
				for (int k = 0; k < player.perimeter.size(); k++) {
					player.perimeter[k] = oldPerimeter[k];
				}
				//level->walls[i].setInActive();
			}
		}
	}

	if (keyPressed(VK_RIGHT))
	{
		camTheta -= camTurnSpeed * dt;
		if (camTheta > 180 || camTheta < -180)
			camTheta = -camTheta;
	}
	if (keyPressed(VK_LEFT))
	{
		camTheta += camTurnSpeed * dt;
		if (camTheta > 180 || camTheta < -180)
			camTheta = -camTheta;
	}
	if (keyPressed(VK_UP))
	{
		camZoom += zoomSpeed * dt;
		if (camZoom > maxZoom)
			camZoom = maxZoom;
	}
	if (keyPressed(VK_DOWN))
	{
		camZoom -= zoomSpeed * dt;
		if (camZoom < minZoom)
			camZoom = minZoom;
	}
	if (input->wasKeyPressed(FirstPersonKey))
	{
		cameraMode = firstPerson;
	}
	if (input->wasKeyPressed(TopDownKey))
	{
		cameraMode = topDown;
	}
	// mouse camera control
	if (cameraMode == firstPerson)
	{
		camPhi += mousePos.y * dt;
	}
	if (input->wasKeyPressed(VK_RETURN))
	{
		if (wallDetectionIsOn)
			wallDetectionIsOn = false;
		else
			wallDetectionIsOn = true;
	}
	
	CameraDirection.x = sinf(camTheta);
	CameraDirection.z = cosf(camTheta);

	// Build the view matrix.
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	Vector3 lookAt(0.0f, 0.0f, 0.0f);
	if (cameraMode == topDown)
	{
		camPos = Vector3(0.0f, 150.0f / camZoom, 0.0f);
		camPos += player.getPosition();
		camPos -= CameraDirection * 150 / camZoom;
		target = player.getPosition();
	}
	else if (cameraMode == firstPerson)
	{
		camPos = Vector3(0.0f, player.getHeight(), 0.0f);
		camPos += player.getPosition();
		target = player.getPosition() + Vector3(0.0f, player.getHeight(), 0.0f);
		target += player.getDirection() * 20;
	}
	
	//pos -= Vector3(player.getDirection().x, -0.6f, player.getDirection().z) * 80.0f;
	
	
	
	
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&mView, &camPos, &target, &up);
	input->clearAll();
}

void Game2App::drawScene()
{
	D3DApp::drawScene();

	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDevice->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
    md3dDevice->IASetInputLayout(mVertexLayout);

	// set lighting shader variables
	mfxEyePosVar->SetRawValue(&camPos, 0, sizeof(Vector3));
	for (int i=0; i<numberOfLights; ++i)
	{
		mfxLightVar[i]->SetRawValue(&lights[i], 0, sizeof(Light));
	}
	mfxAmbientVar->SetRawValue(&ambientLight, 0, sizeof(Light));
	mfxLightType->SetInt(lightType);

	// light for Player
	mfxSpotVar->SetRawValue(&spotLight, 0, sizeof(Light));
	// light for Enemies
	/*for (int i = 0; i < numberOfSpotLights; i++) {
		mfxSpotVars.resize(level->spotLights.size());
	}*/
	for (int i = 0; i < level->spotLights.size(); i++) {
		mfxSpotVars[i]->SetRawValue(level->spotLights[i], 0, sizeof(Light));
	}

	// set some variables for the shader
	// set the point to the shader technique
	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);


	// Set the resoure variables
	
	mfxDiffuseMapVar->SetResource(mDiffuseMapRV);
	mfxSpecMapVar->SetResource(mSpecMapRV);

	// Set the texture matrix
	D3DXMATRIX texMtx;
	D3DXMatrixIdentity(&texMtx);
	mfxTexMtxVar->SetMatrix((float*)&texMtx);

	
	// Drawing the Player
	mfxTexVar->SetInt(0);
	mVP = mView * mProj;
	player.setMTech(mTech);
	player.setEffectVariables(mfxWVPVar, mfxWorldVar);
	player.draw(mVP);
	

	//Drawing the level
	level->setTextureUseVar(mfxTexVar);
	level->setDiffuseMap(mfxDiffuseMapVar);
	mfxTexVar->SetInt(0);
	mVP = mView * mProj;
	level->setMTech(mTech);
	level->setEffectVariables(mfxWVPVar, mfxWorldVar);
	level->draw(mVP);

	mfxTexVar->SetInt(1);
	mfxDiffuseMapVar->SetResource(mFloorTex);
	mfxSpecMapVar->SetResource(mFloorSpec);
	floor.setMTech(mTech);
	mfxWVPVar->SetMatrix(floor.getWorldMatrix() * mVP);
	mfxWorldVar->SetMatrix(floor.getWorldMatrix());
	floor.draw();
	mfxTexVar->SetInt(0);

	//Draw splash screen
	Identity(&mVP);

    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex( p )->Apply(0);
        splash.draw();
    }


	/////Text Drawing Section
	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {100, 5, 0, 0};
	RECT R1 = {0, 0, 800, 600};
	RECT R2 = {0, 540, 800, 600};

	std::wostringstream outs;  
	
	outs.precision(6);
	outs << "MouseX: " << mousePos.x << "\n";
	outs << "MouseY: " << mousePos.y << "\n";
	outs << "Camera Theta: " << camTheta << "\n";
	if (spotted)
		outs << "You are spotted!\n";
	outs << "Wall Detection: ";
	if (wallDetectionIsOn)
		outs << "On\n";
	else
		outs << "Off\n";
	string Hud = score.getString();
	

	/*outs << score.getString() << L"\n";
	outs << L"Blobs Available: " << ammo << L"\n";
	outs << L"Gallons Left: " << lives;*/
	std::wstring text = outs.str();
	mFont->DrawText(0, text.c_str(), -1, &R, DT_NOCLIP, White);
	timesNew.draw(Hud, Vector2(5, 5));
	/*if (gameOver)
	{
		mFont->DrawText(0, L"Game Over!", -1, &R1, DT_CENTER | DT_VCENTER, Black);
	}
	float gameTime = mTimer.getGameTime();
	if (gameTime < 3.0f)
	{
		mFont->DrawText(0, L"Move your Box LEFT and RIGHT with A & D to avoid hitting the obstacles", -1, &R2, DT_CENTER | DT_VCENTER, Black);
	}
	else if (gameTime < 6.0f)
	{
		mFont->DrawText(0, L"Change the color of your Box by pressing the J and L keys.", -1, &R2, DT_CENTER | DT_VCENTER, Black);
	}
	else if (gameTime < 9.0f)
	{
		mFont->DrawText(0, L"The closer the color of your cube is to the floor, the higher the score multiplier!", -1, &R2, DT_CENTER | DT_VCENTER, Black);
	}
	if (activeMessage)
	{
		mFont->DrawText(0, message.c_str(), -1, &R2, DT_CENTER | DT_VCENTER, Black);
	}*/
	

	mSwapChain->Present(0, 0);
}

void Game2App::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
//	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
 
	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX10CreateEffectFromFile(L"lighting.fx", 0, 0, 
		"fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &mFX, &compilationErrors, 0);
	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
	} 

	mTech = mFX->GetTechniqueByName("LightTech");
	
	mfxWVPVar = mFX->GetVariableByName("gWVP")->AsMatrix();
	mfxWorldVar  = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxEyePosVar = mFX->GetVariableByName("gEyePosW");
	for (int i=0; i<numberOfLights; ++i)
	{
		ID3D10EffectVariable* var = mFX->GetVariableByName("pointLights")->GetElement(i);
		mfxLightVar.push_back(var);
	}
	mfxAmbientVar = mFX->GetVariableByName("ambientLight");
	mfxSpotVar = mFX->GetVariableByName("spotLight");
	for (int i = 0; i < numberOfSpotLights; i++)
	{
		ID3D10EffectVariable* vars = mFX->GetVariableByName("spotLights")->GetElement(i);
		mfxSpotVars.push_back(vars);
	}
	mfxLightType = mFX->GetVariableByName("gLightType")->AsScalar();
	mfxLightCount = mFX->GetVariableByName("numberOfLights")->AsScalar();
	mfxSpotCount = mFX->GetVariableByName("numberOfSpotLights")->AsScalar();
	mfxTexVar = mFX->GetVariableByName("useTex")->AsScalar();

	mfxDiffuseMapVar = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	mfxSpecMapVar    = mFX->GetVariableByName("gSpecMap")->AsShaderResource();
	mfxTexMtxVar     = mFX->GetVariableByName("gTexMtx")->AsMatrix();
	//mfxFLIPVar = mFX->GetVariableByName("flip");

}

void Game2App::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"DIFFUSE",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"SPECULAR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 56, D3D10_INPUT_PER_VERTEX_DATA, 0},
	};

	// Create the input layout
    D3D10_PASS_DESC PassDesc;
    mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 5, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}






// Optimized method
bool Box2::intersect(const Ray &r, float t0, float t1){
	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	tmin = (bounds[r.sign[0]].x - r.origin.x) * r.inv_direction.x;
	tmax = (bounds[1-r.sign[0]].x - r.origin.x) * r.inv_direction.x;
	tymin = (bounds[r.sign[1]].y - r.origin.y) * r.inv_direction.y;
	tymax = (bounds[1-r.sign[1]].y - r.origin.y) * r.inv_direction.y;
	if ( (tmin > tymax) || (tymin > tmax) )
	return false;
	if (tymin > tmin)
	tmin = tymin;
	if (tymax < tmax)
	tmax = tymax;
	tzmin = (bounds[r.sign[2]].z - r.origin.z) * r.inv_direction.z;
	tzmax = (bounds[1-r.sign[2]].z - r.origin.z) * r.inv_direction.z;
	if ( (tmin > tzmax) || (tzmin > tmax) )
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;
	return ( (tmin < t1) && (tmax > t0) );
}