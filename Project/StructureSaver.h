#pragma once
#include <raylib.h>
#include <string>
#include <map>
#include <vector>
#include <cmath>
#include "StructureData.h"

class World;
struct Point;

struct Index3D 
{
	int x = -1;
	int y = -1;
	int z = -1;
};

class StructureSaver
{
public:
	StructureSaver(World& t_world);

	void SaveFile(StructureTypes t_type, std::string t_name, BoundingBox t_structureBounds);

private:

	std::vector<Point> GetPointsToSave(BoundingBox t_bounds);

	World& world;

	Index3D Get3DIndex(int i, int axisSize);
};

#include "World.h"
#include "Chunk.h"