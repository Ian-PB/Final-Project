#include "Chunk.h"
#include <raymath.h>
#include <iostream>

#include "MarchingTables.h"

Chunk::Chunk()
{
	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	noise.SetFrequency(frequency);
	noise.SetSeed((int)(rand() % 9999));
}

void Chunk::Init()
{
	Mesh sphereMesh = GenMeshSphere(0.1f, 5, 5);
	pointModel = LoadModelFromMesh(sphereMesh);

	// Setup chunk's mesh using max vertex count
	int maxVertexCount = (SIZE - 1) * (SIZE - 1) * (SIZE - 1) * 15;
	mesh.vertexCount = maxVertexCount;
	mesh.triangleCount = maxVertexCount / 3.0f;

	mesh.vertices = (float*)MemAlloc(maxVertexCount * 3 * sizeof(float));

	UploadMesh(&mesh, true);
	model = LoadModelFromMesh(mesh);

	SetupPoints();
	GenerateMesh();
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

	DrawModel(model, position, 1.0f, LIGHTGRAY);
	DrawModelWires(model, position, 1.0f, MAROON);
}

void Chunk::Update()
{
	bool changedPoints = false;

	if (IsKeyDown(KEY_UP))
	{
		surfaceLevel += 0.01f;

		if (surfaceLevel > 1.0f)
			surfaceLevel = 1.0f;

		changedPoints = true;
	}
	else if (IsKeyDown(KEY_DOWN))
	{
		surfaceLevel -= 0.01f;

		if (surfaceLevel < -1.0f)
			surfaceLevel = -1.0f;

		changedPoints = true;
	}

	if (IsKeyDown(KEY_LEFT))
	{
		scrollX -= 0.1f;
		UpdateDensities();
		changedPoints = true;
	}
	else if (IsKeyDown(KEY_RIGHT))
	{
		scrollX += 0.1f;
		UpdateDensities();
		changedPoints = true;
	}

	if (IsKeyReleased(KEY_SPACE))
	{
		noise.SetSeed((int)(rand() % 9999));
		scrollX = 0.0f;
		UpdateDensities();
		changedPoints = true;
	}


	if (changedPoints)
		GenerateMesh();

	if (IsKeyReleased(KEY_P))
		showDebugPoints = !showDebugPoints;
}

void Chunk::GenerateMesh()
{
	std::vector<Vector3> vertices;

	for (int x = 0; x < SIZE - 1; x++)
	{
		for (int y = 0; y < SIZE - 1; y++)
		{
			for (int z = 0; z < SIZE - 1; z++)
			{
				// int i = x * SIZE * SIZE + y * SIZE + z;
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

				std::vector<Triangle> triangles = GetMeshVerticesForSection(cubeSection);

				for (const Triangle& tri : triangles)
				{
					vertices.push_back(tri.a);
					vertices.push_back(tri.b);
					vertices.push_back(tri.c);
				}
			}
		}
	}

	int currentVertexCount = (int)vertices.size();

	// Apply new vertices found to mesh
	mesh.vertexCount = currentVertexCount;
	mesh.triangleCount = currentVertexCount / 3;

	for (int i = 0; i < mesh.vertexCount; i++)
	{
		mesh.vertices[i * 3 + 0] = vertices[i].x;
		mesh.vertices[i * 3 + 1] = vertices[i].y;
		mesh.vertices[i * 3 + 2] = vertices[i].z;
	}

	// Update the mesh on the gpu buffer with the new vertices
	UpdateMeshBuffer(mesh, 0, mesh.vertices, currentVertexCount * 3 * sizeof(float), 0);
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

				Vector3 localPos = { x * pointSpacing, y * pointSpacing, z * pointSpacing };
				points[i].position = localPos;
				points[i].density = noise.GetNoise(localPos.x + scrollX, localPos.y, localPos.z);

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
				Vector3 localPos = { x * pointSpacing, y * pointSpacing, z * pointSpacing };
				points[i].density = noise.GetNoise(localPos.x + scrollX, localPos.y, localPos.z);
			}
		}
	}
}

