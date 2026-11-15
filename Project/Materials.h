#pragma once
#include <raylib.h>
#include <map>

// Needed cause theres no < opertation for Color for use in a map
struct ColorLess
{
    bool operator()(const Color& a, const Color& b) const
    {
        if (a.r != b.r) return a.r < b.r;
        if (a.g != b.g) return a.g < b.g;
        if (a.b != b.b) return a.b < b.b;
        return a.a < b.a;
    }
};

enum class MaterialTypes
{
	Stone,
	Grass,
	Gold
};

class Materials
{
public:
	static const std::map<MaterialTypes, Color> MATERIAL_TO_COLOR;
	static const std::map<Color, MaterialTypes, ColorLess> COLOR_TO_MATERIAL;
};

