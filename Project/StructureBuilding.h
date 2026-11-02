#pragma once
#include <raylib.h>
#include "BaseBuildingBlock.h"

class StructureBuilding
{
public:
	StructureBuilding();

	void Init();
	void Draw();
	void Update();

private:
	void GetEditModeInputs();
	EditTypes editMode = EditTypes::Move;

	BaseBuildingBlock testObject;

	Camera3D camera;
};

