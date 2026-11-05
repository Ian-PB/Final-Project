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
	SelectedEditting
};

class StructureBuilding
{
public:
	StructureBuilding();

	void Init();
	void Draw();
	void Update();

private:
	void CheckKeybinds();

	UiModes currentUiMode = UiModes::Off;
	void DrawUI();
	void DrawSpawnObjUI();

	void DrawSelectedEdittingPanel();
	bool posXEditMode = false;
	bool posYEditMode = false;
	bool posZEditMode = false;
	bool scaleXEditMode = false;
	bool scaleYEditMode = false;
	bool scaleZEditMode = false;
	bool rotXEditMode = false;
	bool rotYEditMode = false;
	bool rotZEditMode = false;

	void Bake();
	void ResetBaking();

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

