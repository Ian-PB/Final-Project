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
}

void StructureBuilding::Draw()
{
	BeginMode3D(camera);

    DrawPlane({ 0.0f, 0.0f, 0.0f }, { 100, 100 }, RAYWHITE);

	EndMode3D();
}

void StructureBuilding::Update()
{
    UpdateCamera(&camera, CAMERA_FREE);
}
