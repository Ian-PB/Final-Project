#pragma once
#include <raylib.h>
#include <string>
#include <vector>
#include <map>

struct Point;

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

class StructureData
{
public:
	static const std::string FILE_PATH;
	static const std::map<StructureTypes, std::string> typeToFilePath;
};

#include "Chunk.h"