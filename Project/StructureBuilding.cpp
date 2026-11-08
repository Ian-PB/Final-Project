#include "StructureBuilding.h"
#include <raylib.h>
#include <raymath.h>
#include <raygui.h>
#include <iostream>

#include "StructureData.h"
#include "StructurePaster.h"

#include "SphereBuildingBlock.h"
#include "CylinderBuildingBlock.h"
#include "Materials.h"

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

    structureSaver = std::make_shared<StructureSaver>(world);
}

void StructureBuilding::Draw()
{
	BeginMode3D(camera);

        // Ground
        DrawPlane({ worldCenter.x, 0.0f, worldCenter.z }, { worldSize.x, worldSize.z }, RAYWHITE);
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
        if (displayObjs)
        {
            DrawText("B - Bake Objects into a mesh", GetScreenWidth() * 0.01f, GetScreenHeight() * 0.01f, 25, RED);
            DrawText("T - Manually Edit Selected Object", GetScreenWidth() * 0.01f, GetScreenHeight() * 0.01f + 30, 25, RED);
            DrawText("N - Spawn New Object", GetScreenWidth() * 0.01f, GetScreenHeight() * 0.01f + 60, 25, RED);
            if (selectedObject != nullptr)
            {
                DrawText("Q - Change Selected Objects Material", GetScreenWidth() * 0.01f, GetScreenHeight() * 0.01f + 90, 25, RED);
                DrawText("DELETE - Remove Selected Object", GetScreenWidth() * 0.01f, GetScreenHeight() * 0.01f + 120, 25, RED);
            }
        }
        else
        {
            DrawText("B - Undo Baking", GetScreenWidth() * 0.01f, GetScreenHeight() * 0.01f, 25, RED);
            DrawText("Enter - Save Mesh", GetScreenWidth() * 0.01f, GetScreenHeight() * 0.01f + 30, 25, RED);
        }
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
        else if (currentUiMode == UiModes::SpawnObj)
        {
            currentUiMode = UiModes::Off;
            DisableCursor();
        }
    }

    if (IsKeyPressed(KEY_Q) && selectedObject != nullptr)
    {
        if (currentUiMode == UiModes::Off)
        {
            materialInt = 0;
            currentUiMode = UiModes::SetSelectedMaterial;
            EnableCursor();
        }
        else if (currentUiMode == UiModes::SetSelectedMaterial)
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
        else if (currentUiMode == UiModes::SelectedEditting)
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

    // Bake objects into the a mesh
    if (IsKeyPressed(KEY_B) && currentUiMode == UiModes::Off)
    {
        if (displayObjs)
            Bake();
        else
            ResetBaking();
    }

    if (IsKeyPressed(KEY_O))
    {
        // Try spawn test structure
        StructureExport testStruct = StructurePaster::GetStructureFromFile(StructureTypes::None, "Name");
        StructurePaster::PlaceStructure(world, testStruct, camera.position);

        // Rebuild all chunks (Temp)
        for (Chunk* chunk : world.GetAllChunks())
        {
            chunk->SetDirty();
            world.QueueChunkChangeAtPos(chunk->GetPosition());
        }
    }

    if (IsKeyPressed(KEY_ENTER) && !displayObjs)
    {
        if (currentUiMode == UiModes::Off)
        {
            currentUiMode = UiModes::SavingStructure;
            EnableCursor();
        }
        else if (currentUiMode == UiModes::SavingStructure)
        {
            currentUiMode = UiModes::Off;
            DisableCursor();
        }
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
    case UiModes::SavingStructure:
        DrawSavingStructurePanel();
        break;
    case UiModes::SetSelectedMaterial:
        DrawSelectMaterialPanel();
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

void StructureBuilding::DrawSavingStructurePanel()
{
    const Rectangle PANEL_RECT = { GetScreenWidth() * 0.02f, GetScreenHeight() * 0.02f, GetScreenWidth() * 0.2f, GetScreenHeight() * 0.4f };
    DrawRectangleRounded(PANEL_RECT, 0.2, 10, LIGHTGRAY);
    DrawRectangleRoundedLines(PANEL_RECT, 0.2, 10, DARKGRAY);

    // Element Setup
    Rectangle elementRect;
    elementRect.width = PANEL_RECT.width * 0.7f;
    elementRect.height = PANEL_RECT.height * 0.07f;
    const float ELEMENT_X_START = PANEL_RECT.x + PANEL_RECT.width * 0.15f;
    elementRect.x = ELEMENT_X_START;
    elementRect.y = PANEL_RECT.y + PANEL_RECT.height * 0.1f;

    if (GuiTextBox(elementRect, structureName, MAX_NAME_SIZE, nameEditMode))
        nameEditMode = !nameEditMode;

    // Spacing
    elementRect.x = ELEMENT_X_START;
    elementRect.y += elementRect.width * 0.2f;

    int selectedTypeInt = static_cast<int>(selectedType) + 1;
    if (GuiDropdownBox(elementRect, "None;World;", &selectedTypeInt, typeDropdownOpen))
        typeDropdownOpen = !typeDropdownOpen;
    selectedType = static_cast<StructureTypes>(selectedTypeInt - 1); // Convert back to enum

    // Spacing
    elementRect.x = ELEMENT_X_START;
    elementRect.y = PANEL_RECT.height * 0.9f;

    if (GuiButton(elementRect, "Save To File"))
    {
        structureSaver->SaveFile(selectedType, structureName, structureBounds);
    }
}

void StructureBuilding::DrawSelectMaterialPanel()
{
    const Rectangle PANEL_RECT = { GetScreenWidth() * 0.02f, GetScreenHeight() * 0.02f, GetScreenWidth() * 0.2f, GetScreenHeight() * 0.4f };
    DrawRectangleRounded(PANEL_RECT, 0.2, 10, LIGHTGRAY);
    DrawRectangleRoundedLines(PANEL_RECT, 0.2, 10, DARKGRAY);

    // Element Setup
    Rectangle elementRect;
    elementRect.width = PANEL_RECT.width * 0.7f;
    elementRect.height = PANEL_RECT.height * 0.07f;
    const float ELEMENT_X_START = PANEL_RECT.x + PANEL_RECT.width * 0.15f;
    elementRect.x = ELEMENT_X_START;
    elementRect.y = PANEL_RECT.y + PANEL_RECT.height * 0.1f;

    MaterialTypes oldMaterial = materialSelected;
    if (GuiDropdownBox(elementRect, "Stone;Grass;Gold", &materialInt, materialDropdownOpen))
        materialDropdownOpen = !materialDropdownOpen;
    materialSelected = static_cast<MaterialTypes>(materialInt); // Convert back to enum

    if (oldMaterial != materialSelected)
    {
        Color materialColor = Materials::MATERIAL_TO_COLOR.at(materialSelected);
        selectedObject->SetColor(materialColor);
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
            for (int i = 0; i < chunk->GetPointAmount(); i++)
            {
                Point& point = chunk->GetPointAt(i);

                Vector3 worldPointPos = Vector3Add(point.position, chunk->GetPosition());

                // Change densities
                if (obj->ChangePointDensity(worldPointPos, point.density))
                {
                    // Check for errors
                    if (point.color.r == RAYWHITE.r && point.color.g == RAYWHITE.g && point.color.b == RAYWHITE.b)
                        point.color = obj->GetColor();
                    // Set point color (Don't override more valuable materials)
                    else if (Materials::COLOR_TO_MATERIAL.at(obj->GetColor()) >= Materials::COLOR_TO_MATERIAL.at(point.color))
                    {
                        point.color = obj->GetColor();
                    }

                    anyModificationsMade = true;
                }
            }
        }
    }

    // Only stop the display of objects if there was a single edit made
    if (anyModificationsMade)
    {
        isDragging = false;
        selectedObject = nullptr;
        for (std::shared_ptr<BaseBuildingBlock> obj : objects) // Reset all
        {
            obj->SetSelected(false);
            obj->SetDragging(false);
            obj->ResetHoveredAxis();
        }

        displayObjs = false;

        structureBounds = FindStructureBounds();

        for (Chunk* chunk : world.GetChunksInBounds(structureBounds))
        {
            chunk->SetDirty();
            world.QueueChunkChangeAtPos(chunk->GetPosition());
        }
    }
}

void StructureBuilding::ResetBaking()
{
    world.EmptyWorldGen();
    displayObjs = true;
}

Index3D StructureBuilding::Get3DIndex(int i, int axisSize)
{
    Index3D index3D;
    index3D.x = i / (axisSize * axisSize);
    index3D.y = (i / axisSize) % axisSize;
    index3D.z = i % axisSize;

    return index3D;
}

BoundingBox StructureBuilding::FindStructureBounds()
{
    // Same across all chunks
    int pointsPerAxis = world.GetChunk(0, 0, 0).GetPointsPerAxis();
    BoundingBox bounds = {};
    bool foundPoint = false;

    for (Chunk* chunk : world.GetAllChunks())
    {
        // Loop through chunks points
        for (int i = 0; i < chunk->GetPointAmount(); i++)
        {
            const Point& p = chunk->GetPointAt(i);

            // Ignore empty points
            if (p.density >= 1.0f)
                continue;

            Index3D i3D = Get3DIndex(i, pointsPerAxis);
            Vector3 globalPos = chunk->GetGlobalPos({ (float)i3D.x, (float)i3D.y, (float)i3D.z });

            if (!foundPoint)
            {
                bounds.min = globalPos;
                bounds.max = globalPos;
                foundPoint = true;
                continue;
            }

            // Change each axis indepentantly
            bounds.min.x = std::min(globalPos.x, bounds.min.x);
            bounds.min.y = std::min(globalPos.y, bounds.min.y);
            bounds.min.z = std::min(globalPos.z, bounds.min.z);

            bounds.max.x = std::max(globalPos.x, bounds.max.x);
            bounds.max.y = std::max(globalPos.y, bounds.max.y);
            bounds.max.z = std::max(globalPos.z, bounds.max.z);
        }
    }

    return bounds;
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
    {
        selectedObject->SetSelected(false);
        selectedObject = nullptr;
    }

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