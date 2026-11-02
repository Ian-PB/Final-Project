#include "StructureBuilding.h"
#include <raylib.h>
#include <raymath.h>

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

    testObject.Init();
}

void StructureBuilding::Draw()
{
	BeginMode3D(camera);

    DrawPlane({ 0.0f, 0.0f, 0.0f }, { 100, 100 }, RAYWHITE);

    testObject.Draw(editMode);

	EndMode3D();

    // Quick crosshair
    DrawCircle(GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f, 3, WHITE);
    DrawCircle(GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f, 2, RED);
}

void StructureBuilding::Update()
{
    UpdateCamera(&camera, CAMERA_FREE);
    GetEditModeInputs();

    testObject.Update(editMode);
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
