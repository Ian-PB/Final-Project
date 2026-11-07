#include "Chunk.h"
#include <raymath.h>
#include <iostream>

#include "MarchingTables.h"

Chunk::Chunk()
{
}

// FastNoiseLite is a library I added
void Chunk::Init(FastNoiseLite* t_noise, Shader* t_shader)
{
	shader = t_shader;

	noise = t_noise;
	Mesh sphereMesh = GenMeshSphere(0.1f, 5, 5);
	pointModel = LoadModelFromMesh(sphereMesh);

	SetupPoints();
}

void Chunk::InitCollider()
{
	collider.Init(triangles, position);
}

void Chunk::Draw()
{
	DrawModel(model, position, 1.0f, GRAY);
	// DrawModelWires(model, position, 1.0f, LIGHTGRAY);

	collider.Draw();
}

void Chunk::Update()
{
	//if (IsKeyReleased(KEY_P))
	//	showDebugPoints = !showDebugPoints;

	collider.Update();
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
	mesh.colors = (unsigned char*)MemAlloc(mesh.vertexCount * 4);

	// Apply all info to mesh
	for (int i = 0; i < mesh.vertexCount; i++)
	{
		int vertA = i * 3;
		int vertB = i * 3 + 1;
		int vertC = i * 3 + 2;

		// Vertices
		mesh.vertices[vertA] = meshData.vertices[i].x;
		mesh.vertices[vertB] = meshData.vertices[i].y;
		mesh.vertices[vertC] = meshData.vertices[i].z;

		// Normals
		mesh.normals[vertA] = meshData.normals[i].x;
		mesh.normals[vertB] = meshData.normals[i].y;
		mesh.normals[vertC] = meshData.normals[i].z;

		// Colors
		int colorIndex = i * 4;
		mesh.colors[colorIndex] = meshData.colors[i].r;
		mesh.colors[colorIndex + 1] = meshData.colors[i].g;
		mesh.colors[colorIndex + 2] = meshData.colors[i].b;
		mesh.colors[colorIndex + 3] = meshData.colors[i].a;
	}

	// Upload Mesh and apply to Model
	UploadMesh(&mesh, true);
	model = LoadModelFromMesh(mesh);
	model.materials[0].shader = *shader;

	collider.SetBounds(GetBounds());

	// No longer needs a change
	dirty = false;
}

// Resources used to research how to do marching cubes:
// https://www.youtube.com/watch?v=M3iI2l0ltbE 
// https://mclark45.medium.com/marching-cubes-algorithm-618302629331 
// https://developer.nvidia.com/gpugems/gpugems3/part-i-geometry/chapter-1-generating-complex-procedural-terrains-using-gpu 
// https://paulbourke.net/geometry/polygonise/ 
void Chunk::GenerateMeshData(float t_surfaceLevel)
{
	surfaceLevel = t_surfaceLevel;
	dirty = true;

	triangles.clear();

	meshData.vertices.clear();
	meshData.normals.clear();
	meshData.colors.clear();

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
					cubeSection.colors[i] = points[index].color;
				}

				GetMeshVerticesForSection(cubeSection);
			}
		}
	}

	for (const Triangle& tri : triangles)
	{
		meshData.vertices.push_back(tri.a);
		meshData.vertices.push_back(tri.b);
		meshData.vertices.push_back(tri.c);

		// All vertices get same normal (flat shading)
		meshData.normals.push_back(tri.normal);
		meshData.normals.push_back(tri.normal);
		meshData.normals.push_back(tri.normal);

		// Set each vert to have it's point's color
		meshData.colors.push_back(tri.aColor);
		meshData.colors.push_back(tri.bColor);
		meshData.colors.push_back(tri.cColor);
	}

	meshData.currentVertexCount = (int)meshData.vertices.size();
}

