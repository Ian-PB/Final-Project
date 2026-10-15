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
        Vector3 end = position;
        end.y += height;
        DrawCapsuleWires(position, end, radius, 10, 10, GREEN);
        DrawSphere(position, 0.02f, PINK);
    }
}

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

float CapsuleCollider::ShortestSegmentToSegmentDistSq(Vector3 p1, Vector3 q1, Vector3 p2, Vector3 q2)
{
    const float EPS = 0.000001f;

    Vector3 d1 = Vector3Subtract(q1, p1);
    Vector3 d2 = Vector3Subtract(q2, p2);
    Vector3 r = Vector3Subtract(p1, p2);

    float a = Vector3DotProduct(d1, d1);
    float e = Vector3DotProduct(d2, d2);
    float f = Vector3DotProduct(d2, r);

    float s = 0.0f;
    float t = 0.0f;

    if (a <= EPS && e <= EPS)
        return Vector3DotProduct(r, r);

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

    Vector3 c1 = Vector3Add(p1, Vector3Scale(d1, s));
    Vector3 c2 = Vector3Add(p2, Vector3Scale(d2, t));

    return Vector3DistanceSqr(c1, c2);
}

bool CapsuleCollider::CapsuleTriangle(Vector3 t_start, Vector3 t_end, const Triangle& tri)
{
    float radiusSq = radius * radius;

    // Check if segment intersects triangle
    Vector3 direction = Vector3Subtract(t_end, t_start);
    float length = Vector3Length(direction);

    if (length > 0.000001f)
    {
        Ray ray;
        ray.position = t_start;
        ray.direction = Vector3Scale(direction, 1.0f / length);

        RayCollision hit = GetRayCollisionTriangle(ray, tri.a, tri.b, tri.c);

        if (hit.hit && hit.distance <= length)
            return true;
    }

    // Check capsule endpoints against triangle face
    Vector3 p1 = ClosestPointToTriangle(t_start, tri.a, tri.b, tri.c);

    Vector3 p2 = ClosestPointToTriangle(t_end, tri.a, tri.b, tri.c);

    if (Vector3DistanceSqr(t_start, p1) <= radiusSq || Vector3DistanceSqr(t_end, p2) <= radiusSq)
        return true;

    // Check capsule segment against triangle edges
    if (ShortestSegmentToSegmentDistSq(t_start, t_end, tri.a, tri.b) <= radiusSq)
        return true;

    if (ShortestSegmentToSegmentDistSq(t_start, t_end, tri.b, tri.c) <= radiusSq)
        return true;

    if (ShortestSegmentToSegmentDistSq(t_start, t_end, tri.c, tri.a) <= radiusSq)
        return true;

    return false;
}

bool CapsuleCollider::CheckAgainstTriangles(const std::vector<Triangle>& t_triangles)
{
    // Position represents bottom of capsule
    Vector3 start = position;
    start.y += radius;

    Vector3 end = position;
    end.y += std::max(radius, height - radius);

    for (const Triangle& tri : t_triangles)
    {
        if (CapsuleTriangle(start, end, tri))
            return true;
    }

    return false;
}
