#pragma once
#include <raylib.h>
#include <vector>
#include "FastNoiseLite.h"
#include "MeshCollider.h"

struct Point
{
	Color color = RED;

	Vector3 position = { 0.0f, 0.0f, 0.0f };
	float density = 1.0f; // -1 is solid while 1 is empty
};

struct CubeSection
{
	Vector3 p[8];
	float val[8];
	Color colors[8];
};

struct MeshData
{
	int currentVertexCount = 0;
	std::vector<Vector3> vertices;
	std::vector<Vector3> normals;

	std::vector<Color> colors;
};

struct WallFlags {
	bool posXWall = false;
	bool negXWall = false;
	bool posYWall = false;
	bool negYWall = false;
	bool posZWall = false;
	bool negZWall = false;
};

class Chunk
{
public:
	Chunk();
	void Init(FastNoiseLite* t_noise, Shader* t_shader);
	void InitCollider();
	void Draw();
	void Update();

	void ApplyMeshDataToModel();
	void GenerateMeshData(float t_surfaceLevel);
	float surfaceLevel = 0.0f;
	void UpdateDensities();

	float GetChunkSize() const { return ((SIZE - 1) * pointSpacing); }
	float GetPointSpacing() const { return pointSpacing; }
	void SetPosition(Vector3 t_pos) { position = t_pos; }

	bool IsDirty() const { return dirty; }
	void SetDirty() { dirty = true; }

	Mesh GetMesh() const { return mesh; }
	Vector3 GetPosition() const { return position; }
	BoundingBox GetBounds();

	MeshCollider GetCollider() const { return collider; }

	void EditSphere(Vector3 t_pos, float t_radius, bool destroy = true);

	void ClearChunk(WallFlags walls = {});

	int GetPointAmount() const { return SIZE * SIZE * SIZE; }
	int GetPointsPerAxis() const { return SIZE; }
	Point& GetPointAt(int i) { return points[i]; }
	
	bool TryGetPointAtPos(Vector3 t_pos, Point*& t_point);
	Vector3 GetGlobalPos(Vector3 t_index) { return { (t_index.x * pointSpacing) + position.x, (t_index.y * pointSpacing) + position.y, (t_index.z * pointSpacing) + position.z }; }

	int IndexFrom3D(int x, int y, int z) { return (x * SIZE * SIZE + y * SIZE + z); }

private:
	bool dirty = false;

	void SetupPoints();
	void GetMeshVerticesForSection(CubeSection t_cubeSection);
	Vector3 VertexInterp(Vector3 p1, Vector3 p2, float valp1, float valp2);
	Color ColorInterp(Color p1, Color p2, float valp1, float valp2);
	static void SetTriangleNormal(Triangle& tri);

	Color GetColorFromDensity(float t_density);

	Vector3 GetLocalPos(Vector3 t_index) { return { t_index.x * pointSpacing, t_index.y * pointSpacing, t_index.z * pointSpacing }; }

	MeshData meshData;
	Mesh mesh;
	std::vector<Triangle> triangles;
	Model model;
	Model pointModel;

	Vector3 position = { 0.0f, 0.0f, 0.0f };

	static const int SIZE = 20; // Amount of points in each direction
	Point points[SIZE * SIZE * SIZE]; // Amount of points total in the cube

	float pointSpacing = 1.0f;
	const float EDIT_MARGIN = 1.5f;

	// Collision
	MeshCollider collider;

	// Noise
	FastNoiseLite* noise;

	// Shaders
	Shader* shader;
};

