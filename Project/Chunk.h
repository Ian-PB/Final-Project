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

	int GetChunkSize() { return ((SIZE - 1) * pointSpacing); }
	void SetPosition(Vector3 t_pos) { position = t_pos; }

private:
	void SetupPoints();
	void UpdateDensities();
	std::vector<Triangle> GetMeshVerticesForSection(CubeSection t_cubeSection);
	Vector3 VertexInterp(Vector3 p1, Vector3 p2, float valp1, float valp2);

	Color GetColorFromDensity(float val);

	Vector3 GetLocalPos(Vector3 t_index) { return { t_index.x * pointSpacing, t_index.y * pointSpacing, t_index.z * pointSpacing }; }
	Vector3 GetGlobalPos(Vector3 t_index) { return { (t_index.x * pointSpacing) + position.x, (t_index.y * pointSpacing) + position.y, (t_index.z * pointSpacing) + position.z }; }
	float surfaceLevel = 0.0f;

	Mesh mesh;
	Model model;
	Model pointModel;

	Vector3 position = { 0.0f, 0.0f, 0.0f };

	static const int SIZE = 10; // Amount of points in each direction
	Point points[SIZE * SIZE * SIZE]; // Amount of points total in the cube

	float pointSpacing = 1.5f;

	// Noise
	FastNoiseLite noise;
	float frequency = 0.1f;
	float scrollX = 0.0f;
	static int NOISE_SEED;

	// Debug
	bool showDebugPoints = false;
};

