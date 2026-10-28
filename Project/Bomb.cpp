#include "Bomb.h"

Bomb::Bomb(World& t_world) : WORLD(t_world)
{
	position = { 0.0f, 0.0f, 0.0f };

    collider.Setup(radius, position);
}

void Bomb::Draw()
{
    if (!active)
        return;

	DrawSphere(position, radius, RED);
}

void Bomb::Update()
{
    if (!active)
        return;

    velocity.y -= 0.01f;

    // Move
    position = Vector3Add(position, velocity);
    collider.SetPos(position);

    UpdateOccupiedChunks();

    CollisionDetection();

    // Drag
    velocity = Vector3Scale(velocity, 0.98f);
}

void Bomb::Throw(Vector3 t_pos, Vector3 t_vel)
{
    active = true;

    position = t_pos;
    velocity = t_vel;

    collider.SetPos(position);
    UpdateOccupiedChunks();
}

void Bomb::Explode()
{
    WORLD.EditSphere(position, explosionRadius, true);
}

void Bomb::CollisionDetection()
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

            // Remove velocity into the surface
            float velocityIntoNormal = Vector3DotProduct(velocity, collisionInfo.normal);
            if (velocityIntoNormal < 0.0f)
            {
                velocity = Vector3Subtract(velocity, Vector3Scale(collisionInfo.normal, velocityIntoNormal));
            }

            position = Vector3Add(position, correction);
            collider.SetPos(position);
        }
    }
}

void Bomb::UpdateOccupiedChunks()
{
    BoundingBox bounds;
    bounds.min = { position.x - radius, position.y - radius, position.z - radius };
    bounds.max = { position.x + radius, position.y + radius, position.z + radius };

    // Save chunks that are occupied
    occupiedChunks.clear();
    occupiedChunks = WORLD.GetChunksInBounds(bounds);
}
