#include "Materials.h"

const std::map<MaterialTypes, Color> Materials::MATERIAL_TO_COLOR{
	{ MaterialTypes::Stone, GRAY },
	{ MaterialTypes::Grass, GREEN },
	{ MaterialTypes::Gold, GOLD }
};

const std::map<Color, MaterialTypes, ColorLess> Materials::COLOR_TO_MATERIAL{
	{ GRAY, MaterialTypes::Stone },
	{ GREEN, MaterialTypes::Grass },
	{ GOLD, MaterialTypes::Gold }
};