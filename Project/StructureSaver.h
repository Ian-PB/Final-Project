#pragma once
#include <raylib.h>
#include <string>
#include <map>
#include <vector>
#include <cmath>

class World;
struct Point;

struct Index3D 
{
	int x = -1;
	int y = -1;
	int z = -1;
};

struct StructureExport
{
	std::string name = "Temp";
	Vector3 dimensions = { 0.0f, 0.0f, 0.0f };
	Vector3 minBounds = { 0.0f, 0.0f, 0.0f };
	Vector3 maxBounds = { 0.0f, 0.0f, 0.0f };
	std::vector<Point> points;
};

enum class StructureTypes
{
	None = -1,
	World
};

class StructureSaver
{
public:
	StructureSaver(World& t_world);

	void SaveFile(StructureTypes t_type, std::string t_name);

	static StructureExport GetStructureFromFile(StructureTypes t_type, std::string t_name);
	static void PlaceStructure(World& t_world, const StructureExport& t_structure, Vector3 t_minCornerPos = { 0.0f, 0.0f, 0.0f });

private:

	BoundingBox FindStructureBounds();
	std::vector<Point> GetPointsToSave(BoundingBox t_bounds);

	static const std::string FILE_PATH;
	static const std::map<StructureTypes, std::string> typeToFilePath;
	static const char SPLITTING_CHAR;

	World& world;

	Index3D Get3DIndex(int i, int axisSize);
};

#include "World.h"
#include "Chunk.h"