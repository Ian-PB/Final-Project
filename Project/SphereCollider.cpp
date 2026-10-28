#include "SphereCollider.h"

void SphereCollider::Setup(float t_radius, Vector3 t_position)
{
    radius = t_radius; 
    position = t_position;
}

void SphereCollider::Draw()
{
    ToggleDebug();

    if (debugMode)
    {
        DrawSphereWires(position, radius, 5, 5, GREEN);
    }
}

CollisionInfo SphereCollider::CheckAgainstTriangles(const std::vector<Triangle>& t_triangles)
{
    CollisionInfo deepestCollision;

    for (const Triangle& tri : t_triangles)
    {
        CollisionInfo collisionInfo = SphereTriangle(tri);

        if (collisionInfo.hit && (collisionInfo.penetration > deepestCollision.penetration))
        {
            deepestCollision = collisionInfo;
        }
    }

    return deepestCollision;
}

CollisionInfo SphereCollider::SphereTriangle(const Triangle& tri)
{
    CollisionInfo collisionInfo;

    Vector3 closestPoint = ClosestPointToTriangle(position, tri.a, tri.b, tri.c);
    float dist = Vector3Distance(position, closestPoint);

    // No collision
    if (dist > radius)
        return collisionInfo;

    collisionInfo.hit = true;
    collisionInfo.penetration = radius - dist;

    // Get collision normal
    if (dist > 0.0f)
    {
        Vector3 triToCapsule = Vector3Subtract(position, closestPoint);
        collisionInfo.normal = Vector3Scale(triToCapsule, 1.0f / dist);
    }
    else
    {
        collisionInfo.normal = Vector3Normalize(tri.normal);

        if (Vector3LengthSqr(collisionInfo.normal) < 0.000001f)
            collisionInfo.normal = { 0.0f, 1.0f, 0.0f };

        // Check if basicly 0
        if (Vector3LengthSqr(collisionInfo.normal) < 0.000001f)
            collisionInfo.normal = { 0.0f, 1.0f, 0.0f }; // Default to upwards
    }

    return collisionInfo;
}
