#include "StructureBuilding.h"
#include <raylib.h>
#include <raymath.h>
#include <raygui.h>
#include <iostream>

#include "SphereBuildingBlock.h"
#include "CylinderBuildingBlock.h"

StructureBuilding::StructureBuilding()
{
    camera.up = { 0.0f, 1.0f, 0.0f };                   // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;                 // Camera projection type
}

void StructureBuilding::Init()
{
    DisableCursor();

    Lighting::Init();
    world.Init();
    worldSize = world.GetSizeDimensions();
    worldCenter = Vector3Multiply(worldSize, { 0.5f, 0.5f, 0.5f });
    world.EmptyWorldGen();

    camera.position = { worldCenter.x, 2.0f, worldCenter.z };
    camera.target = { worldCenter.x + 2.0f, 2.0f, worldCenter.z };

    SpawnCube();
}

void StructureBuilding::Draw()
{
	BeginMode3D(camera);

        // Ground
        DrawPlane({ worldCenter.x, 0.0f, worldCenter.z }, { worldSize.x, worldSize.z }, GRAY);
        // Corner pillar
        DrawCylinder({ 0.0f, 0.0f, 0.0f }, 0.2f, 0.2f, worldSize.y, 5, RED);
        DrawCylinder({ worldSize.x, 0.0f, 0.0f }, 0.2f, 0.2f, worldSize.y, 5, RED);
        DrawCylinder({ 0.0f, 0.0f, worldSize.z }, 0.2f, 0.2f, worldSize.y, 5, RED);
        DrawCylinder({ worldSize.x, 0.0f, worldSize.z }, 0.2f, 0.2f, worldSize.y, 5, RED);
        // Roof edges
        DrawCylinderEx({ 0.0f, worldSize.y, 0.0f }, { worldSize.x, worldSize.y, 0.0f }, 0.2f, 0.2f, worldSize.y, RED);
        DrawCylinderEx({ 0.0f, worldSize.y, 0.0f }, { 0.0f, worldSize.y, worldSize.z }, 0.2f, 0.2f, worldSize.y, RED);
        DrawCylinderEx({ 0.0f, worldSize.y, worldSize.z }, { worldSize.x, worldSize.y, worldSize.z }, 0.2f, 0.2f, worldSize.y, RED);
        DrawCylinderEx({ worldSize.x, worldSize.y, worldSize.z }, { worldSize.x, worldSize.y, 0.0f }, 0.2f, 0.2f, worldSize.y, RED);


        if (displayObjs)
        {
            for (const std::shared_ptr<BaseBuildingBlock> obj : objects)
            {
                obj->Draw(editMode);
            }
        }

        world.Draw();

	EndMode3D();

    // Quick crosshair
    DrawCircle(GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f, 3, WHITE);
    DrawCircle(GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f, 2, RED);

    if (currentUiMode != UiModes::Off)
        DrawUI();
}

void StructureBuilding::Update()
{
    // Temp
    if (IsKeyPressed(KEY_N))
    {
        if (currentUiMode == UiModes::Off)
        {
            currentUiMode = UiModes::SpawnObj;
            EnableCursor();
        }
        else
        {
            currentUiMode = UiModes::Off;
            DisableCursor();
        }
    }

    // Don't update the rest if in a menu
    if (currentUiMode != UiModes::Off)
        return;

    if (IsMouseButtonPressed(0))
    {
        if (!IsHoveringAxis())
            CheckObjectClicked();
    }

    for (const std::shared_ptr<BaseBuildingBlock> obj : objects)
    {
        isDragging = obj->IsDragging();

        if (isDragging)
            break;
    }

    if (!isDragging)
        UpdateCamera(&camera, CAMERA_FREE);

    GetEditModeInputs();

    if (displayObjs)
    {
        for (const std::shared_ptr<BaseBuildingBlock> obj : objects)
        {
            obj->Update(editMode);
        }
    }

    // Bake objects into the world
    if (IsKeyPressed(KEY_B))
    {
        Bake();
        displayObjs = false;
    }
    world.Update();
}

void StructureBuilding::DrawUI()
{
    switch (currentUiMode)
    {
    case UiModes::SpawnObj:
        DrawSpawnObjUI();
        break;
    case UiModes::EditSelectedObj:
        DrawSpawnObjUI();
        break;
    }
}

