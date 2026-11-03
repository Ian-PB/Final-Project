#include "StructureBuilding.h"
#include <raylib.h>
#include <raymath.h>
#include <iostream>

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

    SpawnObject();
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

        for (const std::shared_ptr<BaseBuildingBlock> obj : objects)
        {
            obj->Draw(editMode);
        }

        world.Draw();

	EndMode3D();

    // Quick crosshair
    DrawCircle(GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f, 3, WHITE);
    DrawCircle(GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f, 2, RED);
}

void StructureBuilding::Update()
{
    world.Update();

    // Temp
    if (IsKeyPressed(KEY_N))
    {
        SpawnObject();
    }

    if (IsMouseButtonPressed(0))
    {
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

    for (const std::shared_ptr<BaseBuildingBlock> obj : objects)
    {
        obj->Update(editMode);
    }
}

void StructureBuilding::SpawnObject()
{
    std::shared_ptr<BaseBuildingBlock> newObj = std::make_shared<BaseBuildingBlock>(camera);
    Vector3 pos = { worldCenter.x, 2.0f, worldCenter.x };
    newObj->SetPos(pos);

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
