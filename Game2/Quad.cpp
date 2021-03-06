
#include "Quad.h"
#include "Vertex.h"
#include "constants.h"

Quad::Quad()
: mNumVertices(0), mNumFaces(0), md3dDevice(0), mVB(0), mIB(0)
{
	rotX = 0;
	rotY = 0;
	rotZ = 0;
	Identity(&world);
}
 
Quad::~Quad()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
}


void Quad::init(ID3D10Device* device, float scale, D3DXCOLOR c)
{
	init(device, scale, scale, c);
}
void Quad::init(ID3D10Device* device, float scaleX, float scaleY, D3DXCOLOR c)
{
	md3dDevice = device;
 
	mNumVertices = 4; //2 triangles per quad
	mNumFaces    = 2; 

	Vector2 tl, tr, bl, br;
	tl = Vector2(0.0f, 0.0f);
	tr = Vector2(1.0f, 0.0f);
	bl = Vector2(0.0f, 1.0f);
	br = Vector2(1.0f, 1.0f);

	// Create vertex buffer
	float x = 0.5f;
	float y = 1.0f;
	Vector3 norm = Vector3(0.0f, 0.0f, -1.0f);
	float specM = 0.7f;
	float specPower = 64.0f;
	DXColor spec = DXColor(c.r * specM, c.g * specM, c.b * specM, specPower);

	Vertex vertices[] =
    {
		{D3DXVECTOR3(-x, y, 0.0f), norm, c, spec, tl},
		{D3DXVECTOR3(x, y, 0.0f), norm, c, spec, tr},
		{D3DXVECTOR3(x, 0, 0.0f), norm, c, spec, br},
		{D3DXVECTOR3(-x, 0, 0.0f), norm, c, spec, bl}
    };

	// Scale the Quad.
	for(DWORD i = 0; i < mNumVertices; ++i)
	{
		vertices[i].pos.x *= scaleX;
		vertices[i].pos.y *= scaleY;
	}

    D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = vertices;
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));

	//Index buffer
	DWORD indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3

	};


	D3D10_BUFFER_DESC ibd;
    ibd.Usage = D3D10_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(DWORD) * mNumFaces*3;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}

void Quad::update(float dt)
{
	Matrix rotXM, rotYM, rotZM, transM;
	RotateX(&rotXM, rotX);
	RotateY(&rotYM, rotY);
	RotateZ(&rotZM, rotZ); 
	Translate(&transM, position.x, position.y, position.z);
	world = rotXM * rotYM * rotZM * transM;
}

void Quad::draw()
{
UINT stride = sizeof(Vertex);
    UINT offset = 0;
	md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	md3dDevice->DrawIndexed(mNumFaces*3, 0, 0);
}