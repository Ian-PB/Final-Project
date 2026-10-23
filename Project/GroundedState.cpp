#include "GroundedState.h"
#include "Player.h"
#include <raymath.h>
#include <cmath>

GroundedState::GroundedState(Player& t_player) : PlayerState(t_player)
{
}

void GroundedState::Update()
{
    // Leave state when not grounded to aerial state
    if (!p.isGrounded)
    {
		p.SwapState(p.aerialState);
        return;
    }

    float deltaTime = GetFrameTime();
    // Flattened so you dont go in the direction you're facing
    float yaw = p.camera->GetYaw();
    Vector3 flatForward = { sinf(yaw), 0.0f, cosf(yaw) };
    Vector3 right = { cosf(yaw), 0.0f, -sinf(yaw) };

    // Make speed not based off FPS
    float speed = p.moveSpeed * deltaTime;

    // Move Forward or Backward
    if (IsKeyDown(KEY_W))
    {
        p.velocity += Vector3Scale(flatForward, speed);
    }
    else if (IsKeyDown(KEY_S))
    {
        p.velocity -= Vector3Scale(flatForward, speed);
    }
    // Move Left or Right
    if (IsKeyDown(KEY_A))
    {
        p.velocity += Vector3Scale(right, speed);
    }
    else if (IsKeyDown(KEY_D))
    {
        p.velocity -= Vector3Scale(right, speed);
    }

    if (IsKeyPressed(KEY_SPACE))
    {
        p.jump.CheckJump(p.velocity);
    }

    p.position = Vector3Add(p.position, p.velocity);

    p.velocity.x *= p.GROUNDED_DRAG;
    p.velocity.z *= p.GROUNDED_DRAG;
}
