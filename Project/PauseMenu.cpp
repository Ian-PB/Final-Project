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
		worldSeed = world.noiseSeed;
		noiseSeedEditMode = false;
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
	if (GuiButton(rect, "Exit Game (L)"))
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
	auto IncrementElement = [&]() {
		elementNum++;
		y = PANEL_START_POS.y + ELEMENT_SPACING * elementNum;
		rect = { x, y, w, h };
		};

	DrawText("General (Keybinds currently)", x, y, 30, TEXT_COLOR);
	DrawRectangle(x, y + 32, GuiGetTextWidth("General") * 3, 2, TEXT_COLOR);
	IncrementElement();

	// Player Stuff
	DrawText("WASD / Arrows - Move around", x, y, 20, TEXT_COLOR);
	IncrementElement();
	DrawText("Space - Jump", x, y, 20, TEXT_COLOR);
	IncrementElement();
	DrawText("Left Click - Shoot", x, y, 20, TEXT_COLOR);
	IncrementElement();
	DrawText("Right Click - Swap build / break", x, y, 20, TEXT_COLOR);
	IncrementElement();
	DrawText("Scroll wheel up/down - increase/decrease break/build size", x, y, 20, TEXT_COLOR);
	IncrementElement();

	// World stuff
	DrawText("C - Hollow Cube World", x, y, 20, TEXT_COLOR);
	IncrementElement();
	DrawText("G - Randomize world with noise", x, y, 20, TEXT_COLOR);
	IncrementElement();

	// Debug
	DrawText("F1 - Toggle collider visuals", x, y, 20, TEXT_COLOR);
	IncrementElement();
	DrawText("F2 - Swap between free flying and normal movement", x, y, 20, TEXT_COLOR);
	IncrementElement();
}

void PauseMenu::DrawPlayerSettings()
{
	int elementNum = 0;
	float x = PANEL_START_POS.x * 1.1f;
	float y = PANEL_START_POS.y;
	float w = ELEMENT_SIZE.x;
	float h = ELEMENT_SIZE.y;
	Rectangle rect = { x, y, w, h };

	// Setup helper lambda for increment the element
	auto IncrementElement = [&]() {
		elementNum++;
		y = PANEL_START_POS.y + ELEMENT_SPACING * elementNum;
		rect = { x, y, w, h };
		};

	// Helper to move over a column
	auto IncrementColumn = [&]() {
		elementNum = 1;
		x += ELEMENT_SPACING * 4.0f;
		y = PANEL_START_POS.y + ELEMENT_SPACING * elementNum;
		rect = { x, y, w, h };
		};

	DrawText("Player", x, y, 30, TEXT_COLOR);
	DrawRectangle(x, y + 32, GuiGetTextWidth("Player") * 3, 2, TEXT_COLOR);
	IncrementElement();

	// Jump force Spinner
	int playerJumpInt = player.jump.jumpForce * 10;
	GuiSpinner(rect, "Jump Force", &playerJumpInt, 0, 50, false);
	player.jump.jumpForce = playerJumpInt / 10.0f;
	IncrementElement();

	// Move speed Spinner
	int playerMoveSpeedInt = player.moveSpeed * 10;
	GuiSpinner(rect, "Move Speed", &playerMoveSpeedInt, 0, 100, false);
	player.moveSpeed = playerMoveSpeedInt / 10.0f;
	IncrementElement();

	// Gravity Spinner
	int playerGravityForceInt = player.gravityForce * 10;
	GuiSpinner(rect, "Gravity Force", &playerGravityForceInt, 0, 100, false);
	player.gravityForce = playerGravityForceInt / 10.0f;
	IncrementElement();

	// Max standable slope
	GuiSpinner(rect, "Max Standable Slope (DEG)", &player.MAX_SLOPE_ANGLE, 0, 180, false);
	IncrementElement();

	// Crouch speed mult
	int crouchSpeedMultInt = player.CROUCH_SPEED_MULT * 10;
	GuiSpinner(rect, "Crouch Speed Mult (Mult by 10)", &crouchSpeedMultInt, 0, 50, false);
	player.CROUCH_SPEED_MULT = crouchSpeedMultInt / 10.0f;
	IncrementElement();

	// Grounded Friction
	int groundedFrictionInt = player.GROUNDED_FRICTION * 100.0f;
	GuiSpinner(rect, "Grounded Friction (Mult by 100)", &groundedFrictionInt, 0, 200, false);
	player.GROUNDED_FRICTION = groundedFrictionInt / 100.0f;
	IncrementElement();

	// Sliding Friction
	int slidingFrictionInt = player.SLIDING_FRICTION * 100.0f;
	GuiSpinner(rect, "Sliding Friction (Mult by 100)", &slidingFrictionInt, 0, 200, false);
	player.SLIDING_FRICTION = slidingFrictionInt / 100.0f;
	IncrementElement();

	// Sliding Boost
	int slidingBoostInt = player.slideBoost * 10;
	GuiSpinner(rect, "Slide Boost (Mult by 10)", &slidingBoostInt, 0, 50, false);
	player.slideBoost = slidingBoostInt / 10.0f;
	IncrementElement();

	// Aerial Drag
	int aerialDragInt = player.AERIAL_DRAG * 100.0f;
	GuiSpinner(rect, "Aerial Drag (Mult by 100)", &aerialDragInt, 0, 200, false);
	player.AERIAL_DRAG = aerialDragInt / 100.0f;
	IncrementElement();

	// Max Velocity
	int maxVelocityInt = player.MAX_VELOCITY * 10;
	GuiSpinner(rect, "Max Velocity", &maxVelocityInt, 0, 20, false);
	player.MAX_VELOCITY = maxVelocityInt / 10.0f;
	IncrementElement();
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
	auto IncrementElement = [&]() {
		elementNum++;
		y = PANEL_START_POS.y + ELEMENT_SPACING * elementNum;
		rect = { x, y, w, h };
		};

	DrawText("World", x, y, 30, TEXT_COLOR);
	DrawRectangle(x, y + 32, GuiGetTextWidth("World") * 3, 2, TEXT_COLOR);
	IncrementElement();

	// Seed input
	if (GuiValueBox(rect, "Noise Seed", &worldSeed, 0, 9999, noiseSeedEditMode))
	{
		noiseSeedEditMode = !noiseSeedEditMode;

		if (!noiseSeedEditMode && worldSeed != world.noiseSeed) // Changed
		{
			world.noiseSeed = worldSeed;
			world.UpdatedSeed();
			world.UpdateAllDensities();
			world.completeRebuild = true;
		}
	}
	IncrementElement();

	// Surface Level Slider
	float prevLevel = world.surfaceLevel;
	GuiSliderBar(rect, "Solid", "Air", &world.surfaceLevel, -1, 1);
	if (prevLevel != world.surfaceLevel) // Changed
	{
		world.completeRebuild = true;
	}
	IncrementElement();

	// Frequency
	float prevFreq = world.frequency;
	GuiSliderBar(rect, "0", "5", &world.frequency, 0, 5);
	if (prevFreq != world.frequency) // Changed
	{
		world.SetupNoise();
		world.completeRebuild = true;
	}
	IncrementElement();
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
