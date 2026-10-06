#include "Chunk.h"
#include <raymath.h>
#include <iostream>

#include "MarchingTables.h"

Chunk::Chunk()
{

}

void Chunk::Init(FastNoiseLite* t_noise)
{
	noise = t_noise;
	Mesh sphereMesh = GenMeshSphere(0.1f, 5, 5);
	pointModel = LoadModelFromMesh(sphereMesh);

	SetupPoints();
}

void Chunk::Draw()
{
	// Draw points
	if (showDebugPoints)
		for (int x = 0; x < SIZE; x++)
		{
			for (int y = 0; y < SIZE; y++)
			{
				for (int z = 0; z < SIZE; z++)
				{
					int i = x * SIZE * SIZE + y * SIZE + z;

					Vector3 realPos = Vector3Add(position, points[i].position);

					DrawModel(pointModel, realPos, 1.0f, points[i].color);
				}
			}
		}

	DrawModel(model, position, 1.0f, GRAY);
	DrawModelWires(model, position, 1.0f, LIGHTGRAY);
}

void Chunk::Update()
{
	if (IsKeyReleased(KEY_P))
		showDebugPoints = !showDebugPoints;
}

void Chunk::ApplyMeshDataToModel()
{
	// Unload old model
	UnloadModel(model);

	// Create new mesh with the new vertices
	mesh = Mesh();
	mesh.vertexCount = meshData.currentVertexCount;
	mesh.triangleCount = meshData.currentVertexCount / 3;

	
	mesh.vertices = (float*)MemAlloc(meshData.currentVertexCount * 3 * sizeof(float));
	mesh.normals = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));

	// Apply all info to mesh
	for (int i = 0; i < mesh.vertexCount; i++)
	{
		// Vertices
		mesh.vertices[i * 3 + 0] = meshData.vertices[i].x;
		mesh.vertices[i * 3 + 1] = meshData.vertices[i].y;
		mesh.vertices[i * 3 + 2] = meshData.vertices[i].z;

		// Normals
		mesh.normals[i * 3 + 0] = meshData.normals[i].x;
		mesh.normals[i * 3 + 1] = meshData.normals[i].y;
		mesh.normals[i * 3 + 2] = meshData.normals[i].z;
	}

	// Upload Mesh and apply to Model
	UploadMesh(&mesh, true);
	model = LoadModelFromMesh(mesh);

	// No longer needs a change
	dirty = false;
}

void Chunk::GenerateMeshData(float t_surfaceLevel)
{
	meshData.vertices.clear();
	meshData.normals.clear();

	for (int x = 0; x < SIZE - 1; x++)
	{
		for (int y = 0; y < SIZE - 1; y++)
		{
			for (int z = 0; z < SIZE - 1; z++)
			{
				CubeSection cubeSection;

				// Get coordinates for each corner
				int coords[8][3] =
				{
					{ x, y, z },				// 0
					{ x + 1, y, z },			// 1
					{ x + 1, y, z + 1 },		// 2
					{ x, y, z + 1 },			// 3
					{ x, y + 1, z },			// 4
					{ x + 1, y + 1, z },		// 5
					{ x + 1, y + 1, z + 1 },	// 6
					{ x, y + 1, z + 1 }			// 7
				};

				// Apply coords and densities to cubeSection
				for (int i = 0; i < 8; i++)
				{
					int px = coords[i][0];
					int py = coords[i][1];
					int pz = coords[i][2];

					int index = px * SIZE * SIZE + py * SIZE + pz;
					cubeSection.p[i] = points[index].position;
					cubeSection.val[i] = points[index].density;
				}

				std::vector<Triangle> triangles = GetMeshVerticesForSection(cubeSection, t_surfaceLevel);

				for (const Triangle& tri : triangles)
				{
					meshData.vertices.push_back(tri.a);
					meshData.vertices.push_back(tri.b);
					meshData.vertices.push_back(tri.c);

					// All vertices get same normal (flat shading)
					meshData.normals.push_back(tri.normal);
					meshData.normals.push_back(tri.normal);
					meshData.normals.push_back(tri.normal);
				}
			}
		}
	}

	meshData.currentVertexCount = (int)meshData.vertices.size();

	dirty = true;
}

bool Chunk::TrySetDirty()
{
	bool expected = false;
	// If dirty is false set to true (atomicly)
	return dirty.compare_exchange_strong(expected, true);
}

