#include "PauseMenu.h"
#include <raygui.h>


PauseMenu::PauseMenu(Player& t_player, World& t_world) : player(t_player), world(t_world)
{
	GuiSetStyle(VALUEBOX, TEXT_COLOR_NORMAL, 0xFFFFFFFF);
	GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, 0xFFFFFFFF);

	backgroundRect.x = GetScreenWidth() * EDGE_OFFSET_SCALE;
	backgroundRect.y = GetScreenHeight() * EDGE_OFFSET_SCALE;

	backgroundRect.width = GetScreenWidth() * BACKGROUND_SIZE_SCALE;
	backgroundRect.height = GetScreenHeight() * BACKGROUND_SIZE_SCALE;

	// Setup semi-constants
	InitSemiConsts();
}

void PauseMenu::Draw()
{
	if (!active)
		return;

	DrawRectangleRounded(backgroundRect, 0.1f, 10, MAIN_COLOR);
	DrawRectangleRoundedLines(backgroundRect, 0.1f, 10, OUTLINE_COLOR);

	DrawSideMenu();

	switch (currentPanel)
	{
	case PanelOptions::General:
		DrawGeneralSettings();
		break;
	case PanelOptions::Player:
		DrawPlayerSettings();
		break;
	case PanelOptions::World:
		DrawWorldSettings();
		break;
	}
}

void PauseMenu::ToggleActive()
{
	active = !active;

	if (active)
	{
		EnableCursor();
	}
	else
	{
		DisableCursor();
	}
}

void PauseMenu::DrawSideMenu()
{
	int buttonNum = 0;
	// Draw side buttons
	float x = BUTTON_START_POS.x;
	float y = BUTTON_START_POS.y + BUTTON_SPACING * buttonNum;
	float w = BUTTON_SIZE.x;
	float h = BUTTON_SIZE.y;
	Rectangle rect = { x, y, w, h };

	// Setup helper lambda for increment the button
	auto incrementButton = [&]() {
		buttonNum++;
		y = BUTTON_START_POS.y + BUTTON_SPACING * buttonNum;
		rect = { x, y, w, h };
		};


	// Seperation bar
	DrawRectangle(MENU_WIDTH, backgroundRect.y, 2.0f, backgroundRect.height, OUTLINE_COLOR);

	if (GuiButton(rect, "General"))
	{
		currentPanel = PanelOptions::General;
	}
	incrementButton();

	// Set button variables
	if (GuiButton(rect, "Player"))
	{
		currentPanel = PanelOptions::Player;
	}
	incrementButton();

	// Set button variables
	if (GuiButton(rect, "World"))
	{
		currentPanel = PanelOptions::World;
	}
	incrementButton();

	// Should be last
	if (GuiButton(rect, "Exit Game (F10)"))
	{	
		CloseWindow();
		return;
	}
}

void PauseMenu::DrawGeneralSettings()
{
	int elementNum = 0;
	float x = PANEL_START_POS.x;
	float y = PANEL_START_POS.y;
	float w = ELEMENT_SIZE.x;
	float h = ELEMENT_SIZE.y;
	Rectangle rect = { x, y, w, h };

	// Setup helper lambda for increment the element
	auto incrementElement = [&]() {
		elementNum++;
		y = PANEL_START_POS.y + ELEMENT_SPACING * elementNum;
		rect = { x, y, w, h };
		};

	DrawText("General", x, y, 30, TEXT_COLOR);
	DrawRectangle(x, y + 32, GuiGetTextWidth("General") * 3, 2, TEXT_COLOR);
}

void PauseMenu::DrawPlayerSettings()
{
	int elementNum = 0;
	float x = PANEL_START_POS.x;
	float y = PANEL_START_POS.y;
	float w = ELEMENT_SIZE.x;
	float h = ELEMENT_SIZE.y;
	Rectangle rect = { x, y, w, h };

	// Setup helper lambda for increment the element
	auto incrementElement = [&]() {
		elementNum++;
		y = PANEL_START_POS.y + ELEMENT_SPACING * elementNum;
		rect = { x, y, w, h };
		};

	DrawText("Player", x, y, 30, TEXT_COLOR);
	DrawRectangle(x, y + 32, GuiGetTextWidth("Player") * 3, 2, TEXT_COLOR);
	incrementElement();

	// Jump force Spinner
	int playerJumpInt = player.jump.jumpForce * 10;
	GuiSpinner(rect, "Jump Force", &playerJumpInt, 0, 50, false);
	player.jump.jumpForce = playerJumpInt / 10.0f;
	incrementElement();

	// Move speed Spinner
	int playerMoveSpeedInt = player.moveSpeed * 10;
	GuiSpinner(rect, "Move Speed", &playerMoveSpeedInt, 0, 100, false);
	player.moveSpeed = playerMoveSpeedInt / 10.0f;
	incrementElement();

	// Gravity Spinner
	int playerGravityForceInt = player.gravityForce * 10;
	GuiSpinner(rect, "Gravity Force", &playerGravityForceInt, 0, 100, false);
	player.gravityForce = playerGravityForceInt / 10.0f;
	incrementElement();
}

void PauseMenu::DrawWorldSettings()
{
	int elementNum = 0;
	float x = PANEL_START_POS.x;
	float y = PANEL_START_POS.y;
	float w = ELEMENT_SIZE.x;
	float h = ELEMENT_SIZE.y;
	Rectangle rect = { x, y, w, h };

	// Setup helper lambda for increment the element
	auto incrementElement = [&]() {
		elementNum++;
		y = PANEL_START_POS.y + ELEMENT_SPACING * elementNum;
		rect = { x, y, w, h };
		};

	DrawText("World", x, y, 30, TEXT_COLOR);
	DrawRectangle(x, y + 32, GuiGetTextWidth("World") * 3, 2, TEXT_COLOR);
	incrementElement();

	// Surface Level Slider
	float prevLevel = world.surfaceLevel;
	GuiSliderBar(rect, "Solid", "Air", &world.surfaceLevel, -1, 1);
	if (prevLevel != world.surfaceLevel) // Changed
	{
		world.completeRebuild = true;
	}
	incrementElement();
}

void PauseMenu::InitSemiConsts()
{
	// Side buttons
	BUTTON_START_POS = { backgroundRect.x + (backgroundRect.width * BUTTON_START_POS_SCALE.x), backgroundRect.y + (backgroundRect.height * BUTTON_START_POS_SCALE.y) };
	BUTTON_SIZE = { backgroundRect.width * BUTTON_SIZE_SCALE.x, backgroundRect.height * BUTTON_SIZE_SCALE.y };
	BUTTON_SPACING = (BUTTON_SPACING_SCALE * backgroundRect.height);
	MENU_WIDTH = backgroundRect.x + BUTTON_SIZE.x + (BUTTON_SPACING / 2.0f);
	// Panels
	PANEL_START_POS = { MENU_WIDTH * 1.2f, BUTTON_START_POS.y };
	ELEMENT_SIZE = { backgroundRect.width * ELEMENT_SIZE_SCALE.x, backgroundRect.height * ELEMENT_SIZE_SCALE.y };
	ELEMENT_SPACING = (ELEMENT_SPACING_SCALE * backgroundRect.height);
}
