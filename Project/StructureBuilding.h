#pragma once
#include <raylib.h>

class StructureBuilding
{
public:
	StructureBuilding();

	void Init();
	void Draw();
	void Update();

private:
	Camera3D camera;
};

