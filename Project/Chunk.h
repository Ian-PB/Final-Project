#pragma once
#include <raylib.h>
#include <vector>
#include "FastNoiseLite.h"


struct Triangle
{
	Vector3 a;
	Vector3 b;
	Vector3 c;
};
struct Point
{

	Color color = RED;

	Vector3 position = { 0.0f, 0.0f, 0.0f };
	float density = 0.0f;
};

struct CubeSection
{
	Vector3 p[8];
	double val[8];
};

class Chunk
{
public:
	Chunk();
	void Init();
	void Draw();
	void Update();

	void GenerateMesh();

private:
	void SetupPoints();
	std::vector<Triangle> GetMeshVerticesForSection(CubeSection t_cubeSection);
	Vector3 VertexInterp(Vector3 p1, Vector3 p2, float valp1, float valp2);

	Color GetColorFromDensity(float val);
	float surfaceLevel = 0.0f;

	Mesh mesh;
	Model model;
	Model pointModel;

	Vector3 position = { 0.0f, 1.5f, 0.0f };

	static const int SIZE = 50; // Amount of points in each direction
	Point points[SIZE * SIZE * SIZE]; // Amount of points total in the cube

	float pointSpacing = 1.5f;

	// Noise
	FastNoiseLite noise;

	// Debug
	bool showDebugPoints = false;
};

