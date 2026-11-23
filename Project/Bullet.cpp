#include "Bullet.h"
#include <raylib.h>
#include <raymath.h>
#include <vector>

#include "PlayersManager.h"
#include "CapsuleCollider.h"

Bullet::Bullet(int t_owner, World& t_world) : NetworkObject(t_owner), WORLD(t_world)
{
    position = { 0.0f, 0.0f, 0.0f };

    collider.Setup(radius, position);
}

void Bullet::Draw()
{
    if (!active)
        return;

    DrawSphere(position, radius, RED);
}

void Bullet::Update()
{
    // Reset hits
    hitPosition = { 0.0f, 0.0f, 0.0f };
    hasCollided = false;

    if (!isOwner)
        return;

    if (!active)
        return;

    // Movement
    position = Vector3Add(position, velocity * GetFrameTime());
    collider.SetPos(position);

    UpdateOccupiedChunks();

    CollisionDetection();
}

void Bullet::Shoot(Vector3 t_pos, Vector3 t_dir)
{
    active = true;

    position = t_pos;
    velocity = t_dir * speed;

    collider.SetPos(position);
    UpdateOccupiedChunks();
}

std::vector<char> Bullet::Serialise()
{
    std::vector<char> buffer;

    auto write = [&](const void* data, size_t size) {
        const char* bytes = reinterpret_cast<const char*>(data);
        buffer.insert(buffer.end(), bytes, bytes + size);
        };

    // Dont send anything if not active and has not collided
    if (!active && !hasCollided)
        return buffer;

    SerialiseBase(buffer);

    uint32_t len = 0;

    // Active
    write(&active, sizeof(active));

    // Transform
    write(&position, sizeof(position));

    // Shooting
    write(&hasCollided, sizeof(hasCollided));
    if (hasCollided) // Only send info if needed
    {
        write(&hitPosition, sizeof(hitPosition));
        write(&explosionRadius, sizeof(explosionRadius));
        write(&breaking, sizeof(breaking));
    }

    // Set true length of buffer
    len = static_cast<uint32_t>(buffer.size());
    memcpy(buffer.data(), &len, sizeof(len));

    return buffer;
}

NetworkObject* Bullet::UpdateFromDeserialisation(const char* t_data)
{
    size_t offset = 0;

    auto read = [&](void* out, size_t s) {
        memcpy(out, t_data + offset, s);
        offset += s;
        };

    // Active
    read(&active, sizeof(active));

    // Transform
    read(&position, sizeof(position));

    // Shooting
    read(&hasCollided, sizeof(hasCollided));
    if (hasCollided) // Only read if it was sent basicly
    {
        read(&hitPosition, sizeof(hitPosition));
        read(&explosionRadius, sizeof(explosionRadius));
        read(&breaking, sizeof(breaking));
        // Edit at pos
        OnHit();
    }

    return this;
}

void Bullet::CollisionDetection()
{
    // Player collision
    for (const std::shared_ptr<Player> player : PlayersManager::GetPlayers())
    {
        // Check if not itself
        if (player->GetOwner() == owner)
            continue;

        CapsuleCollider playerCollider = player->GetCollider();
        CollisionInfo collisionInfo = playerCollider.CheckAgainstSphere(position, radius);

        // Hit found
        if (collisionInfo.hit)
        {
            player->TakeDamage(damage);

            OnHit();
            return;
        }
    }

    // World collision
    for (const Chunk* chunk : occupiedChunks)
    {
        for (int i = 0; i < MAX_COLLISION_ITERATIONS; i++)
        {
            CollisionInfo collisionInfo = collider.CheckAgainstTriangles(chunk->GetCollider().GetTriangles());

            // Leave early if no collision
            if (!collisionInfo.hit)
                break;

            hasCollided = true;
            OnHit();

            return;
        }
    }
}

void Bullet::OnHit()
{
    active = false;

    WORLD.EditSphere(position, explosionRadius, breaking);
}

void Bullet::UpdateOccupiedChunks()
{
    BoundingBox bounds;
    bounds.min = { position.x - radius, position.y - radius, position.z - radius };
    bounds.max = { position.x + radius, position.y + radius, position.z + radius };

    // Save chunks that are occupied
    occupiedChunks.clear();
    occupiedChunks = WORLD.GetChunksInBounds(bounds);
}
