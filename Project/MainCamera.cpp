#include "MainCamera.h"

MainCamera::MainCamera(Vector3& t_followPosition, float t_heightOffset) : followPosition(t_followPosition), heightOffset(t_heightOffset)
{
    camera.position = GetPos();                         // Camera position
    camera.target = { 0.0f, 2.0f, 0.0f };               // Camera looking at point
    camera.up = { 0.0f, 1.0f, 0.0f };                   // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    // Camera initially faces towards -Z
    cameraYaw = PI;
    cameraPitch = 0.0f;

    DisableCursor();
}

void MainCamera::UpdatePos()
{
    camera.position = Vector3Add(followPosition, positionOffset);
    camera.target = Vector3Add(camera.position, forward);
}

void MainCamera::Update()
{
    camera.position = Vector3Add(followPosition, positionOffset);

    Vector2 mouseDelta = GetMouseDelta();

    cameraYaw -= mouseDelta.x * cameraSensitivity;
    cameraPitch -= mouseDelta.y * cameraSensitivity;

    // Dont allow the camera to flip upside down
    cameraPitch = Clamp(cameraPitch, -1.5f, 1.5f);

    forward = { cosf(cameraPitch) * sinf(cameraYaw), sinf(cameraPitch), cosf(cameraPitch) * cosf(cameraYaw) };
    camera.target = Vector3Add(camera.position, forward); // Face in new direction (forward)
}

void MainCamera::SetHeight(float t_height)
{
    height = t_height; 
    positionOffset.y = height * heightOffset;
}
