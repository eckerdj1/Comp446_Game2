//=======================================================================================
// Box.h by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#ifndef BOX_H
#define BOX_H

#include "d3dUtil.h"
#include "constants.h"
#include "CubeTex.h"
#include "Quad.h"

class Player;

class Box
{
public:

	Box();
	~Box();

	void init(ID3D10Device* device, float scale);
	void init(ID3D10Device* device, float scale, D3DXCOLOR c);
	void init(ID3D10Device* device, float sX, float sY, float sZ, D3DXCOLOR c1, D3DXCOLOR c2);
	void draw();
	void releaseVBuffer();
	
	void setDiffuseMap(ID3D10EffectShaderResourceVariable* var);

	void setVertexColor(DXColor c1,DXColor c2);
	DXColor getColor1();
	DXColor getColor2();

private:
	DWORD mNumVertices;
	DWORD mNumFaces;
	
	CubeTex textures;
	bool textured;

	float scaleX, scaleY, scaleZ;
	DXColor color1, color2;

	Quad frontTex, backTex, leftSideTex, rightSideTex, topTex, bottomTex;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;

	ID3D10EffectShaderResourceVariable* diffuseMapVar;
};


#endif // BOX_H
