#include "Player.h"
#include <raymath.h>

Player::Player(const World& t_world) : WORLD(t_world)
{
    SetupCamera();
    rayCollision.distance = 9999.0f;
    rayCollision.hit = false;
}

void Player::Init()
{
}

void Player::Update()
{
    CameraLook();
    Movement();

    if (IsMouseButtonReleased(0))
        ShootRay();
}

void Player::Draw()
{
    if (rayCollision.hit)
    {
        DrawSphere(rayStart, 0.2f, { 230, 41, 55, 100 }); // Start point
        DrawSphere(rayCollision.point, 0.5f, { 230, 41, 55, 100 }); // End point
        DrawLine3D(rayStart, rayCollision.point, { 230, 41, 55, 100 });
    }
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

    Vector2 screenCenter =
    {
        GetScreenWidth() / 2.0f,
        GetScreenHeight() / 2.0f
    };
    // Use screenCenter instead of the mouse pos because the mouse is locked
    ray = GetScreenToWorldRay(screenCenter, camera);
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

void Player::ShootRay()
{
    rayCollision.hit = false;
    rayCollision.distance = 9999.0f;
    rayStart = position;

    // Get the chunks array from world
    Vector3 worldDimensions = WORLD.GetDimensions();
    std::vector<Vector3> hitPositions;

    for (int x = 0; x < worldDimensions.x; x++)
    {
        for (int y = 0; y < worldDimensions.y; y++)
        {
            for (int z = 0; z < worldDimensions.z; z++)
            {
                const Chunk& chunk = WORLD.GetChunk(x, y, z);
                // Get chunks transform position
                Matrix transform = MatrixTranslate(
                    chunk.GetPosition().x,
                    chunk.GetPosition().y,
                    chunk.GetPosition().z
                );

                RayCollision meshCollision = GetRayCollisionMesh(ray, chunk.GetMesh(), transform);

                // Check if in range and hit
                if (meshCollision.hit && (meshCollision.distance < rayCollision.distance))
                {
                    rayCollision.hit = true;
                    // Add to hit positions
                    hitPositions.push_back(meshCollision.point);
                }
            }
        }
    }

    // Find which point is the closest
    float closestHitDist = 9999.0f;
    Vector3 closestHit = rayStart; // Set to the same point until changed
    for (Vector3 hitPos : hitPositions)
    {
        float dist = Vector3Distance(rayStart, hitPos);
        if (Vector3Distance(rayStart, hitPos) < closestHitDist)
        {
            closestHit = hitPos;
            closestHitDist = dist;
        }
    }

    rayCollision.point = closestHit;
}