void StructureBuilding::DrawSpawnObjUI()
{
    bool objSpawned = false;
    Rectangle bounds{};
    bounds.width = GetScreenWidth() * 0.05f;
    bounds.height = GetScreenHeight() * 0.05f;

    bounds.x = GetScreenWidth() * 0.05f;
    bounds.y = GetScreenHeight() * 0.05f;
    if (GuiButton(bounds, "Cube"))
    {
        SpawnCube();
        objSpawned = true;
    }

    bounds.x += GetScreenWidth() * 0.1f;
    if (GuiButton(bounds, "Sphere"))
    {
        SpawnSphere();
        objSpawned = true;
    }

    bounds.x += GetScreenWidth() * 0.1f;
    if (GuiButton(bounds, "Cylinder"))
    {
        SpawnCylinder();
        objSpawned = true;
    }

    // If any buttons were pressed remove ui
    if (objSpawned)
    {
        currentUiMode = UiModes::Off;
        DisableCursor();
    }
}

void StructureBuilding::Bake()
{
    for (const std::shared_ptr<BaseBuildingBlock> obj : objects)
    {
        BoundingBox objBounds = obj->GetBounds();
        objBounds.min = Vector3SubtractValue(objBounds.min, obj->GetBoundingSkin());
        objBounds.max = Vector3AddValue(objBounds.max, obj->GetBoundingSkin());

        std::vector<Chunk*> chunks = world.GetChunksInBounds(obj->GetBounds());

        // Loop through all intersecting chunks
        for (Chunk* chunk : chunks)
        {
            bool modified = false;

            for (int i = 0; i < chunk->GetPointAmount(); i++)
            {
                Point& point = chunk->GetPointAt(i);
                Vector3 worldPointPos = Vector3Add(point.position, chunk->GetPosition());

                // Change densities
                if (obj->ChangePointDensity(worldPointPos, point.density))
                   modified = true;
            }

            if (modified)
            {
                chunk->SetDirty();
                world.QueueChunkChangeAtPos(chunk->GetPosition());
            }
        }
    }
}

void StructureBuilding::SpawnCube()
{
    std::shared_ptr<BaseBuildingBlock> newObj = std::make_shared<BaseBuildingBlock>(camera);
    Vector3 pos = { worldCenter.x, 2.0f, worldCenter.x };
    newObj->SetPos(pos);
    newObj->InitModel();

    objects.push_back(newObj);
}

void StructureBuilding::SpawnSphere()
{
    std::shared_ptr<BaseBuildingBlock> newObj = std::make_shared<SphereBuildingBlock>(camera);
    Vector3 pos = { worldCenter.x, 2.0f, worldCenter.x };
    newObj->SetPos(pos);
    newObj->InitModel();

    objects.push_back(newObj);
}

void StructureBuilding::SpawnCylinder()
{
    std::shared_ptr<BaseBuildingBlock> newObj = std::make_shared<CylinderBuildingBlock>(camera);
    Vector3 pos = { worldCenter.x, 2.0f, worldCenter.x };
    newObj->SetPos(pos);
    newObj->InitModel();

    objects.push_back(newObj);
}

void StructureBuilding::CheckObjectClicked()
{
    Vector3 dir = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    Ray ray = { camera.position, dir };

    float closestDist = 9999.0f;
    int closestIndex = -1;

    for (int i = 0; i < objects.size(); i++)
    {
        RayCollision collision = objects[i]->CheckIfClicked(ray);

        if (collision.hit && collision.distance < closestDist)
        {
            closestDist = collision.distance;
            closestIndex = i;
        }
    }

    std::cout << closestIndex << std::endl;

    // Not over any
    if (closestIndex == -1)
        return;

    // Disable last one
    if (selectedObject != nullptr)
        selectedObject->SetSelected(false);

    // Set selected object
    selectedObject = objects[closestIndex];
    selectedObject->SetSelected(true);
}

void StructureBuilding::GetEditModeInputs()
{
    if (IsKeyPressed(KEY_ONE))
        editMode = EditTypes::Move;

    else if (IsKeyPressed(KEY_TWO))
        editMode = EditTypes::Rotate;

    else if (IsKeyPressed(KEY_THREE))
        editMode = EditTypes::Scale;
}

bool StructureBuilding::IsHoveringAxis()
{
    for (const std::shared_ptr<BaseBuildingBlock> obj : objects)
    {
        // An axis is hovered
        if (obj->GetHoveredAxis() != Axis::None)
            return true;
    }
    return false;
}
