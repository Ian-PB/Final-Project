#include "StructurePaster.h"
#include <fstream>
#include <raymath.h>
#include <iostream>
#include <string>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "StructureData.h"
#include "World.h"
#include "Materials.h"

StructureExport StructurePaster::GetStructureFromFile(StructureTypes t_type, std::string t_name)
{
    json jsonObj;
    std::string path = StructureData::FILE_PATH + StructureData::typeToFilePath.at(t_type) + t_name + ".json";
    std::ifstream file(path);

    if (!file.is_open())
    {
        std::cerr << "File not found at: " << path << '\n';
        return StructureExport();
    }

    file >> jsonObj;

    StructureExport exportedStructure;

    exportedStructure.minBounds.x = jsonObj["minBounds"][0].get<float>();
    exportedStructure.minBounds.y = jsonObj["minBounds"][1].get<float>();
    exportedStructure.minBounds.z = jsonObj["minBounds"][2].get<float>();

    exportedStructure.maxBounds.x = jsonObj["maxBounds"][0].get<float>();
    exportedStructure.maxBounds.y = jsonObj["maxBounds"][1].get<float>();
    exportedStructure.maxBounds.z = jsonObj["maxBounds"][2].get<float>();

    for (const auto& jsonPoint : jsonObj["points"])
    {
        Point p;

        p.position.x = jsonPoint["position"][0].get<float>();
        p.position.y = jsonPoint["position"][1].get<float>();
        p.position.z = jsonPoint["position"][2].get<float>();

        p.density = jsonPoint["density"].get<float>();

        Color materialColor = Materials::MATERIAL_TO_COLOR.at(static_cast<MaterialTypes>(jsonPoint["material"].get<int>()));
        p.color.r = materialColor.r;
        p.color.g = materialColor.g;
        p.color.b = materialColor.b;
        p.color.a = materialColor.a;

        exportedStructure.points.push_back(p);
    }

    return exportedStructure;
}

void StructurePaster::PlaceStructure(World& t_world, const StructureExport& t_structure, Vector3 t_centerBottomPos)
{
    float chunkPointSpacing = t_world.GetChunk(0, 0, 0).GetPointSpacing();
    BoundingBox structureBounds = { t_structure.minBounds, t_structure.maxBounds };

    // Center of structure placement
    Vector3 flatStructureMax = { structureBounds.max.x, 0.0f, structureBounds.max.z };
    Vector3 minCornerPos = Vector3Subtract(t_centerBottomPos, flatStructureMax / 2.0f);

    for (const Point& p : t_structure.points)
    {
        Vector3 pointWorldPos = Vector3Add(p.position, minCornerPos);
        BoundingBox edgeCheckBounds = { Vector3SubtractValue(pointWorldPos, chunkPointSpacing), Vector3AddValue(pointWorldPos,chunkPointSpacing) };

        std::vector<Chunk*> chunks = t_world.GetChunksInBounds(edgeCheckBounds);

        for (Chunk* chunk : chunks)
        {
            Point* pointToChange = nullptr;

            if (chunk->TryGetPointAtPos(pointWorldPos, pointToChange))
            {
                pointToChange->density = std::min(pointToChange->density, p.density);
                pointToChange->color = p.color;
            }
        }
    }
}