std::vector<Triangle> Chunk::GetMeshVerticesForSection(CubeSection t_cubeSection)
{
	std::vector<Triangle> triangles;
	Vector3 vertlist[12] = {};

	int cubeindex = 0;
	// Determine the index into the edge table which tells us which vertices are inside of the surface
	if (t_cubeSection.val[0] < surfaceLevel) cubeindex |= 1;
	if (t_cubeSection.val[1] < surfaceLevel) cubeindex |= 2;
	if (t_cubeSection.val[2] < surfaceLevel) cubeindex |= 4;
	if (t_cubeSection.val[3] < surfaceLevel) cubeindex |= 8;
	if (t_cubeSection.val[4] < surfaceLevel) cubeindex |= 16;
	if (t_cubeSection.val[5] < surfaceLevel) cubeindex |= 32;
	if (t_cubeSection.val[6] < surfaceLevel) cubeindex |= 64;
	if (t_cubeSection.val[7] < surfaceLevel) cubeindex |= 128;

	// Cube is entirely in/out of the surface 
	if (MarchingTables::EDGE_TABLE[cubeindex] == 0)
		return triangles; // Returns empty

	// Find the vertices where the surface intersects the cube
	if (MarchingTables::EDGE_TABLE[cubeindex] & 1)
		vertlist[0] = VertexInterp(t_cubeSection.p[0], t_cubeSection.p[1], t_cubeSection.val[0], t_cubeSection.val[1]);
	if (MarchingTables::EDGE_TABLE[cubeindex] & 2)
		vertlist[1] = VertexInterp(t_cubeSection.p[1], t_cubeSection.p[2], t_cubeSection.val[1], t_cubeSection.val[2]);
	if (MarchingTables::EDGE_TABLE[cubeindex] & 4)
		vertlist[2] = VertexInterp(t_cubeSection.p[2], t_cubeSection.p[3], t_cubeSection.val[2], t_cubeSection.val[3]);
	if (MarchingTables::EDGE_TABLE[cubeindex] & 8)
		vertlist[3] = VertexInterp(t_cubeSection.p[3], t_cubeSection.p[0], t_cubeSection.val[3], t_cubeSection.val[0]);
	if (MarchingTables::EDGE_TABLE[cubeindex] & 16)
		vertlist[4] = VertexInterp(t_cubeSection.p[4], t_cubeSection.p[5], t_cubeSection.val[4], t_cubeSection.val[5]);
	if (MarchingTables::EDGE_TABLE[cubeindex] & 32)
		vertlist[5] = VertexInterp(t_cubeSection.p[5], t_cubeSection.p[6], t_cubeSection.val[5], t_cubeSection.val[6]);
	if (MarchingTables::EDGE_TABLE[cubeindex] & 64)
		vertlist[6] = VertexInterp(t_cubeSection.p[6], t_cubeSection.p[7], t_cubeSection.val[6], t_cubeSection.val[7]);
	if (MarchingTables::EDGE_TABLE[cubeindex] & 128)
		vertlist[7] = VertexInterp(t_cubeSection.p[7], t_cubeSection.p[4], t_cubeSection.val[7], t_cubeSection.val[4]);
	if (MarchingTables::EDGE_TABLE[cubeindex] & 256)
		vertlist[8] = VertexInterp(t_cubeSection.p[0], t_cubeSection.p[4], t_cubeSection.val[0], t_cubeSection.val[4]);
	if (MarchingTables::EDGE_TABLE[cubeindex] & 512)
		vertlist[9] = VertexInterp(t_cubeSection.p[1], t_cubeSection.p[5], t_cubeSection.val[1], t_cubeSection.val[5]);
	if (MarchingTables::EDGE_TABLE[cubeindex] & 1024)
		vertlist[10] = VertexInterp(t_cubeSection.p[2], t_cubeSection.p[6], t_cubeSection.val[2], t_cubeSection.val[6]);
	if (MarchingTables::EDGE_TABLE[cubeindex] & 2048)
		vertlist[11] = VertexInterp(t_cubeSection.p[3], t_cubeSection.p[7], t_cubeSection.val[3], t_cubeSection.val[7]);

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

		triangles.push_back(tri);
	}

	return triangles;
}

Vector3 Chunk::VertexInterp(Vector3 p1, Vector3 p2, float valp1, float valp2)
{
	double wayAlongEdge;
	Vector3 smoothedPoint;

	if (abs(surfaceLevel - valp1) < 0.00001)
		return(p1);
	if (abs(surfaceLevel - valp2) < 0.00001)
		return(p2);
	if (abs(valp1 - valp2) < 0.00001)
		return(p1);

	wayAlongEdge = (surfaceLevel - valp1) / (valp2 - valp1);
	smoothedPoint.x = p1.x + wayAlongEdge * (p2.x - p1.x);
	smoothedPoint.y = p1.y + wayAlongEdge * (p2.y - p1.y);
	smoothedPoint.z = p1.z + wayAlongEdge * (p2.z - p1.z);

	return(smoothedPoint);
}

Color Chunk::GetColorFromDensity(float val)
{
	unsigned char grey = (unsigned char)roundf((val + 1.0f) * 127.5f);
	return { grey, grey, grey, 255 };
}

