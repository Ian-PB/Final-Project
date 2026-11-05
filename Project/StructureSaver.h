#pragma once
#include <raylib.h>
#include <raygui.h>
#include <string>
#include <map>
#include <vector>

class World;
struct Point;

struct Index3D 
{
	int x = -1;
	int y = -1;
	int z = -1;
};

struct PointBounds
{
	Vector3 minPos = { -9999.0f, -9999.0f, -9999.0f };
	Index3D minIndex3D;

	Vector3 maxPos = { 9999.0f, 9999.0f, 9999.0f };
	Index3D maxIndex3D;
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

private:

	PointBounds FindStructureBounds();
	std::vector<float> GetPointsToSave(PointBounds t_bounds);

	static const std::string FILE_PATH;
	static const std::map<StructureTypes, std::string> typeToFilePath;
	static const char SPLITTING_CHAR;

	World& world;

	Index3D Get3DIndex(int i, int axisSize);
	int GetFlatIndex(Index3D index3D, int axisSize) { return index3D.x + index3D.y * axisSize + index3D.z * axisSize * axisSize; }
	int GetFlatIndex(int x, int y, int z, int axisSize) { return x + y * axisSize + z * axisSize * axisSize; }
};

#include "World.h"
#include "Chunk.h"