#include "StructureSaver.h"
#include <iostream>
#include <fstream>
#include <raymath.h>

const std::string StructureSaver::FILE_PATH = "resources/Structures/";
const std::map<StructureTypes, std::string> StructureSaver::typeToFilePath = {
    { StructureTypes::None, "None/" },
    { StructureTypes::World, "World/" }
};
const char StructureSaver::SPLITTING_CHAR = '@';

StructureSaver::StructureSaver(World& t_world) : world(t_world)
{
}

void StructureSaver::SaveFile(StructureTypes t_type, std::string t_name)
{
    std::string path = FILE_PATH + typeToFilePath.at(t_type) + t_name;

    std::ofstream structureFile(path);

    PointBounds bounds = FindStructureBounds();
    std::vector<float> pointDensities = GetPointsToSave(bounds);

    for (const int& p : pointDensities)
    {
        structureFile << p;
        structureFile << SPLITTING_CHAR;
    }
}

PointBounds StructureSaver::FindStructureBounds()
{
    // Same across all chunks
    int pointsPerAxis = world.GetChunk(0, 0, 0).GetPointsPerAxis();
    PointBounds bounds;


    for (Chunk* chunk : world.GetAllChunks())
    {
        // Loop through chunks points
        for (int i = 0; i < chunk->GetPointAmount(); i++)
        {
            const Point& p = chunk->GetPointAt(i);

            // Ignore empty points
            if (p.density >= 1)
                continue;

            Index3D i3D = Get3DIndex(i, pointsPerAxis);
            Vector3 globalPos = chunk->GetGlobalPos({ (float)i3D.x, (float)i3D.y, (float)i3D.z });

            // Check if it should be the new min
            if (bounds.minPos.x < globalPos.x && bounds.minPos.y < globalPos.y && bounds.minPos.z < globalPos.z)
            {
                bounds.minPos = globalPos;

                Index3D i3D = Get3DIndex(i, pointsPerAxis);
                bounds.minIndex3D = i3D;
            }
            // Check if it should be the new max
            else if (bounds.maxPos.x > globalPos.x && bounds.maxPos.y > globalPos.y && bounds.maxPos.z > globalPos.z)
            {
                bounds.maxPos = globalPos;

                Index3D i3D = Get3DIndex(i, pointsPerAxis);
                bounds.maxIndex3D = i3D;
            }
        }
    }

    return bounds;
}

std::vector<float> StructureSaver::GetPointsToSave(PointBounds t_bounds)
{
    // Same across all chunks
    int pointsPerAxis = world.GetChunk(0, 0, 0).GetPointsPerAxis();
    std::vector<float> points;

    for (int x = t_bounds.minIndex3D.x; x < t_bounds.maxIndex3D.x; x++)
    {
        for (int y = t_bounds.minIndex3D.y; y < t_bounds.maxIndex3D.y; y++)
        {
            for (int z = t_bounds.minIndex3D.z; z < t_bounds.maxIndex3D.z; z++)
            {
                int i = GetFlatIndex(x, y, z, pointsPerAxis);
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
