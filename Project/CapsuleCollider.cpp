#include "CapsuleCollider.h"
#include <raymath.h>
#include <algorithm>

void CapsuleCollider::Setup(float t_height, float t_radius, Vector3 t_position)
{
	height = t_height;
	radius = t_radius;
	position = t_position;
}

void CapsuleCollider::Draw()
{
    ToggleDebug();

    if (debugMode)
    {
        Vector3 start = position;
        start.y += radius;

        Vector3 end = position;
        end.y += std::max(radius, height - radius);

        DrawCapsuleWires(start, end, radius, 10, 10, GREEN);
        DrawSphere(position, 0.02f, PINK);
    }
}


// Sources of research and editted code
// https://blog.bearcats.nl/capsule-triangle-sweep/
// https://cseweb.ucsd.edu/classes/sp19/cse291-d/Files/CSE291_13_CollisionDetection.pdf
// http://www.r-5.org/files/books/computers/algo-list/realtime-3d/Christer_Ericson-Real-Time_Collision_Detection-EN.pdf
Vector3 CapsuleCollider::ClosestPointToTriangle(Vector3 t_point, Vector3 t_triA, Vector3 t_triB, Vector3 t_triC)
{
    Vector3 ab = Vector3Subtract(t_triB, t_triA);
    Vector3 ac = Vector3Subtract(t_triC, t_triA);
    Vector3 ap = Vector3Subtract(t_point, t_triA);

    float d1 = Vector3DotProduct(ab, ap);
    float d2 = Vector3DotProduct(ac, ap);

    if (d1 <= 0 && d2 <= 0)
        return t_triA;

    Vector3 bp = Vector3Subtract(t_point, t_triB);

    float d3 = Vector3DotProduct(ab, bp);
    float d4 = Vector3DotProduct(ac, bp);

    if (d3 >= 0 && d4 <= d3)
        return t_triB;

    float vc = d1 * d4 - d3 * d2;

    if (vc <= 0 && d1 >= 0 && d3 <= 0)
        return Vector3Add(t_triA, Vector3Scale(ab, d1 / (d1 - d3)));

    Vector3 cp = Vector3Subtract(t_point, t_triC);

    float d5 = Vector3DotProduct(ab, cp);
    float d6 = Vector3DotProduct(ac, cp);

    if (d6 >= 0 && d5 <= d6)
        return t_triC;

    float vb = d5 * d2 - d1 * d6;

    if (vb <= 0 && d2 >= 0 && d6 <= 0)
        return Vector3Add(t_triA, Vector3Scale(ac, d2 / (d2 - d6)));

    float va = d3 * d6 - d5 * d4;

    if (va <= 0 && (d4 - d3) >= 0 && (d5 - d6) >= 0)
    {
        Vector3 bc = Vector3Subtract(t_triC, t_triB);
        float interpolation = (d4 - d3) / ((d4 - d3) + (d5 - d6));

        return Vector3Add(t_triB, Vector3Scale(bc, interpolation));
    }

    float denom = 1.0f / (va + vb + vc);
    float v = vb * denom;
    float w = vc * denom;

    return Vector3Add(t_triA, Vector3Add(Vector3Scale(ab, v), Vector3Scale(ac, w)));
}

// Sources of research and editted code
// https://blog.bearcats.nl/capsule-triangle-sweep/
// https://cseweb.ucsd.edu/classes/sp19/cse291-d/Files/CSE291_13_CollisionDetection.pdf
// http://www.r-5.org/files/books/computers/algo-list/realtime-3d/Christer_Ericson-Real-Time_Collision_Detection-EN.pdf
float CapsuleCollider::ShortestSegmentToSegmentDistSq(Vector3 t_p1, Vector3 t_q1, Vector3 t_p2, Vector3 t_q2, Vector3& t_closest1, Vector3& t_closest2)
{
    const float EPS = 0.000001f;

    Vector3 d1 = Vector3Subtract(t_q1, t_p1);
    Vector3 d2 = Vector3Subtract(t_q2, t_p2);
    Vector3 r = Vector3Subtract(t_p1, t_p2);

    float a = Vector3DotProduct(d1, d1);
    float e = Vector3DotProduct(d2, d2);
    float f = Vector3DotProduct(d2, r);

    float s = 0.0f;
    float t = 0.0f;

    if (a <= EPS && e <= EPS)
    {
        t_closest1 = t_p1;
        t_closest2 = t_p2;
        return Vector3DotProduct(r, r);
    }

    if (a <= EPS)
    {
        t = std::clamp(f / e, 0.0f, 1.0f);
    }
    else
    {
        float c = Vector3DotProduct(d1, r);

        if (e <= EPS)
        {
            s = std::clamp(-c / a, 0.0f, 1.0f);
        }
        else
        {
            float b = Vector3DotProduct(d1, d2);
            float denom = a * e - b * b;

            if (denom > EPS)
                s = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);

            t = (b * s + f) / e;

            if (t < 0.0f)
            {
                t = 0.0f;
                s = std::clamp(-c / a, 0.0f, 1.0f);
            }
            else if (t > 1.0f)
            {
                t = 1.0f;
                s = std::clamp((b - c) / a, 0.0f, 1.0f);
            }
        }
    }

    t_closest1 = Vector3Add(t_p1, Vector3Scale(d1, s));
    t_closest2 = Vector3Add(t_p2, Vector3Scale(d2, t));

    return Vector3DistanceSqr(t_closest1, t_closest2);
}

