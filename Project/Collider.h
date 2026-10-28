#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>

struct Triangle
{
	Vector3 a;
	Vector3 b;
	Vector3 c;

	Vector3 normal;
};

struct CollisionInfo
{
    bool hit = false;
    Vector3 normal = { 0.0f, 0.0f, 0.0f };
    float penetration = 0.0f;
};


class Collider
{
public:
	// virtual bool CheckCollision(Mesh t_mesh, Matrix t_matrix, Vector3& t_fixedPos) = 0;

	virtual void Draw() = 0;

	Vector3 GetPos() const { return position; }
	void SetPos(Vector3 t_newPos) { position = t_newPos; }

protected:
	void ToggleDebug() 
	{
		if (IsKeyReleased(KEY_F1))
			debugMode = !debugMode;
	}

	Vector3 position = { 0.0f, 0.0f, 0.0f };

    // Sources of research and editted code
    // https://blog.bearcats.nl/capsule-triangle-sweep/
    // https://cseweb.ucsd.edu/classes/sp19/cse291-d/Files/CSE291_13_CollisionDetection.pdf
    // http://www.r-5.org/files/books/computers/algo-list/realtime-3d/Christer_Ericson-Real-Time_Collision_Detection-EN.pdf
    Vector3 ClosestPointToTriangle(Vector3 t_point, Vector3 t_triA, Vector3 t_triB, Vector3 t_triC)
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

	// Debug
	bool debugMode = false;
	const float DEBUG_POINT_RADIUS = 0.05f;
};