BoundingBox Chunk::GetBounds()
{
	BoundingBox bounds = GetMeshBoundingBox(mesh);

	bounds.min = Vector3Add(bounds.min, position);
	bounds.max = Vector3Add(bounds.max, position);

	return bounds;
}

void Chunk::EditSphere(Vector3 t_pos, float t_radius, bool destroy)
{
	for (int i = 0; i < SIZE*SIZE*SIZE; i++)
	{
		Vector3 pointWorldPos = Vector3Add(position, points[i].position);
		float dist = Vector3Distance(pointWorldPos, t_pos);

		if (dist <= t_radius + EDIT_MARGIN)
		{
			// Signed distance: Neg = within radius, Pos = Outside radius
			float newDensity = dist - t_radius;

			if (destroy)
				points[i].density = fmaxf(points[i].density, -newDensity); // Keep highest value since 1 = air
			else
				points[i].density = fminf(points[i].density, newDensity); // Keep lowest value since -1 = solid
		}
	}
	int temp = 0;
}

void Chunk::ClearChunk(WallFlags walls)
{
	for (int x = 0; x < SIZE; x++)
	{
		for (int y = 0; y < SIZE; y++)
		{
			for (int z = 0; z < SIZE; z++)
			{
				int i = IndexFrom3D(x, y, z);
				bool solid = 
					(walls.posXWall && x == 1) ||			// Pos X Wall
					(walls.negXWall && x == SIZE - 2) ||	// Neg X Wall
					(walls.posYWall && y == 1) ||			// Pos Y Wall
					(walls.negYWall && y == SIZE - 2) ||	// Neg Y Wall
					(walls.posZWall && z == 1) ||			// Pos Z Wall
					(walls.negZWall && z == SIZE - 2);		// Neg Z Wall

				if (solid)
					points[i].density = -1; // Solid
				else
					points[i].density = 1; // Empty

				points[i].color = RAYWHITE;
			}
		}
	}
}

bool Chunk::TryGetPointAtPos(Vector3 t_pos, Point*& t_point)
{
	// Convert to localPos
	Vector3 localPos = Vector3Subtract(t_pos, position);

	// Convert to grid index
	int x = (int)std::round(localPos.x / pointSpacing);
	int y = (int)std::round(localPos.y / pointSpacing);
	int z = (int)std::round(localPos.z / pointSpacing);

	// Bounds check
	if (x < 0 || x >= SIZE || y < 0 || y >= SIZE || z < 0 || z >= SIZE)
	{
		return false;
	}

	t_point = &points[IndexFrom3D(x, y, z)];
	return true;
}

void Chunk::SetupPoints()
{
	for (int x = 0; x < SIZE; x++)
	{
		for (int y = 0; y < SIZE; y++)
		{
			for (int z = 0; z < SIZE; z++)
			{
				int i = IndexFrom3D(x, y, z);

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
				int i = IndexFrom3D(x, y, z);

				Vector3 globalPos = GetGlobalPos({ (float)x, (float)y, (float)z });
				points[i].density = noise->GetNoise(globalPos.x, globalPos.y, globalPos.z);

				points[i].color = GetColorFromDensity(points[i].density);
			}
		}
	}
}

