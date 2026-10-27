#pragma once
#include <memory>
#include <raylib.h>
#include "raygui.h"

class Player;
class World;

class PauseMenu
{
public:
	PauseMenu(Player& t_player, World& t_world);
	void Draw();

	void ToggleActive();
	bool IsActive() { return active; }

private:
	// Enum for options
	enum class PanelOptions
	{
		General,
		Player,
		World
	};
	PanelOptions currentPanel = PanelOptions::General;

	// Pointers to editables
	Player& player;
	World& world;

	int IncrementBy(int t_amount, float& t_newPos);

	void DrawSideMenu();
	void DrawGeneralSettings();
	void DrawPlayerSettings();
	void DrawWorldSettings();

	bool active = false;

	Rectangle backgroundRect;

	const Color MAIN_COLOR = DARKGRAY;
	const Color OUTLINE_COLOR = GRAY;
	const Color TEXT_COLOR = LIGHTGRAY;

	const float EDGE_OFFSET_SCALE = 0.05f;
	const float BACKGROUND_SIZE_SCALE = 0.9f;
	// Button constants
	const float BUTTON_SPACING_SCALE = 0.2f;
	const Vector2 BUTTON_START_POS_SCALE = { 0.02f, 0.04f };
	const Vector2 BUTTON_SIZE_SCALE = { 0.2f, 0.1f };
	void InitSemiConsts();
	Vector2 BUTTON_START_POS = { 0.0f, 0.0f };
	Vector2 BUTTON_SIZE = { 0.0f, 0.0f };
	float BUTTON_SPACING = 0.0f;
	float MENU_WIDTH = 0.0f;
	
	// Panel constants
	Vector2 PANEL_START_POS = { 0.0f, 0.0f };
	const float ELEMENT_SPACING_SCALE = 0.08f;
	const Vector2 ELEMENT_SIZE_SCALE = { 0.1f, 0.05f };
	Vector2 ELEMENT_SIZE = { 0.0f, 0.0f };
	float ELEMENT_SPACING = 0.0f;

	// Panel Values
	int worldSeed = 0;
	bool noiseSeedEditMode = false;
};

#include "Player.h"
#include "World.h"