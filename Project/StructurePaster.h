#pragma once
#include <raylib.h>
#include <string>
#include "StructureData.h"

class World;

class StructurePaster
{
public:
	static StructureExport GetStructureFromFile(StructureTypes t_type, std::string t_name);
	static void PlaceStructure(World& t_world, const StructureExport& t_structure, Vector3 t_centerBottomPos = { 0.0f, 0.0f, 0.0f });
};

#include "World.h"