void Chunk::EditSphere(Vector3 t_pos, float t_radius, bool destroy)
{
	for (int x = 0; x < SIZE; x++)
	{
		for (int y = 0; y < SIZE; y++)
		{
			for (int z = 0; z < SIZE; z++)
			{
				int i = x * SIZE * SIZE + y * SIZE + z;

				Vector3 pointWorldPos = Vector3Add(position, points[i].position);
				if (Vector3Distance(pointWorldPos, t_pos) <= t_radius)
				{
					if (destroy)
						points[i].density = 1.0f;
					else
						points[i].density = -1.0f;
				}
			}
		}
	}
}

void Chunk::SetupPoints()
{
	for (int x = 0; x < SIZE; x++)
	{
		for (int y = 0; y < SIZE; y++)
		{
			for (int z = 0; z < SIZE; z++)
			{
				int i = x * SIZE * SIZE + y * SIZE + z;

				points[i].position = GetLocalPos({(float)x, (float)y, (float)z});

				Vector3 globalPos = GetGlobalPos({ (float)x, (float)y, (float)z });
				points[i].density = noise->GetNoise(globalPos.x, globalPos.y, globalPos.z);

				points[i].color = GetColorFromDensity(points[i].density);
			}
		}
	}
}

void Chunk::UpdateDensities()
{
	for (int x = 0; x < SIZE; x++)
	{
		for (int y = 0; y < SIZE; y++)
		{
			for (int z = 0; z < SIZE; z++)
			{
				int i = x * SIZE * SIZE + y * SIZE + z;

				Vector3 globalPos = GetGlobalPos({ (float)x, (float)y, (float)z });
				points[i].density = noise->GetNoise(globalPos.x, globalPos.y, globalPos.z);

				points[i].color = GetColorFromDensity(points[i].density);
			}
		}
	}
}

