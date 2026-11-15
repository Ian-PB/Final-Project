#pragma once
#include <raylib.h>
#include <memory>
#include <vector>
#include "BaseBuildingBlock.h"
#include "World.h"
#include "StructureSaver.h"
#include "StructureData.h"
#include "Materials.h"

enum class UiModes
{
	Off = -1,
	SpawnObj,
	SelectedEditting,
	SavingStructure,
	SetSelectedMaterial
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

	void DrawSavingStructurePanel();
	static const int MAX_NAME_SIZE = 64;
	char structureName[MAX_NAME_SIZE] = "Name";
	bool nameEditMode = false;
	StructureTypes selectedType = StructureTypes::None;
	bool typeDropdownOpen = false;

	void DrawSelectMaterialPanel();
	MaterialTypes materialSelected = MaterialTypes::Stone;
	int materialInt = 0;
	bool materialDropdownOpen = false;
	const int COLOR_THRESHOLD = 0.1f;

	void Bake();
	void ResetBaking();

	Index3D Get3DIndex(int i, int axisSize);
	BoundingBox FindStructureBounds();

	void SpawnCube();
	void SpawnSphere();
	void SpawnCylinder();
	void SpawnCone();
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

	std::shared_ptr<StructureSaver> structureSaver;

	World world;
	Vector3 worldCenter;
	Vector3 worldSize;
	BoundingBox structureBounds;
};

