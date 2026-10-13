#include "CylinderCollider.h"
#include <iostream>
#include <raymath.h>

const Vector3 CylinderCollider::RAY_UP_DIR = { 0.0f, 1.0f, 0.0f };
const Vector3 CylinderCollider::RAY_DOWN_DIR = { 0.0f, -1.0f, 0.0f };
const float CylinderCollider::MAX_COLLISION_DISTANCE = 0.1f;

bool CylinderCollider::CheckCollision(Mesh t_mesh, Matrix t_matrix, Vector3& t_fixedPos)
{
    GetRays();

    for (int i = 0; i < rays.size(); i++)
    {
        RayCollision rayCollison = GetRayCollisionMesh(rays[i], t_mesh, t_matrix);

        if (rayCollison.hit && rayCollison.distance <= MAX_COLLISION_DISTANCE)
        {
            return true;
        }
    }

    return false;
}

void CylinderCollider::Draw()
{
    if (IsKeyReleased(KEY_F1))
        debugMode = !debugMode;

    if (debugMode)
        DrawDebug();
}

void CylinderCollider::Setup(float t_h, float t_r, int t_slices)
{
    height = t_h;
    radius = t_r;
    slices = t_slices;

    bottomCircleDivisionOffsets.clear();
    topCircleDivisionOffsets.clear();
    heightDivisionOffsets.clear();
    directionsAway.clear();

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

            if (h == 0)
                topCircleDivisionOffsets.push_back(offset);
            else
                bottomCircleDivisionOffsets.push_back(offset);

            // This is to stop duplicates on the directions away
            if (h != 0)
                continue;

            // Directions away
            Vector3 dir = Vector3Normalize({ offset.x, 0.0f, offset.z });
            directionsAway.push_back(dir);
        }
    }


    // Make height offsets
    for (int i = 1; i < slices; i++)
    {
        float y = (height * i) / slices;

        // Only go through half since if not it will make doubles of each point
        for (int endPoint = 0; endPoint < topCircleDivisionOffsets.size(); endPoint++)
        {
            heightDivisionOffsets.push_back({ topCircleDivisionOffsets[endPoint].x, y, topCircleDivisionOffsets[endPoint].z });
        }
    }
}

void CylinderCollider::DrawDebug()
{
    // Wires
    DrawCylinderWires(position, radius, radius, height, slices, GREEN);

    // Draw points on top circle
    DrawPoints();

    // Draw rays
    DrawRays();
}

void CylinderCollider::DrawPoints()
{
    // Draw points on top and bottom
    DrawSphere(position, DEBUG_POINT_RADIUS, PINK);
    DrawSphere({ position.x, position.y + height, position.z }, DEBUG_POINT_RADIUS, PINK);

    for (int i = 0; i < topCircleDivisionOffsets.size(); i++)
    {
        Vector3 pointPos = Vector3Add(position, topCircleDivisionOffsets[i]);
        DrawSphere(pointPos, DEBUG_POINT_RADIUS, BLUE);
    }
    // Draw points on bottom circle
    for (int i = 0; i < bottomCircleDivisionOffsets.size(); i++)
    {
        Vector3 pointPos = Vector3Add(position, bottomCircleDivisionOffsets[i]);
        DrawSphere(pointPos, DEBUG_POINT_RADIUS, BLUE);
    }
    // Draw points along height
    for (int i = 0; i < heightDivisionOffsets.size(); i++)
    {
        Vector3 pointPos = Vector3Add(position, heightDivisionOffsets[i]);
        DrawSphere(pointPos, DEBUG_POINT_RADIUS, RED);
    }
}

void CylinderCollider::DrawRays()
{
    // Top ring
    for (int i = 0; i < topCircleDivisionOffsets.size(); i++)
    {
        Ray ray;
        ray.position = Vector3Add(position, topCircleDivisionOffsets[i]);
        ray.direction = RAY_UP_DIR;
        DrawRay(ray, BLUE);
    }
    // Bottom ring
    for (int i = 0; i < bottomCircleDivisionOffsets.size(); i++)
    {
        Ray ray;
        ray.position = Vector3Add(position, bottomCircleDivisionOffsets[i]);
        ray.direction = RAY_DOWN_DIR;
        DrawRay(ray, BLUE);
    }
    // Along height
    for (int i = 0; i < heightDivisionOffsets.size(); i++)
    {
        Ray ray;
        ray.position = Vector3Add(position, heightDivisionOffsets[i]);
        int dirIndex = (int)(i % slices);
        ray.direction = directionsAway[dirIndex];
        DrawRay(ray, RED);
    }
}

void CylinderCollider::GetRays()
{
    rays.clear();

    // Check collisions upwards
    for (int i = 0; i < topCircleDivisionOffsets.size(); i++)
    {
        Ray ray;
        ray.position = Vector3Add(position, topCircleDivisionOffsets[i]);
        ray.direction = RAY_UP_DIR;

        rays.push_back(ray);
    }
    // Check collisions downwards
    for (int i = 0; i < bottomCircleDivisionOffsets.size(); i++)
    {
        Ray ray;
        ray.position = Vector3Add(position, bottomCircleDivisionOffsets[i]);
        ray.direction = RAY_DOWN_DIR;

        rays.push_back(ray);
    }
    // Along height
    for (int i = 0; i < heightDivisionOffsets.size(); i++)
    {
        Ray ray;
        ray.position = Vector3Add(position, heightDivisionOffsets[i]);
        int dirIndex = (int)(i % slices);
        ray.direction = directionsAway[dirIndex];

        rays.push_back(ray);
    }
}
