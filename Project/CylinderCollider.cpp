#include "CylinderCollider.h"
#include <iostream>
#include <raymath.h>

bool CylinderCollider::CheckCollision(Mesh& t_mesh, Vector3& t_fixedPos)
{
    return false;
}

void CylinderCollider::Draw()
{
    if (IsKeyReleased(KEY_F1))
        debugMode = !debugMode;

    if (!debugMode)
        return;

    DrawCylinderWires(position, radius, radius, height, slices, GREEN);
    DrawSphere(position, DEBUG_POINT_RADIUS, PINK);
    DrawSphere({position.x, position.y + height, position.z}, DEBUG_POINT_RADIUS, PINK);

    // Draw points on sphere
    for (int i = 0; i < circleDivisionOffsets.size(); i++)
    {
        Vector3 pointPos = Vector3Add(position, circleDivisionOffsets[i]);
        DrawSphere(pointPos, DEBUG_POINT_RADIUS, BLUE);
    }

    for (int i = 0; i < heightDivisionOffsets.size(); i++)
    {
        Vector3 pointPos = Vector3Add(position, heightDivisionOffsets[i]);
        DrawSphere(pointPos, DEBUG_POINT_RADIUS, RED);
    }
}

void CylinderCollider::Setup(float t_h, float t_r, int t_slices)
{
    height = t_h;
    radius = t_r;
    slices = t_slices;

    circleDivisionOffsets.clear();
    heightDivisionOffsets.clear();

    // Make sure slices are at least greater than 3 to make a square;
    if (slices < 3)
    {
        std::cout << "Collider's slices not valid" << std::endl;
        return;
    }

    // Make top / bottom division offsets
    for (int h = 0; h < 2; h++) // h
    {
        Vector3 offset = { 0.0f, 0.0f, 0.0f };
        // Get the Y offset for both ends
        if (h == 0)
            offset.y = height;

        // Get the XZ offset
        for (int i = 0; i < slices; i++) // XZ
        {
            float angle = (2.0f * PI * i) / slices;

            offset.x = cosf(angle) * radius;
            offset.z = sinf(angle) * radius;

            circleDivisionOffsets.push_back(offset);
        }
    }


    // Make height offsets
    for (int i = 1; i < slices; i++)
    {
        float y = (height * i) / slices;

        // Only go through half since if not it will make doubles of each point
        for (int endPoint = 0; endPoint < circleDivisionOffsets.size() / 2; endPoint++)
        {
            heightDivisionOffsets.push_back({ circleDivisionOffsets[endPoint].x, y, circleDivisionOffsets[endPoint].z });
        }
    }
}
