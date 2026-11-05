#pragma once
#include <raylib.h>
#include <memory>
#include <vector>
#include "BaseBuildingBlock.h"
#include "World.h"

enum class UiModes
{
	Off = -1,
	SpawnObj,
	EditSelectedObj
};

class StructureBuilding
{
public:
	StructureBuilding();

	void Init();
	void Draw();
	void Update();

private:
	UiModes currentUiMode = UiModes::Off;
	void DrawUI();
	void DrawSpawnObjUI();

	void Bake();

	void SpawnCube();
	void SpawnSphere();
	void SpawnCylinder();
	void CheckObjectClicked();

	void GetEditModeInputs();
	EditTypes editMode = EditTypes::Move;

	bool RemoveSelectedObj();
	bool IsHoveringAxis();
	bool displayObjs = true;

	std::vector<std::shared_ptr<BaseBuildingBlock>> objects;
	std::shared_ptr<BaseBuildingBlock> selectedObject;

	Camera3D camera;

	bool isDragging = false;

	World world;
	Vector3 worldCenter;
	Vector3 worldSize;
};

