#include "Player.h"
#include <raymath.h>
#include <string>
#include <iostream>

Player::Player(World& t_world) : WORLD(t_world), light(Lighting::CreateLight(ORANGE)), randLight(Lighting::CreateLight(WHITE))
{
    position = collider.GetPos();
    rayCollision.distance = 9999.0f;
    rayCollision.hit = false;
}

void Player::Init()
{
    Vector3 camOffset = { 0.0f, height * 0.8f, 0.0f };
    camera = std::make_shared<MainCamera>(position, camOffset);

    randLight.strength = 5.0f;
    collider.Setup(height, radius);
}

void Player::Update()
{

    // Enable / disable debug
    if (IsKeyReleased(KEY_F1))
        displayDebugElements = !displayDebugElements;

    camera->Update();
    Movement();

    if (IsMouseButtonReleased(0))
        ShootRay();
    if (IsMouseButtonReleased(1))
        breaking = !breaking;

    CollisionDetection();

    // Scroll to increase or decrease ray
    float scroll = GetMouseWheelMove();
    if (scroll > 0.0f)
        rayRadius += radiusChange;

    else if (scroll < 0.0f)
    {
        rayRadius -= radiusChange;
        if (rayRadius < 0.0f) // Clamp
            rayRadius = 0.0f;
    }
}

void Player::Draw()
{
    if (rayCollision.hit)
    {
        if (displayDebugElements)
        {
            DrawSphere(rayStart, 0.2f, { 230, 41, 55, 100 }); // Start point
            DrawSphere(rayCollision.point, 0.5f, { 230, 41, 55, 100 }); // End point
            DrawLine3D(rayStart, rayCollision.point, { 230, 41, 55, 100 });

            // Temp destruction sphere
            DrawSphereWires(rayCollision.point, rayRadius, 10, 10, RED);
        }

        DrawSphere(rayCollision.point, 0.2, randLight.color);
    }

    collider.Draw();
}

void Player::Draw2D()
{
    if (breaking)
        DrawText("Breaking", 0, 25, 16, RED);
    else
        DrawText("Building", 0, 25, 16, GREEN);

    std::string radiusText = "Radius: " + std::to_string(rayRadius);
    DrawText(radiusText.c_str(), 0, 42, 10, WHITE);
}

void Player::Movement()
{
    // Flattened so you dont go in the direction you're facing
    float yaw = camera->GetYaw();
    Vector3 flatForward = { sinf(yaw), 0.0f, cosf(yaw) };
    Vector3 right = { cosf(yaw), 0.0f, -sinf(yaw) };

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
    collider.SetPos(position);

    light.position = position;
}

void Player::ShootRay()
{
    Vector2 screenCenter =
    {
        GetScreenWidth() / 2.0f,
        GetScreenHeight() / 2.0f
    };
    // Use screenCenter instead of the mouse pos because the mouse is locked
    ray = GetScreenToWorldRay(screenCenter, camera->GetCameraRef());

    rayCollision.hit = false;
    rayCollision.distance = 9999.0f;
    rayStart = camera->GetPos();

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
    SetRandLight(rayCollision.point);

    // Destroy hitPos of mesh
    WORLD.EditSphere(closestHit, rayRadius, breaking);
}

void Player::CollisionDetection()
{
    BoundingBox bounds;
    bounds.min = { position.x - radius, position.y, position.z - radius };
    bounds.max = { position.x + radius, position.y + radius, position.z + radius };

    // Temp collision check
    std::vector<Chunk*> chunks= WORLD.GetChunksInBounds(bounds);

    for (const Chunk* chunk : chunks)
    {
        for (int i = 0; i < MAX_COLLISION_ITERATIONS; i++)
        {
            CollisionInfo collisionInfo = collider.CheckAgainstTriangles(chunk->GetCollider().GetTriangles());

            // Leave early if no collision
            if (!collisionInfo.hit)
                break;

            // Correct position
            Vector3 correction = Vector3Scale(collisionInfo.normal, collisionInfo.penetration);
            position = Vector3Add(position, correction);
            collider.SetPos(position);
        }
    }

    // Update camera after resolving collisions
    camera->UpdatePos();
}

void Player::SetRandLight(Vector3 pos)
{
    randLight.position = pos;

    int randColor = rand() % 5;

    switch (randColor)
    {
    case 0:
        randLight.color = RED;
        break;
    case 1:
        randLight.color = GREEN;
        break;
    case 2:
        randLight.color = BLUE;
        break;
    case 3:
        randLight.color = PURPLE;
        break;
    case 4:
        randLight.color = GOLD;
        break;
    }
}
