#pragma once
#include <raylib.h>

class Collider
{
public:

	virtual bool CheckCollision(Vector3& t_newPos) = 0;
protected:

	Vector2 position;
};