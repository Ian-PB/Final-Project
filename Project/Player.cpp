#include "Player.h"
#include <raymath.h>
#include <string>
#include <iostream>

Player::Player(World& t_world) : WORLD(t_world), light(Lighting::CreateLight(ORANGE)), randLight(Lighting::CreateLight(WHITE))
{
    position = collider.GetPos();
    shootRayCollision.distance = 9999.0f;
    shootRayCollision.hit = false;

    for (int i = 0; i < GROUND_CHECK_RAYS_AMOUNT; i++)
        groundCheckRays[i].direction = {0.0f, -1.0f, 0.0f};

    groundCheckRaysOffsets[0] = { 0.0f, 0.0f, 0.0f };
    groundCheckRaysOffsets[1] = { radius, 0.0f, 0.0f };
    groundCheckRaysOffsets[2] = { -radius, 0.0f, 0.0f };
    groundCheckRaysOffsets[3] = { 0.0f, 0.0f, radius };
    groundCheckRaysOffsets[4] = { 0.0f, 0.0f, -radius };

    aboveCheckRay.direction = { 0.0f, 1.0f, 0.0f };
}

void Player::Init()
{
    rayHeightDist = height / 2.0f + RAY_EXTENTION;

    Vector3 camOffset = { 0.0f, height * 0.8f, 0.0f };
    camera = std::make_shared<MainCamera>(position, camOffset);

    randLight.strength = 5.0f;
    light.strength = 25.0f;
    collider.Setup(height, radius);
}

