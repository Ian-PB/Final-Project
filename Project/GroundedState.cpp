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
    if (IsKeyPressed(KEY_SPACE))
    {
        p.jump.CheckJump(p.velocity);
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
    if (IsKeyDown(KEY_W))
    {
        moveDir = Vector3Add(moveDir, flatForward);
    }
    else if (IsKeyDown(KEY_S))
    {
        moveDir = Vector3Subtract(moveDir, flatForward);
    }
    // Move Left or Right
    if (IsKeyDown(KEY_A))
    {
        moveDir = Vector3Add(moveDir, right);
    }
    else if (IsKeyDown(KEY_D))
    {
        moveDir = Vector3Subtract(moveDir, right);
    }

    // Project the movement along the grounds normal
    Vector3 projectedMovement = Vector3Normalize(Vector3Reject(moveDir, p.groundNormal));
    projectedMovement = Vector3Scale(projectedMovement, speed);

    float slopeAngle = Vector3Angle(p.groundNormal, p.UP) * RAD2DEG;
    if (slopeAngle > p.MAX_SLOPE_ANGLE)
    {
        std::cout << "Slope angle: " << slopeAngle << std::endl;
        // Flatten the projectect movement to not allow player to move up the slope
        projectedMovement = { projectedMovement.x, 0.0f, projectedMovement.z };
    }

    p.velocity = Vector3Add(p.velocity, projectedMovement);

    p.position = Vector3Add(p.position, p.velocity);

    p.velocity.x *= p.GROUNDED_DRAG;
    //p.velocity.y *= p.GROUNDED_DRAG;
    p.velocity.z *= p.GROUNDED_DRAG;
}
