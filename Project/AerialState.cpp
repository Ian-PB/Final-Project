#include "AerialState.h"
#include "Player.h"
#include <raymath.h>
#include <cmath>

AerialState::AerialState(Player& t_player) : PlayerState(t_player)
{
}

void AerialState::Update()
{
    // Leave state when not grounded to aerial state
    if (p.isGrounded)
    {
        p.SwapState(p.groundedState);
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
    // Only possible with multiple jumps
    if (IsKeyPressed(KEY_SPACE))
    {
        p.jump.CheckJump(p.velocity);
    }

    // Gravity
    p.velocity.y -= p.gravityForce * deltaTime;

    // Clamp Velocity
    if (Vector3Length(p.velocity) > p.MAX_VELOCITY)
    {
        p.velocity = Vector3Scale(Vector3Normalize(p.velocity), p.MAX_VELOCITY);
    }

    p.position = Vector3Add(p.position, p.velocity);

    p.velocity.x *= p.AERIAL_DRAG;
    p.velocity.z *= p.AERIAL_DRAG;
}
