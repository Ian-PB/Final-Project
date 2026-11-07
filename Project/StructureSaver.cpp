#include "StructureSaver.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <raymath.h>
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

    // Create JSON Object
    json jsonObj;

    std::vector<Point> points = GetPointsToSave(bounds);
    for (const Point& p : points)
    {
        Vector3 correctedPointPos = Vector3Subtract(p.position, bounds.min);

        jsonObj["points"].push_back({
            { "position", { correctedPointPos.x, correctedPointPos.y, correctedPointPos.z } },
            { "density", p.density }
            // Color can be added later
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
        file << jsonObj.dump(4);
    }
}

StructureExport StructureSaver::GetStructureFromFile(StructureTypes t_type, std::string t_name)
{
    json jsonObj;
    std::string path = FILE_PATH + typeToFilePath.at(t_type) + t_name + ".json";
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

        exportedStructure.points.push_back(p);
    }

    return exportedStructure;
}

void StructureSaver::PlaceStructure(World& t_world, const StructureExport& t_structure, Vector3 t_centerBottomPos)
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
            }
        }
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
