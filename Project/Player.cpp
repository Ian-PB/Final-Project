#include "Player.h"
#include <raymath.h>

void Player::Init()
{
	SetupCamera();
}

void Player::Update()
{
    CameraLook();
    Movement();
}

void Player::SetupCamera()
{
    position = { 0.0f, 2.0f, 4.0f };
    camera.position = position;                         // Camera position
    camera.target = { 0.0f, 2.0f, 0.0f };               // Camera looking at point
    camera.up = { 0.0f, 1.0f, 0.0f };                   // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    // Camera initially faces towards -Z
    cameraYaw = PI;
    cameraPitch = 0.0f;

    DisableCursor();
}

void Player::CameraLook()
{
    Vector2 mouseDelta = GetMouseDelta();

    cameraYaw -= mouseDelta.x * cameraSensitivity;
    cameraPitch -= mouseDelta.y * cameraSensitivity;

    // Dont allow the camera to flip upside down
    cameraPitch = Clamp(cameraPitch, -1.5f, 1.5f);

    forward = { cosf(cameraPitch) * sinf(cameraYaw), sinf(cameraPitch), cosf(cameraPitch) * cosf(cameraYaw) };
    camera.target = Vector3Add(position, forward); // Face in new direction (forward)
}

void Player::Movement()
{
    // Flattened so you dont go in the direction you're facing
    Vector3 flatForward = { sinf(cameraYaw), 0.0f, cosf(cameraYaw) };
    Vector3 right = { cosf(cameraYaw), 0.0f, -sinf(cameraYaw) };

    // Make speed not based off FPS
    float speed = moveSpeed * GetFrameTime();

    // Move Forward or Backward
    if (IsKeyDown(KEY_W))
    {
        position = Vector3Add(position, Vector3Scale(flatForward, speed));
    }
    else if (IsKeyDown(KEY_S))
    {
        position = Vector3Subtract(position, Vector3Scale(flatForward, speed));
    }
    // Move Left or Right
    if (IsKeyDown(KEY_A))
    {
        position = Vector3Add(position, Vector3Scale(right, speed));
    }
    else if (IsKeyDown(KEY_D))
    {
        position = Vector3Subtract(position, Vector3Scale(right, speed));
    }

    // Move Up or Down
    if (IsKeyDown(KEY_SPACE))
    {
        position.y += speed;
    }
    else if (IsKeyDown(KEY_LEFT_SHIFT))
    {
        position.y -= speed;
    }

    // Set position to the camera
    camera.position = position;
    camera.target = Vector3Add(camera.position, forward);
}