void Player::Update()
{
    // Enable / disable debug
    if (IsKeyReleased(KEY_F1))
        displayDebugElements = !displayDebugElements;
    else if (IsKeyReleased(KEY_F2))
        freeFly = !freeFly;

    UpdateOccupiedChunks();

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
    if (shootRayCollision.hit)
    {
        if (displayDebugElements)
        {
            DrawSphere(shootRayStart, 0.2f, { 230, 41, 55, 100 }); // Start point
            DrawSphere(shootRayCollision.point, 0.5f, { 230, 41, 55, 100 }); // End point
            DrawLine3D(shootRayStart, shootRayCollision.point, { 230, 41, 55, 100 });

            // Temp destruction sphere
            DrawSphereWires(shootRayCollision.point, rayRadius, 10, 10, RED);
        }

        DrawSphere(shootRayCollision.point, 0.2, randLight.color);
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

    DrawText("F2 toggle flying", 0, 55, 10, BLUE);
    std::string posString = "Pos: " + std::to_string(position.x) + ", " + std::to_string(position.y) + ", " + std::to_string(position.z);
    DrawText(posString.c_str(), 0, 67, 10, WHITE);
    std::string velocityString = "Velocity: " + std::to_string(velocity.x) + ", " + std::to_string(velocity.y) + ", " + std::to_string(velocity.z);
    DrawText(velocityString.c_str(), 0, 80, 10, WHITE);

    if (jump.canJump)
        DrawText("canJump", 0, 100, 16, GREEN);
    else
        DrawText("canJump", 0, 100, 16, RED);

    if (isGrounded)
        DrawText("isGrounded", 0, 120, 16, GREEN);
    else
        DrawText("isGrounded", 0, 120, 16, RED);
}

void Player::UpdatePositions()
{
    // Collider
    collider.SetPos(position);

    // Rays
    Vector3 bodyCenter = { position.x, position.y + height / 2.0f, position.z };

    for (int i = 0; i < GROUND_CHECK_RAYS_AMOUNT; i++)
        groundCheckRays[i].position = Vector3Add(bodyCenter, groundCheckRaysOffsets[i]);

    aboveCheckRay.position = bodyCenter;

    // Light
    light.position = bodyCenter;
}

void Player::Movement()
{

    if (freeFly)
        FreeFly();
    else
    {
        RayChecks();
        NormalMovement();
    }

    UpdatePositions();
}

void Player::NormalMovement()
{
    float deltaTime = GetFrameTime();
    // Flattened so you dont go in the direction you're facing
    float yaw = camera->GetYaw();
    Vector3 flatForward = { sinf(yaw), 0.0f, cosf(yaw) };
    Vector3 right = { cosf(yaw), 0.0f, -sinf(yaw) };

    // Make speed not based off FPS
    float speed = moveSpeed * deltaTime;

    // Move Forward or Backward
    if (IsKeyDown(KEY_W))
    {
        velocity += Vector3Scale(flatForward, speed);
    }
    else if (IsKeyDown(KEY_S))
    {
        velocity -= Vector3Scale(flatForward, speed);
    }
    // Move Left or Right
    if (IsKeyDown(KEY_A))
    {
        velocity += Vector3Scale(right, speed);
    }
    else if (IsKeyDown(KEY_D))
    {
        velocity -= Vector3Scale(right, speed);
    }

    if (IsKeyPressed(KEY_SPACE))
    {
        jump.CheckJump(velocity);
    }

    // Gravity
    if (!isGrounded)
    {
        velocity.y -= gravityForce * deltaTime;
        if (velocity.y < MAX_DOWNWARD_FORCE)
            velocity.y = MAX_DOWNWARD_FORCE;
    }

    position = Vector3Add(position, velocity);

    velocity.x *= DRAG;
    velocity.z *= DRAG;
}

void Player::RayChecks()
{
    isGrounded = false;

    for (const Chunk* chunk : occupiedChunks)
    {
        std::vector<Triangle> tris = chunk->GetCollider().GetTriangles();

        for (const Triangle& t : tris)
        {
            GroundCheck(t);
            CeilingCheck(t);
        }
    }
}

void Player::CeilingCheck(const Triangle& tri)
{
    RayCollision ray = GetRayCollisionTriangle(aboveCheckRay, tri.a, tri.b, tri.c);
    if (ray.hit && ray.distance <= rayHeightDist) // Hit roof
    {
        // Stop upward momentum of roof hit
        velocity.y = 0.0f;
        jump.isJumping = false;
    }
}

void Player::GroundCheck(const Triangle& tri)
{
    for (int i = 0; i < GROUND_CHECK_RAYS_AMOUNT; i++)
    {
        RayCollision groundRay = GetRayCollisionTriangle(groundCheckRays[i], tri.a, tri.b, tri.c);

        if (groundRay.hit && groundRay.distance <= rayHeightDist) // Is groundeed
        {
            // Refresh jump
            isGrounded = true;
            jump.canJump = true;

            // Only if moving downwards
            if (velocity.y < 0.0f)
            {
                velocity.y = 0.0f;
            }

            float normalVelocity = Vector3DotProduct(velocity, groundRay.normal);

            if (normalVelocity < 0.0f)
            {
                velocity = Vector3Reject(velocity, groundRay.normal);
            }

            // 1 collision found
            break;
        }
    }
}

void Player::UpdateOccupiedChunks()
{
    BoundingBox bounds;
    bounds.min = { position.x - radius, position.y, position.z - radius };
    bounds.max = { position.x + radius, position.y + radius, position.z + radius };

    // Save chunks that are occupied
    occupiedChunks.clear();
    occupiedChunks = WORLD.GetChunksInBounds(bounds);
}

void Player::FreeFly()
{
    // Flattened so you dont go in the direction you're facing
    float yaw = camera->GetYaw();
    Vector3 flatForward = { sinf(yaw), 0.0f, cosf(yaw) };
    Vector3 right = { cosf(yaw), 0.0f, -sinf(yaw) };

    // Make speed not based off FPS
    float speed = 10.0f * GetFrameTime();

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
}

void Player::ShootRay()
{
    Vector2 screenCenter =
    {
        GetScreenWidth() / 2.0f,
        GetScreenHeight() / 2.0f
    };
    // Use screenCenter instead of the mouse pos because the mouse is locked
    shootRay = GetScreenToWorldRay(screenCenter, camera->GetCameraRef());

    shootRayCollision.hit = false;
    shootRayCollision.distance = 9999.0f;
    shootRayStart = camera->GetPos();

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

                RayCollision meshCollision = GetRayCollisionMesh(shootRay, chunk.GetMesh(), transform);

                // Check if in range and hit
                if (meshCollision.hit && (meshCollision.distance < shootRayCollision.distance))
                {
                    shootRayCollision.hit = true;
                    // Add to hit positions
                    hitPositions.push_back(meshCollision.point);
                }
            }
        }
    }

    // Find which point is the closest
    float closestHitDist = 9999.0f;
    Vector3 closestHit = shootRayStart; // Set to the same point until changed
    for (Vector3 hitPos : hitPositions)
    {
        float dist = Vector3Distance(shootRayStart, hitPos);
        if (Vector3Distance(shootRayStart, hitPos) < closestHitDist)
        {
            closestHit = hitPos;
            closestHitDist = dist;
        }
    }

    shootRayCollision.point = closestHit;
    SetRandLight(shootRayCollision.point);

    // Destroy hitPos of mesh
    WORLD.EditSphere(closestHit, rayRadius, breaking);
}

void Player::CollisionDetection()
{
    for (const Chunk* chunk : occupiedChunks)
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

            // Project velocity along normal of collision
            float normalVelocity = Vector3DotProduct(velocity, collisionInfo.normal);

            if (normalVelocity < 0.0f)
            {
                velocity = Vector3Reject(velocity, collisionInfo.normal);
            }
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

bool Player::JumpInfo::CheckJump(Vector3& t_velocity)
{
    if (canJump)
    {
        canJump = false;
        t_velocity.y += jumpForce;
        isJumping = true;

        return true;
    }

    return false;
}