// Resources used to research how to do marching cubes:
// https://www.youtube.com/watch?v=M3iI2l0ltbE 
// https://mclark45.medium.com/marching-cubes-algorithm-618302629331 
//https://developer.nvidia.com/gpugems/gpugems3/part-i-geometry/chapter-1-generating-complex-procedural-terrains-using-gpu 
// https://paulbourke.net/geometry/polygonise/ 
void Chunk::GetMeshVerticesForSection(CubeSection t_cubeSection)
{
	Vector3 vertlist[12] = {};
	Color colorlist[12] = {};

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
		return; // Returns empty

	// Find the vertices where the surface intersects the cube
	if (MarchingTables::EDGE_TABLE[cubeindex] & 1)
	{
		vertlist[0] = VertexInterp(t_cubeSection.p[0], t_cubeSection.p[1], t_cubeSection.val[0], t_cubeSection.val[1]);
		colorlist[0] = ColorInterp(t_cubeSection.colors[0], t_cubeSection.colors[1], t_cubeSection.val[0], t_cubeSection.val[1]);
	}
	if (MarchingTables::EDGE_TABLE[cubeindex] & 2)
	{
		vertlist[1] = VertexInterp(t_cubeSection.p[1], t_cubeSection.p[2], t_cubeSection.val[1], t_cubeSection.val[2]);
		colorlist[1] = ColorInterp(t_cubeSection.colors[1], t_cubeSection.colors[2], t_cubeSection.val[1], t_cubeSection.val[2]);
	}
	if (MarchingTables::EDGE_TABLE[cubeindex] & 4)
	{
		vertlist[2] = VertexInterp(t_cubeSection.p[2], t_cubeSection.p[3], t_cubeSection.val[2], t_cubeSection.val[3]);
		colorlist[2] = ColorInterp(t_cubeSection.colors[2], t_cubeSection.colors[3], t_cubeSection.val[2], t_cubeSection.val[3]);
	}
	if (MarchingTables::EDGE_TABLE[cubeindex] & 8)
	{
		vertlist[3] = VertexInterp(t_cubeSection.p[3], t_cubeSection.p[0], t_cubeSection.val[3], t_cubeSection.val[0]);
		colorlist[3] = ColorInterp(t_cubeSection.colors[3], t_cubeSection.colors[0], t_cubeSection.val[3], t_cubeSection.val[0]);
	}
	if (MarchingTables::EDGE_TABLE[cubeindex] & 16)
	{
		vertlist[4] = VertexInterp(t_cubeSection.p[4], t_cubeSection.p[5], t_cubeSection.val[4], t_cubeSection.val[5]);
		colorlist[4] = ColorInterp(t_cubeSection.colors[4], t_cubeSection.colors[5], t_cubeSection.val[4], t_cubeSection.val[5]);
	}
	if (MarchingTables::EDGE_TABLE[cubeindex] & 32)
	{
		vertlist[5] = VertexInterp(t_cubeSection.p[5], t_cubeSection.p[6], t_cubeSection.val[5], t_cubeSection.val[6]);
		colorlist[5] = ColorInterp(t_cubeSection.colors[5], t_cubeSection.colors[6], t_cubeSection.val[5], t_cubeSection.val[6]);
	}
	if (MarchingTables::EDGE_TABLE[cubeindex] & 64)
	{
		vertlist[6] = VertexInterp(t_cubeSection.p[6], t_cubeSection.p[7], t_cubeSection.val[6], t_cubeSection.val[7]);
		colorlist[6] = ColorInterp(t_cubeSection.colors[6], t_cubeSection.colors[7], t_cubeSection.val[6], t_cubeSection.val[7]);
	}
	if (MarchingTables::EDGE_TABLE[cubeindex] & 128)
	{
		vertlist[7] = VertexInterp(t_cubeSection.p[7], t_cubeSection.p[4], t_cubeSection.val[7], t_cubeSection.val[4]);
		colorlist[7] = ColorInterp(t_cubeSection.colors[7], t_cubeSection.colors[4], t_cubeSection.val[7], t_cubeSection.val[4]);
	}
	if (MarchingTables::EDGE_TABLE[cubeindex] & 256)
	{
		vertlist[8] = VertexInterp(t_cubeSection.p[0], t_cubeSection.p[4], t_cubeSection.val[0], t_cubeSection.val[4]);
		colorlist[8] = ColorInterp(t_cubeSection.colors[0], t_cubeSection.colors[4], t_cubeSection.val[0], t_cubeSection.val[4]);
	}
	if (MarchingTables::EDGE_TABLE[cubeindex] & 512)
	{
		vertlist[9] = VertexInterp(t_cubeSection.p[1], t_cubeSection.p[5], t_cubeSection.val[1], t_cubeSection.val[5]);
		colorlist[9] = ColorInterp(t_cubeSection.colors[1], t_cubeSection.colors[5], t_cubeSection.val[1], t_cubeSection.val[5]);
	}
	if (MarchingTables::EDGE_TABLE[cubeindex] & 1024)
	{
		vertlist[10] = VertexInterp(t_cubeSection.p[2], t_cubeSection.p[6], t_cubeSection.val[2], t_cubeSection.val[6]);
		colorlist[10] = ColorInterp(t_cubeSection.colors[2], t_cubeSection.colors[6], t_cubeSection.val[2], t_cubeSection.val[6]);
	}
	if (MarchingTables::EDGE_TABLE[cubeindex] & 2048)
	{
		vertlist[11] = VertexInterp(t_cubeSection.p[3], t_cubeSection.p[7], t_cubeSection.val[3], t_cubeSection.val[7]);
		colorlist[11] = ColorInterp(t_cubeSection.colors[3], t_cubeSection.colors[7], t_cubeSection.val[3], t_cubeSection.val[7]);
	}

	// Create the triangle 
	for (int i = 0; MarchingTables::TRIANGULATIONS[cubeindex][i] != -1; i += 3)
	{
		Triangle tri;

		int edgeA = MarchingTables::TRIANGULATIONS[cubeindex][i];
		int edgeB = MarchingTables::TRIANGULATIONS[cubeindex][i + 1];
		int edgeC = MarchingTables::TRIANGULATIONS[cubeindex][i + 2];

		tri.a = vertlist[edgeA];
		tri.b = vertlist[edgeB];
		tri.c = vertlist[edgeC];

		tri.aColor = colorlist[edgeA];
		tri.bColor = colorlist[edgeB];
		tri.cColor = colorlist[edgeC];

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
}

// Resources used to research how to do marching cubes:
// https://www.youtube.com/watch?v=M3iI2l0ltbE 
// https://mclark45.medium.com/marching-cubes-algorithm-618302629331 
// https://developer.nvidia.com/gpugems/gpugems3/part-i-geometry/chapter-1-generating-complex-procedural-terrains-using-gpu 
// https://paulbourke.net/geometry/polygonise/ 
Vector3 Chunk::VertexInterp(Vector3 p1, Vector3 p2, float valp1, float valp2)
{
	float wayAlongEdge;
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

Color Chunk::ColorInterp(Color p1, Color p2, float valp1, float valp2)
{
	float wayAlongEdge;
	Color smoothedColor;

	if (abs(surfaceLevel - valp1) < 0.00001)
		return(p1);
	if (abs(surfaceLevel - valp2) < 0.00001)
		return(p2);
	if (abs(valp1 - valp2) < 0.00001)
		return(p1);

	wayAlongEdge = (surfaceLevel - valp1) / (valp2 - valp1);
	smoothedColor.r = p1.r + wayAlongEdge * (p2.r - p1.r);
	smoothedColor.g = p1.g + wayAlongEdge * (p2.g - p1.g);
	smoothedColor.b = p1.b + wayAlongEdge * (p2.b - p1.b);

	return smoothedColor;
}

void Chunk::SetTriangleNormal(Triangle& tri)
{
	Vector3 a = Vector3Subtract(tri.b, tri.a);
	Vector3 b = Vector3Subtract(tri.c, tri.a);

	tri.normal = Vector3Normalize(Vector3CrossProduct(a, b));
}

Color Chunk::GetColorFromDensity(float t_density)
{
	Color color = RAYWHITE;

	float minSurface = surfaceLevel - 0.1f;
	float maxSurface = surfaceLevel + 0.1f;

	if (t_density >= minSurface && t_density <= maxSurface)
		color = GREEN;
	else if (t_density < minSurface)
		color = RED;
	else if (t_density > maxSurface)
		color = BLUE;

	return color;
}