void CapsuleCollider::TestPoints(Vector3 t_capsulePoint, Vector3 t_trianglePoint, float& t_minDistSq, Vector3& t_closestCapsule, Vector3& t_closestTriangle)
{
    float distSq = Vector3DistanceSqr(t_capsulePoint, t_trianglePoint);

    if (distSq < t_minDistSq)
    {
        t_minDistSq = distSq;
        t_closestCapsule = t_capsulePoint;
        t_closestTriangle = t_trianglePoint;
    }
}

CollisionInfo CapsuleCollider::CapsuleTriangle(Vector3 t_start, Vector3 t_end, const Triangle& tri)
{
    CollisionInfo collisionInfo;
    float radiusSq = radius * radius;
    float minDistSq = FLT_MAX;

    Vector3 closestCapsulePoint = t_start;
    Vector3 closestTrianglePoint = tri.a;


    // Check capsule start point against triangle
    TestPoints(t_start, ClosestPointToTriangle(t_start, tri.a, tri.b, tri.c), minDistSq, closestCapsulePoint, closestTrianglePoint);

    // Check capsule end point against triangle
    TestPoints(t_end, ClosestPointToTriangle(t_end, tri.a, tri.b, tri.c), minDistSq, closestCapsulePoint, closestTrianglePoint);

    // Check capsule line segment against triangle edges (lines)
    Vector3 c1 = { 0.0f, 0.0f, 0.0f };
    Vector3 c2 = { 0.0f, 0.0f, 0.0f };

    ShortestSegmentToSegmentDistSq(t_start, t_end, tri.a, tri.b, c1, c2);
    TestPoints(c1, c2, minDistSq, closestCapsulePoint, closestTrianglePoint);

    ShortestSegmentToSegmentDistSq(t_start, t_end, tri.b, tri.c, c1, c2);
    TestPoints(c1, c2, minDistSq, closestCapsulePoint, closestTrianglePoint);

    ShortestSegmentToSegmentDistSq(t_start, t_end, tri.c, tri.a, c1, c2);
    TestPoints(c1, c2, minDistSq, closestCapsulePoint, closestTrianglePoint);

    // Check where the capsule segment crosses the triangle
    Vector3 ab = Vector3Subtract(tri.b, tri.a);
    Vector3 ac = Vector3Subtract(tri.c, tri.a);

    float normalLength = Vector3Length(tri.normal);
    
    if (normalLength > 0.000001f)
    {
        Vector3 scaledTriNormal = Vector3Scale(tri.normal, 1.0f / normalLength);
        float d0 = Vector3DotProduct(Vector3Subtract(t_start, tri.a), scaledTriNormal);
        float d1 = Vector3DotProduct(Vector3Subtract(t_end, tri.a), scaledTriNormal);

        float denom = d0 - d1;

        if (fabsf(denom) > 0.000001f)
        {
            float t = d0 / denom;

            if (t >= 0.0f && t <= 1.0f)
            {
                Vector3 crossing = Vector3Lerp(t_start, t_end, t);

                Vector3 pointOnTriangle = ClosestPointToTriangle(crossing, tri.a, tri.b, tri.c);

                TestPoints(crossing, pointOnTriangle, minDistSq, closestCapsulePoint, closestTrianglePoint);
            }
        }
    }

    // Check if no collision
    if (minDistSq >= radiusSq)
        return collisionInfo;

    collisionInfo.hit = true;

    // Calculate penetration
    float dist = sqrtf(minDistSq);
    collisionInfo.penetration = radius - dist;

    // Get collision normal
    if (dist > 0.0f)
    {
        Vector3 triToCapsule = Vector3Subtract(closestCapsulePoint, closestTrianglePoint);
        collisionInfo.normal = Vector3Scale(triToCapsule, 1.0f / dist);
    }
    else
    {
        collisionInfo.normal = tri.normal;

        // Check if basicly 0
        if (Vector3LengthSqr(collisionInfo.normal) < 0.000001f)
            collisionInfo.normal = { 0.0f, 1.0f, 0.0f }; // Default to upwards
    }


    return collisionInfo;
}

CollisionInfo CapsuleCollider::CheckAgainstTriangles(const std::vector<Triangle>& t_triangles)
{
    CollisionInfo deepestCollision;
    // Position represents bottom of capsule
    Vector3 start = position;
    start.y += radius;

    Vector3 end = position;
    end.y += std::max(radius, height - radius);

    for (const Triangle& tri : t_triangles)
    {
        CollisionInfo collisionInfo = CapsuleTriangle(start, end, tri);

        if (collisionInfo.hit && (collisionInfo.penetration > deepestCollision.penetration))
        {
            deepestCollision = collisionInfo;
        }
    }

    return deepestCollision;
}
