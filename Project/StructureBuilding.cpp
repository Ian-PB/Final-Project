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
    else
    {
        // Keybinds Text
        DrawText("T - Manually Edit Selected Object", GetScreenWidth() * 0.01f, GetScreenHeight() * 0.01f, 25, RED);
        DrawText("N - Spawn New Object", GetScreenWidth() * 0.01f, GetScreenHeight() * 0.01f + 30, 25, RED);
        DrawText("DELETE - Remove Selected Object", GetScreenWidth() * 0.01f, GetScreenHeight() * 0.01f + 60, 25, RED);
    }
}

void StructureBuilding::Update()
{
    CheckKeybinds();

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

    world.Update();
}

void StructureBuilding::CheckKeybinds()
{
    // Object spawn
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

    // Swap open selected objects info
    if (IsKeyPressed(KEY_T) && selectedObject != nullptr)
    {
        if (currentUiMode == UiModes::Off)
        {
            currentUiMode = UiModes::SelectedEditting;
            EnableCursor();
        }
        else
        {
            currentUiMode = UiModes::Off;
            DisableCursor();
        }
    }

    // Remove selected object
    if (IsKeyPressed(KEY_DELETE))
    {
        if (RemoveSelectedObj())
        {
            // If a object was removed
            selectedObject = nullptr;
        }
    }

    // Bake objects into the world
    if (IsKeyPressed(KEY_B))
    {
        if (displayObjs)
            Bake();
        else
            ResetBaking();
    }
}

void StructureBuilding::DrawUI()
{
    switch (currentUiMode)
    {
    case UiModes::SpawnObj:
        DrawSpawnObjUI();
        break;
    case UiModes::SelectedEditting:
        DrawSelectedEdittingPanel();
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

void StructureBuilding::DrawSelectedEdittingPanel()
{
    const Rectangle PANEL_RECT = { GetScreenWidth() * 0.02f, GetScreenHeight() * 0.02f, GetScreenWidth() * 0.2f, GetScreenHeight() * 0.4f };
    DrawRectangleRounded(PANEL_RECT, 0.2, 10, LIGHTGRAY);
    DrawRectangleRoundedLines(PANEL_RECT, 0.2, 10, DARKGRAY);

    // Element Setup
    Rectangle elementRect;
    elementRect.width = PANEL_RECT.width * 0.7f;
    elementRect.height = PANEL_RECT.height * 0.07f;
    const float ELEMENT_X_START = PANEL_RECT.x + PANEL_RECT.width * 0.20f;
    elementRect.x = ELEMENT_X_START;
    elementRect.y = PANEL_RECT.y + PANEL_RECT.height * 0.1f;

    // Position editting
    Vector3 originalPos = selectedObject->GetPos();
    int posX = originalPos.x;
    int posY = originalPos.y;
    int posZ = originalPos.z;

    if (GuiSpinner(elementRect, "Position X", &posX, -worldSize.x, worldSize.x, posXEditMode))
        posXEditMode = !posXEditMode;
    elementRect.y += elementRect.height;
    if (GuiSpinner(elementRect, "Position Y", &posY, -worldSize.y, worldSize.y, posYEditMode))
        posYEditMode = !posYEditMode;
    elementRect.y += elementRect.height;
    if (GuiSpinner(elementRect, "Position Z", &posZ, -worldSize.z, worldSize.z, posZEditMode))
        posZEditMode = !posZEditMode;
    elementRect.y += elementRect.height;
    selectedObject->SetPos({ (float)posX, (float)posY, (float)posZ });

    // Spacing
    elementRect.x = ELEMENT_X_START;
    elementRect.y += elementRect.width * 0.02f;

    // Scale editting
    Vector3 originalScale = selectedObject->GetScale();
    int scaleX = originalScale.x;
    int scaleY = originalScale.y;
    int scaleZ = originalScale.z;

    if (GuiSpinner(elementRect, "Scale X", &scaleX, 1, 999, scaleXEditMode))
        scaleXEditMode = !scaleXEditMode;
    elementRect.y += elementRect.height;
    if (GuiSpinner(elementRect, "Scale Y", &scaleY, 1, 999, scaleYEditMode))
        scaleYEditMode = !scaleYEditMode;
    elementRect.y += elementRect.height;
    if (GuiSpinner(elementRect, "Scale Z", &scaleZ, 1, 999, scaleZEditMode))
        scaleZEditMode = !scaleZEditMode;
    elementRect.y += elementRect.height;
    selectedObject->SetScale({ (float)scaleX, (float)scaleY, (float)scaleZ });

    // Spacing
    elementRect.x = ELEMENT_X_START;
    elementRect.y += elementRect.width * 0.01f;

    // Rotation editting
    Vector3 originalRot = selectedObject->GetRotation();
    int rotX = roundf(originalRot.x * RAD2DEG);
    int rotY = roundf(originalRot.y * RAD2DEG);
    int rotZ = roundf(originalRot.z * RAD2DEG);

    if (GuiSpinner(elementRect, "Rotation X", &rotX, -180, 180, rotXEditMode))
        rotXEditMode = !rotXEditMode;
    elementRect.y += elementRect.height;
    if (GuiSpinner(elementRect, "Rotation Y", &rotY, -180, 180, rotYEditMode))
        rotYEditMode = !rotYEditMode;
    elementRect.y += elementRect.height;
    if (GuiSpinner(elementRect, "Rotation Z", &rotZ, -180, 180, rotZEditMode))
        rotZEditMode = !rotZEditMode;
    elementRect.y += elementRect.height;
    selectedObject->SetRotation({ (float)rotX * DEG2RAD, (float)rotY * DEG2RAD, (float)rotZ * DEG2RAD });

    // Follow the object while in this mode
    camera.target = selectedObject->GetPos();

    // Spacing
    elementRect.x = PANEL_RECT.x + PANEL_RECT.width * 0.15f;
    elementRect.y += elementRect.width * 0.1f;

    // Close Button
    if (GuiButton(elementRect, "Close (T)"))
    {
        currentUiMode = UiModes::Off;
        DisableCursor();
    }
}

void StructureBuilding::Bake()
{
    bool anyModificationsMade = false;

    for (const std::shared_ptr<BaseBuildingBlock> obj : objects)
    {
        BoundingBox objBounds = obj->GetBounds();
        objBounds.min = Vector3SubtractValue(objBounds.min, obj->GetBoundingSkin());
        objBounds.max = Vector3AddValue(objBounds.max, obj->GetBoundingSkin());

        std::vector<Chunk*> chunks = world.GetChunksInBounds(objBounds);

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
                if (!anyModificationsMade)
                    anyModificationsMade = true;

                chunk->SetDirty();
                world.QueueChunkChangeAtPos(chunk->GetPosition());
            }
        }
    }

    // Only stop the display of objects if there was a single edit made
    if (anyModificationsMade)
        displayObjs = false;
}

