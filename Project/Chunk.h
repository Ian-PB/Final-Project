#pragma once
#include <raylib.h>
#include <vector>
#include "FastNoiseLite.h"


struct Triangle
{
	Vector3 a;
	Vector3 b;
	Vector3 c;

	Vector3 normal;
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
	float val[8];
};

struct MeshData
{
	int currentVertexCount = 0;
	std::vector<Vector3> vertices;
	std::vector<Vector3> normals;
};

class Chunk
{
public:
	Chunk();
	void Init(FastNoiseLite* t_noise);
	void Draw();
	void Update();

	void ApplyMeshDataToModel();
	void GenerateMeshData(float t_surfaceLevel);
	void UpdateDensities();

	float GetChunkSize() const { return ((SIZE - 1) * pointSpacing); }
	void SetPosition(Vector3 t_pos) { position = t_pos; }

	bool IsDirty() const { return dirty; }
	void SetDirty() { dirty = true; }

	Mesh GetMesh() const { return mesh; }
	Vector3 GetPosition() const { return position; }

	void EditSphere(Vector3 t_pos, float t_radius, bool destroy = true);

private:
	bool dirty = false;

	void SetupPoints();
	std::vector<Triangle> GetMeshVerticesForSection(CubeSection t_cubeSection, float t_surfaceLevel);
	Vector3 VertexInterp(Vector3 p1, Vector3 p2, float valp1, float valp2, float t_surfaceLevel);
	static void SetTriangleNormal(Triangle& tri);

	Color GetColorFromDensity(float val);

	Vector3 GetLocalPos(Vector3 t_index) { return { t_index.x * pointSpacing, t_index.y * pointSpacing, t_index.z * pointSpacing }; }
	Vector3 GetGlobalPos(Vector3 t_index) { return { (t_index.x * pointSpacing) + position.x, (t_index.y * pointSpacing) + position.y, (t_index.z * pointSpacing) + position.z }; }

	MeshData meshData;
	Mesh mesh;
	Model model;
	Model pointModel;

	Vector3 position = { 0.0f, 0.0f, 0.0f };

	static const int SIZE = 20; // Amount of points in each direction
	Point points[SIZE * SIZE * SIZE]; // Amount of points total in the cube

	float pointSpacing = 1.0f;

	// Noise
	FastNoiseLite* noise;

	// Debug
	bool showDebugPoints = false;
};

