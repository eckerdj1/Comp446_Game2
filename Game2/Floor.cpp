#include "Floor.h"

Floor::Floor()
{
	dimensions = Vector2(1, 1);
}

Floor::~Floor()
{

}

void Floor::init(float xSize, float ySize)
{
	Box* b = new Box();
	b->init(device, 1, Gray);
	GameObject::init(b, Vector3(0,0,0), Vector3(0,0,0), Vector3(xSize, 0.1f, ySize) , 0);

	dimensions.x = xSize;
	dimensions.y = ySize;
}