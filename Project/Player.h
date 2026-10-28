#pragma once
#include <raylib.h>
#include "Lighting.h"
#include "CapsuleCollider.h"
#include "MainCamera.h"
#include <memory>
#include <vector>
#include "Chunk.h"
#include "ParticleSpawner.h"

// Reading on Character Controllers
// https://brooke.fyi/posts/cpp-character-controller-1/

class World;
// States
class PlayerState;
class GroundedState;
class AerialState;
class SlidingState;
class CrouchingState;

class Player
{
public:
	Player(World& t_world);
	void Init();

	void Update();
	void Draw();
	void Draw2D();

	Vector3& GetPosRef() { return position; }
	Camera& GetCameraRef() { return camera->GetCameraRef(); }
private:
	void UpdatePositions();

	void Movement();
	void NormalMovement();

	void RayChecks();
	void CeilingCheck(const Triangle& tri);
	void GroundCheck(const Triangle& tri);
	bool OnWalkableSlope() const;

	void UpdateOccupiedChunks();
	std::vector<Chunk*> occupiedChunks;

	// States
	void SwapState(std::shared_ptr<PlayerState> t_newState);
	std::shared_ptr<PlayerState> state;
	std::shared_ptr<GroundedState> groundedState;
	std::shared_ptr<AerialState> aerialState;
	std::shared_ptr<SlidingState> slidingState;
	std::shared_ptr<CrouchingState> crouchingState;

	// Jumping
	struct JumpInfo
	{
		bool CheckJump(Vector3& t_velocity);
		bool canJump = false;
		bool isJumping = false;

		float jumpForce = 0.3f;
	};
	JumpInfo jump;
	static const int GROUND_CHECK_RAYS_AMOUNT = 5;
	Ray groundCheckRays[GROUND_CHECK_RAYS_AMOUNT];
	Vector3 groundCheckRaysOffsets[GROUND_CHECK_RAYS_AMOUNT];
	bool isGrounded = false;
	float rayHeightDist;
	const float RAY_EXTENTION = 0.1f;
	Vector3 groundNormal = { 0.0f, 1.0f, 0.0f };
	float closestGroundCollision = 999999.0f;

	static const Vector3 UP;
	static const Vector3 DOWN;
	/*static const*/ int MAX_SLOPE_ANGLE = 55;

	Ray aboveCheckRay;

	// Gravity
	bool gravityOn = true;
	float gravityForce = 1.2f;

	/*const*/ float CROUCH_SPEED_MULT = 0.5f;
	/*const*/ float MAX_VELOCITY = 0.65f;
	/*const*/ float GROUNDED_FRICTION = 0.88f;
	/*const*/ float AERIAL_DRAG = 0.92f;
	/*const*/ float SLIDING_FRICTION = 0.99f;
	const float SLIDING_MOVEMENT_STRENGTH = 0.1f;
	const float MIN_SLIDE_SPEED = 0.1f;
	float slideBoost = 1.5f;


	void FreeFly();
	bool freeFly = true;

	Vector3 velocity = { 0.0f, 0.0f, 0.0f };
	Vector3 position;
	float moveSpeed = 1.75f;

	std::shared_ptr<MainCamera> camera;

	// Ray from camera
	void ShootRay();
	Ray shootRay;
	Vector3 shootRayStart;
	RayCollision shootRayCollision;
	bool breaking = true;
	float rayRadius = 2.0f;
	float radiusChange = 0.5f;

	World& WORLD;

	void CollisionDetection();
	CapsuleCollider collider;
	static const int MAX_COLLISION_ITERATIONS = 6;
	const float STANDING_HEIGHT = 3.0f;
	const float SLIDING_HEIGHT = 1.5f;
	float height = STANDING_HEIGHT;
	float radius = 1.0f;

	// Debug
	bool displayDebugElements = false;

	Light& light;

	void SetRandLight(Vector3 pos);
	Light& randLight;

	ParticleSpawner testParticles;

	// States
	friend class GroundedState;
	friend class AerialState;
	friend class SlidingState;
	friend class CrouchingState;
	// Pause menu editting
	friend class PauseMenu;
};

#include "World.h"
#include "PlayerState.h"
#include "GroundedState.h"
#include "AerialState.h"
#include "SlidingState.h"
#include "CrouchingState.h"