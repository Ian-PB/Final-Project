#include "StructureBuilding.h"
#include <raylib.h>
#include <raymath.h>
#include <iostream>

StructureBuilding::StructureBuilding()
{
    camera.position = { 0.0f, 2.0f, 0.0f };                     // Camera position
    camera.target = { 2.0f, 0.0f, 0.0f };               // Camera looking at point
    camera.up = { 0.0f, 1.0f, 0.0f };                   // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;                 // Camera projection type
}

void StructureBuilding::Init()
{
    DisableCursor();


    objects.push_back(std::make_shared<BaseBuildingBlock>(camera));
}

void StructureBuilding::Draw()
{
	BeginMode3D(camera);

    DrawPlane({ 0.0f, 0.0f, 0.0f }, { 100, 100 }, RAYWHITE);

    for (const std::shared_ptr<BaseBuildingBlock> obj : objects)
    {
        obj->Draw(editMode);
    }

	EndMode3D();

    // Quick crosshair
    DrawCircle(GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f, 3, WHITE);
    DrawCircle(GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f, 2, RED);
}

void StructureBuilding::Update()
{
    // Temp
    if (IsKeyPressed(KEY_N))
    {
        objects.push_back(std::make_shared<BaseBuildingBlock>(camera));

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
