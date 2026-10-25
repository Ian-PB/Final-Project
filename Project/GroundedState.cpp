#include "GroundedState.h"
#include "Player.h"
#include <raymath.h>
#include <cmath>
#include <iostream>

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

    // Jump will take you out of the grounded state
    if (IsKeyPressed(KEY_SPACE) && p.jump.CheckJump(p.velocity))
    {
        p.isGrounded = false;
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
    Vector3 moveDir = { 0.0f, 0.0f, 0.0f };

    // Move Forward or Backward
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
    {
        moveDir = Vector3Add(moveDir, flatForward);
    }
    else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
    {
        moveDir = Vector3Subtract(moveDir, flatForward);
    }
    // Move Left or Right
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
    {
        moveDir = Vector3Add(moveDir, right);
    }
    else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
    {
        moveDir = Vector3Subtract(moveDir, right);
    }

    // Project the movement along the grounds normal
    Vector3 projectedMovement = Vector3Normalize(Vector3Reject(moveDir, p.groundNormal));
    projectedMovement = Vector3Scale(projectedMovement, speed);


    // Also project the current velocity along the ground normal
    p.velocity = Vector3Reject(p.velocity, p.groundNormal);
    // Add both projected together
    p.velocity = Vector3Add(p.velocity, projectedMovement);

    // If the player isn't moving and the slope is walkable
    if (moveDir == Vector3Zero() && OnWalkableSlope())
        p.velocity.y = 0.0f; // Don't slide down it

    // Apply velocity to position
    p.position = Vector3Add(p.position, p.velocity);

    // Friction
    p.velocity = Vector3Scale(p.velocity, p.GROUNDED_FRICTION);
}

bool GroundedState::OnWalkableSlope()
{
    float slopeAngle = Vector3Angle(p.groundNormal, p.UP) * RAD2DEG;
    if (slopeAngle < p.MAX_SLOPE_ANGLE)
        return true;

    return false;
}