std::vector<Triangle> Chunk::GetMeshVerticesForSection(CubeSection t_cubeSection, float t_surfaceLevel)
{
	std::vector<Triangle> triangles;
	Vector3 vertlist[12] = {};

	int cubeindex = 0;
	// Determine the index into the edge table which tells us which vertices are inside of the surface
	if (t_cubeSection.val[0] < t_surfaceLevel) cubeindex |= 1;
	if (t_cubeSection.val[1] < t_surfaceLevel) cubeindex |= 2;
	if (t_cubeSection.val[2] < t_surfaceLevel) cubeindex |= 4;
	if (t_cubeSection.val[3] < t_surfaceLevel) cubeindex |= 8;
	if (t_cubeSection.val[4] < t_surfaceLevel) cubeindex |= 16;
	if (t_cubeSection.val[5] < t_surfaceLevel) cubeindex |= 32;
	if (t_cubeSection.val[6] < t_surfaceLevel) cubeindex |= 64;
	if (t_cubeSection.val[7] < t_surfaceLevel) cubeindex |= 128;

	// Cube is entirely in/out of the surface 
	if (MarchingTables::EDGE_TABLE[cubeindex] == 0)
		return triangles; // Returns empty

	// Find the vertices where the surface intersects the cube
	if (MarchingTables::EDGE_TABLE[cubeindex] & 1)
		vertlist[0] = VertexInterp(t_cubeSection.p[0], t_cubeSection.p[1], t_cubeSection.val[0], t_cubeSection.val[1], t_surfaceLevel);
	if (MarchingTables::EDGE_TABLE[cubeindex] & 2)
		vertlist[1] = VertexInterp(t_cubeSection.p[1], t_cubeSection.p[2], t_cubeSection.val[1], t_cubeSection.val[2], t_surfaceLevel);
	if (MarchingTables::EDGE_TABLE[cubeindex] & 4)
		vertlist[2] = VertexInterp(t_cubeSection.p[2], t_cubeSection.p[3], t_cubeSection.val[2], t_cubeSection.val[3], t_surfaceLevel);
	if (MarchingTables::EDGE_TABLE[cubeindex] & 8)
		vertlist[3] = VertexInterp(t_cubeSection.p[3], t_cubeSection.p[0], t_cubeSection.val[3], t_cubeSection.val[0], t_surfaceLevel);
	if (MarchingTables::EDGE_TABLE[cubeindex] & 16)
		vertlist[4] = VertexInterp(t_cubeSection.p[4], t_cubeSection.p[5], t_cubeSection.val[4], t_cubeSection.val[5], t_surfaceLevel);
	if (MarchingTables::EDGE_TABLE[cubeindex] & 32)
		vertlist[5] = VertexInterp(t_cubeSection.p[5], t_cubeSection.p[6], t_cubeSection.val[5], t_cubeSection.val[6], t_surfaceLevel);
	if (MarchingTables::EDGE_TABLE[cubeindex] & 64)
		vertlist[6] = VertexInterp(t_cubeSection.p[6], t_cubeSection.p[7], t_cubeSection.val[6], t_cubeSection.val[7], t_surfaceLevel);
	if (MarchingTables::EDGE_TABLE[cubeindex] & 128)
		vertlist[7] = VertexInterp(t_cubeSection.p[7], t_cubeSection.p[4], t_cubeSection.val[7], t_cubeSection.val[4], t_surfaceLevel);
	if (MarchingTables::EDGE_TABLE[cubeindex] & 256)
		vertlist[8] = VertexInterp(t_cubeSection.p[0], t_cubeSection.p[4], t_cubeSection.val[0], t_cubeSection.val[4], t_surfaceLevel);
	if (MarchingTables::EDGE_TABLE[cubeindex] & 512)
		vertlist[9] = VertexInterp(t_cubeSection.p[1], t_cubeSection.p[5], t_cubeSection.val[1], t_cubeSection.val[5], t_surfaceLevel);
	if (MarchingTables::EDGE_TABLE[cubeindex] & 1024)
		vertlist[10] = VertexInterp(t_cubeSection.p[2], t_cubeSection.p[6], t_cubeSection.val[2], t_cubeSection.val[6], t_surfaceLevel);
	if (MarchingTables::EDGE_TABLE[cubeindex] & 2048)
		vertlist[11] = VertexInterp(t_cubeSection.p[3], t_cubeSection.p[7], t_cubeSection.val[3], t_cubeSection.val[7], t_surfaceLevel);

	// Create the triangle 
	for (int i = 0; MarchingTables::TRIANGULATIONS[cubeindex][i] != -1; i += 3)
	{
		Triangle tri;

		tri.a = vertlist[MarchingTables::TRIANGULATIONS[cubeindex][i]];
		tri.b = vertlist[MarchingTables::TRIANGULATIONS[cubeindex][i + 1]];
		tri.c = vertlist[MarchingTables::TRIANGULATIONS[cubeindex][i + 2]];


		// Check if tri connects back to 0, 0, 0. If it does dont add it
		if (tri.a == Vector3Zero() || tri.b == Vector3Zero() || tri.c == Vector3Zero())
		{
			// std::cout << "Found zero vector " << std::endl;
			continue;
		}

		// Get the triangle's normal
		SetTriangleNormal(tri);

		triangles.push_back(tri);
	}

	return triangles;
}

Vector3 Chunk::VertexInterp(Vector3 p1, Vector3 p2, float valp1, float valp2, float t_surfaceLevel)
{
	double wayAlongEdge;
	Vector3 smoothedPoint;

	if (abs(t_surfaceLevel - valp1) < 0.00001)
		return(p1);
	if (abs(t_surfaceLevel - valp2) < 0.00001)
		return(p2);
	if (abs(valp1 - valp2) < 0.00001)
		return(p1);

	wayAlongEdge = (t_surfaceLevel - valp1) / (valp2 - valp1);
	smoothedPoint.x = p1.x + wayAlongEdge * (p2.x - p1.x);
	smoothedPoint.y = p1.y + wayAlongEdge * (p2.y - p1.y);
	smoothedPoint.z = p1.z + wayAlongEdge * (p2.z - p1.z);

	return(smoothedPoint);
}

void Chunk::SetTriangleNormal(Triangle& tri)
{
	Vector3 a = Vector3Subtract(tri.b, tri.a);
	Vector3 b = Vector3Subtract(tri.c, tri.a);

	tri.normal = Vector3Normalize(Vector3CrossProduct(a, b));
}

Color Chunk::GetColorFromDensity(float val)
{
	unsigned char grey = (unsigned char)roundf((val + 1.0f) * 127.5f);
	return { grey, grey, grey, 255 };
}

