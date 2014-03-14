#pragma once
#include "Box.h"
#include "GameObject.h"
#include "constants.h"
#include "d3dUtil.h"
#include "input.h"
#include <math.h>
#include <vector>
#include <string>
#include "audio.h"

class Floor : public GameObject
{
public:
	Floor();
	~Floor();

	void init(float xSize, float ySize);

private:
	Vector2 dimensions;

	ID3D10Device* device;
};