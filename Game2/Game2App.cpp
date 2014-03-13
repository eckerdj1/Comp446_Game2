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

	////// New Stuff added by Steve //////
	Box playerBox;
	Box lineBox;
	GameObject outline[11];
	Player player;
	int numberOfObstacles;
	vector<Box*> obstacleBoxes;
	vector<Obstacle> obstacles;
	/////New obstacle code: Daniel J. Ecker////
	float floorMovement;
	int clusterSize, clusterSizeVariation, clusterSeparation;
	int cubeSeparation, lineJiggle, cubeJiggle, clusterJiggle;
	//////////////////////////////////////
	///Floor
	//Floor floor;

	//Lighting
	vector<Light> lights;
	int numberOfLights;
	int lightType; // 0-parallel, 1-pointlight, 2-spotlight

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

	//New Spectrum HUD stuff by Andy
	Box specHudBox[6];
	Box cursorBox;
	GameObject spectrum[6];
	GameObject cursor;

	float getMultiplier();


	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10InputLayout* mVertexLayout;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	//light variables
	ID3D10EffectMatrixVariable* mfxWorldVar;
	ID3D10EffectVariable* mfxEyePosVar;
	ID3D10EffectVariable* mfxLightVar;
	ID3D10EffectScalarVariable* mfxLightType;


	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mVP;

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
	
	//audio->playCue(MAIN_TRACK);

	srand(time(0));

	left = Vector3(1,0,0);
	right = Vector3(-1,0,0);
	forward = Vector3(0,0,-1);
	back = Vector3(0,0,1);
	up = Vector3(0,1,0);
	down = Vector3(0,-1,0);
	zero = Vector3(0,0,0);

	player.init("Daniel", Vector3(0,0, 0), 15, 17, 6, 3.3f, md3dDevice);





	//init lights - using pointlights
	lightType = 1;
	numberOfLights = 1;
	for (int i=0; i<numberOfLights; ++i)
	{
		Light l;
		l.pos = Vector3(0, 50, -17);
		l.ambient = Color(0.67f, 0.67f, 0.67f);
		l.diffuse = Color(1.0f, 1.0f, 1.0f);
		l.specular = Color(1.0f, 1.0f, 1.0f);
		l.att.x = 1.5f;
		l.att.y = 0.0f;
		l.att.z = 0.0f;
		l.range = 97.0f;
		lights.push_back(l);
	}


	buildFX();
	buildVertexLayouts();

}

void Game2App::onResize()
{
	D3DApp::onResize();

	float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);
}

void Game2App::updateScene(float dt)
{
	D3DApp::updateScene(dt);

	float gameTime = mTimer.getGameTime();


	player.update(dt);


	// Build the view matrix.
	D3DXVECTOR3 pos(0.0f,45.0f,-50.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&mView, &pos, &target, &up);
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
	
	D3DXVECTOR3 pos(0.0f,45.0f,-50.0f);
	// set lighting shader variables
	mfxEyePosVar->SetRawValue(&pos, 0, sizeof(Vector3));
	mfxLightVar->SetRawValue(&lights[0], 0, sizeof(Light));
	mfxLightType->SetInt(lightType);

	// set some variables for the shader
	// set the point to the shader technique
	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);


	//setting the color flip variable in the shader

	

	////// New Stuff added by Steve //////
	mVP = mView * mProj;
	player.setMTech(mTech);
	player.setEffectVariables(mfxWVPVar, mfxWorldVar);
	player.draw(mVP);


	/////Text Drawing Section
	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	RECT R1 = {0, 0, 800, 600};
	RECT R2 = {0, 540, 800, 600};

	std::wostringstream outs;  
	
	outs.precision(6);
	string Hud = score.getString();

	/*outs << score.getString() << L"\n";
	outs << L"Blobs Available: " << ammo << L"\n";
	outs << L"Gallons Left: " << lives;
	std::wstring text = outs.str();
	mFont->DrawText(0, text.c_str(), -1, &R, DT_NOCLIP, BLACK);*/
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
	mfxLightVar = mFX->GetVariableByName("gLight");
	mfxLightType = mFX->GetVariableByName("gLightType")->AsScalar();
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
		{"SPECULAR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
    D3D10_PASS_DESC PassDesc;
    mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 4, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}
 
void Game2App::setNewObstacleCluster()
{
	float obstacleScale = 2.5f;
	float startZ = 125.0f;
	float currentZ = startZ;
	float laneSize = 6.0f;
	Vector3 oScale(obstacleScale, obstacleScale, obstacleScale);
	bool lane[5];
	for (int i=0; i<5; ++i)
	{
		lane[i] = false;
	}

	
	int cs = clusterSize + rand() % clusterSizeVariation;
	while(cs)	//put lines of cubes in the same cluster
	{	
		int cubesOnLine = rand() % 4 + 1;
		while (cubesOnLine) //puts cubes on the same line
		{
			float thisZ = currentZ + (rand() % lineJiggle);
			int pickLane = rand() % 5;
			while (lane[pickLane])
			{
				pickLane = rand() % 5;
			}
			float thisX = -12.0f + pickLane * laneSize;
			lane[pickLane] = true;
			cubesOnLine--;
			for (int i=0; i<obstacles.size(); ++i)
			{
				if (obstacles[i].isNotActive())
				{
					obstacles[i].setActive();
					obstacles[i].setPosition(Vector3(thisX, 1, thisZ));
					obstacles[i].setSpeed(0);
					break;
				}
			}
			
		}
		for (int i=0; i<5; ++i)
		{
			lane[i] = false;
		}
		currentZ += (int)(cubeSeparation + rand() % cubeJiggle);
		cs--;
	}
}


