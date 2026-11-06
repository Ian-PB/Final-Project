#include "StructureSaver.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

const std::string StructureSaver::FILE_PATH = "resources/Structures/";
const std::map<StructureTypes, std::string> StructureSaver::typeToFilePath = {
    { StructureTypes::None, "None/" },
    { StructureTypes::World, "Worlds/" }
};
const char StructureSaver::SPLITTING_CHAR = '@';

StructureSaver::StructureSaver(World& t_world) : world(t_world)
{
}

void StructureSaver::SaveFile(StructureTypes t_type, std::string t_name)
{
    std::string path = FILE_PATH + typeToFilePath.at(t_type) + t_name + ".json";
    std::cout << "Saved to: " << path << std::endl;

    std::ofstream file(path);

    BoundingBox bounds = FindStructureBounds();

    Vector3 dimensions;
    dimensions.x = std::abs(bounds.max.x - bounds.min.x);
    dimensions.y = std::abs(bounds.max.y - bounds.min.y);
    dimensions.z = std::abs(bounds.max.z - bounds.min.z);

    // Create JSON Object
    json jsonObj;
    jsonObj["name"] = t_name;
    jsonObj["dimensions"] = { dimensions.x, dimensions.y, dimensions.z };
    jsonObj["densities"] = GetPointsToSave(bounds);

    if (file.is_open())
    {
        file << jsonObj.dump(4);
    }
}

BoundingBox StructureSaver::FindStructureBounds()
{
    // Same across all chunks
    int pointsPerAxis = world.GetChunk(0, 0, 0).GetPointsPerAxis();
    BoundingBox bounds = {};
    bool foundPoint = false;

    for (Chunk* chunk : world.GetAllChunks())
    {
        // Loop through chunks points
        for (int i = 0; i < chunk->GetPointAmount(); i++)
        {
            const Point& p = chunk->GetPointAt(i);

            // Ignore empty points
            if (p.density >= 1.0f)
                continue;

            Index3D i3D = Get3DIndex(i, pointsPerAxis);
            Vector3 globalPos = chunk->GetGlobalPos({ (float)i3D.x, (float)i3D.y, (float)i3D.z });

            if (!foundPoint)
            {
                bounds.min = globalPos;
                bounds.max = globalPos;
                foundPoint = true;
                continue;
            }

            // Change each axis indepentantly
            bounds.min.x = std::min(globalPos.x, bounds.min.x);
            bounds.min.y = std::min(globalPos.y, bounds.min.y);
            bounds.min.z = std::min(globalPos.z, bounds.min.z);

            bounds.max.x = std::max(globalPos.x, bounds.max.x);
            bounds.max.y = std::max(globalPos.y, bounds.max.y);
            bounds.max.z = std::max(globalPos.z, bounds.max.z);
        }
    }

    return bounds;
}

std::vector<float> StructureSaver::GetPointsToSave(BoundingBox t_bounds)
{
    // Same across all chunks
    int pointsPerAxis = world.GetChunk(0, 0, 0).GetPointsPerAxis();
    std::vector<float> points;

    // Get chunks in bounds
    std::vector<Chunk*> chunks = world.GetChunksInBounds(t_bounds);
    
    for (Chunk* chunk : chunks)
    {
        for (int x = 0; x < pointsPerAxis; x++)
        {
            for (int y = 0; y < pointsPerAxis; y++)
            {
                for (int z = 0; z < pointsPerAxis; z++)
                {
                    int i = GetFlatIndex({ x, y, z }, pointsPerAxis);

                    Vector3 globalPos = chunk->GetGlobalPos({ (float)x, (float)y, (float)z });

                    // Check if outside of bounds
                    if (globalPos.x < t_bounds.min.x || globalPos.x > t_bounds.max.x ||
                        globalPos.y < t_bounds.min.y || globalPos.y > t_bounds.max.y ||
                        globalPos.z < t_bounds.min.z || globalPos.z > t_bounds.max.z)
                    {
                        continue;
                    }

                    points.push_back(chunk->GetPointAt(i).density);
                }
            }
        }
    }

    return points;
}

Index3D StructureSaver::Get3DIndex(int i, int axisSize)
{
    Index3D index3D;
    index3D.x = i % axisSize;
    index3D.y = (i / axisSize) % axisSize;
    index3D.z = i / (axisSize * axisSize);

    return index3D;
}
