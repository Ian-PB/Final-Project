#include "SlidingState.h"

SlidingState::SlidingState(Player& t_player) : PlayerState(t_player)
{

}

void SlidingState::Update()
{
    // Leave state when not grounded to aerial state
    if (!p.isGrounded)
    {
        p.SwapState(p.aerialState);
        return;
    }

    float currentSpeed = Vector3Length(p.velocity);
    // If the slide key is released then swap to grounded
    // Or if the speed is low enough
    if (IsKeyReleased(KEY_LEFT_SHIFT) || currentSpeed < 0.01f)
    {
        p.SwapState(p.groundedState);
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
    float speed = (p.moveSpeed * p.SLIDING_MOVEMENT_STRENGTH) * deltaTime;
    Vector3 moveDir = { 0.0f, 0.0f, 0.0f };

    // Move Forward or Backward
    //if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
    //{
    //    moveDir = Vector3Add(moveDir, flatForward);
    //}
    //else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
    //{
    //    moveDir = Vector3Subtract(moveDir, flatForward);
    //}
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

    // Gravity
    p.velocity.y -= p.gravityForce * deltaTime;

    // Also project the current velocity along the ground normal
    p.velocity = Vector3Reject(p.velocity, p.groundNormal);
    // Add both projected together
    p.velocity = Vector3Add(p.velocity, projectedMovement);

    // Clamp Velocity
    if (Vector3Length(p.velocity) > p.MAX_VELOCITY)
    {
        p.velocity = Vector3Scale(Vector3Normalize(p.velocity), p.MAX_VELOCITY);
    }

    // Apply velocity to position
    p.position = Vector3Add(p.position, p.velocity);

    // Friction
    p.velocity.x *= p.SLIDING_FRICTION;
    p.velocity.z *= p.SLIDING_FRICTION;
}

void SlidingState::OnEnterState()
{
    float currentSpeed = Vector3Length(p.velocity);
    float boostedSpeed = currentSpeed * p.slideBoost;

    p.velocity = Vector3Scale(Vector3Normalize(p.velocity), boostedSpeed);

    // Reduce height
    p.height = p.SLIDING_HEIGHT;
    p.collider.Setup(p.height, p.radius);

    p.camera->SetHeight(p.height);
    p.camera->UpdatePos();
}