#include "StructureData.h"

const std::string StructureData::FILE_PATH = "resources/Structures/";
const std::map<StructureTypes, std::string> StructureData::typeToFilePath = {
    { StructureTypes::None, "None/" },
    { StructureTypes::World, "Worlds/" }
};