void StructureBuilding::ResetBaking()
{
    world.EmptyWorldGen();
    displayObjs = true;
}

void StructureBuilding::SpawnCube()
{
    std::shared_ptr<BaseBuildingBlock> newObj = std::make_shared<BaseBuildingBlock>(camera);
    Vector3 pos = { worldCenter.x, 2.0f, worldCenter.x };
    newObj->SetPos(pos);
    newObj->InitModel();

    // Select new object
    if (selectedObject != nullptr)
        selectedObject->SetSelected(false);
    selectedObject = newObj;
    selectedObject->SetSelected(true);

    objects.push_back(newObj);
}

void StructureBuilding::SpawnSphere()
{
    std::shared_ptr<BaseBuildingBlock> newObj = std::make_shared<SphereBuildingBlock>(camera);
    Vector3 pos = { worldCenter.x, 2.0f, worldCenter.x };
    newObj->SetPos(pos);
    newObj->InitModel();

    // Select new object
    if (selectedObject != nullptr)
        selectedObject->SetSelected(false);
    selectedObject = newObj;
    selectedObject->SetSelected(true);

    objects.push_back(newObj);
}

void StructureBuilding::SpawnCylinder()
{
    std::shared_ptr<BaseBuildingBlock> newObj = std::make_shared<CylinderBuildingBlock>(camera);
    Vector3 pos = { worldCenter.x, 2.0f, worldCenter.x };
    newObj->SetPos(pos);
    newObj->InitModel();

    // Select new object
    if (selectedObject != nullptr)
        selectedObject->SetSelected(false);
    selectedObject = newObj;
    selectedObject->SetSelected(true);

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

    // Disable last one
    if (selectedObject != nullptr)
        selectedObject->SetSelected(false);

    // Not over any
    if (closestIndex == -1)
        return;

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

bool StructureBuilding::RemoveSelectedObj()
{
    int indexToDelete = -1;

    for (int i = 0; i < objects.size(); i++)
    {
        if (objects[i]->IsSelected())
        {
            indexToDelete = i;
            break;
        }
    }

    // Return early if none selected
    if (indexToDelete == -1)
        return false;

    objects.erase(objects.begin() + indexToDelete);
    return true;
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