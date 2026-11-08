#include "StructureSaver.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <raymath.h>
using json = nlohmann::json;

#include "StructureData.h"
#include "Materials.h"

StructureSaver::StructureSaver(World& t_world) : world(t_world)
{
}

void StructureSaver::SaveFile(StructureTypes t_type, std::string t_name, BoundingBox t_structureBounds)
{
    std::string path = StructureData::FILE_PATH + StructureData::typeToFilePath.at(t_type) + t_name + ".json";
    std::cout << "Saved to: " << path << std::endl;

    std::ofstream file(path);

    BoundingBox bounds = t_structureBounds;

    // Create JSON Object
    json jsonObj;

    std::vector<Point> points = GetPointsToSave(bounds);
    for (const Point& p : points)
    {
        Vector3 correctedPointPos = Vector3Subtract(p.position, bounds.min);

        jsonObj["points"].push_back({
            { "position", { correctedPointPos.x, correctedPointPos.y, correctedPointPos.z } },
            { "density", p.density },
            { "material", static_cast<int>(Materials::COLOR_TO_MATERIAL.at(p.color)) }
        });
    }

    BoundingBox boundsAroundZero;
    // Set min to { 0, 0, 0 }
    boundsAroundZero.min = { 0.0f, 0.0f, 0.0f };
    boundsAroundZero.max = Vector3Subtract(bounds.max, bounds.min);

    jsonObj["minBounds"] = { boundsAroundZero.min.x, boundsAroundZero.min.y, boundsAroundZero.min.z };
    jsonObj["maxBounds"] = { boundsAroundZero.max.x, boundsAroundZero.max.y, boundsAroundZero.max.z };

    if (file.is_open())
    {
        file << jsonObj.dump();
    }
}

std::vector<Point> StructureSaver::GetPointsToSave(BoundingBox t_bounds)
{
    // Same across all chunks
    int pointsPerAxis = world.GetChunk(0, 0, 0).GetPointsPerAxis();
    std::vector<Point> points;

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
                    int i = chunk->IndexFrom3D(x, y, z);
                    Point& globalPoint = chunk->GetPointAt(i);

                    // Check if the point is just air
                    if (globalPoint.density >= 1.0f)
                        continue;
    
                    Vector3 globalPos = chunk->GetGlobalPos({ (float)x, (float)y, (float)z });

                    // Check if outside of bounds
                    if (globalPos.x < t_bounds.min.x || globalPos.x > t_bounds.max.x ||
                        globalPos.y < t_bounds.min.y || globalPos.y > t_bounds.max.y ||
                        globalPos.z < t_bounds.min.z || globalPos.z > t_bounds.max.z)
                    {
                        continue;
                    }

                    globalPoint.position = globalPos;

                    points.push_back(globalPoint);
                }
            }
        }
    }

    return points;
}

Index3D StructureSaver::Get3DIndex(int i, int axisSize)
{
    Index3D index3D;
    index3D.x = i / (axisSize * axisSize);
    index3D.y = (i / axisSize) % axisSize;
    index3D.z = i % axisSize;

    return index3D;
}
