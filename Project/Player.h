#pragma once
#include <raylib.h>
#include "Lighting.h"
#include "CapsuleCollider.h"
#include "MainCamera.h"
#include <memory>
#include <vector>
#include "Chunk.h"

// Reading on Character Controllers
// https://brooke.fyi/posts/cpp-character-controller-1/

class World;
class PlayerState;
class GroundedState;
class AerialState;
// class SlidingState;

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

	void UpdateOccupiedChunks();
	std::vector<Chunk*> occupiedChunks;

	// States
	void SwapState(std::shared_ptr<PlayerState> t_newState);
	std::shared_ptr<PlayerState> state;
	std::shared_ptr<GroundedState> groundedState;
	std::shared_ptr<AerialState> aerialState;
	// std::shared_ptr<SlidingState> slidingState;

	// Jumping
	struct JumpInfo
	{
		bool CheckJump(Vector3& t_velocity);
		bool canJump;
		bool isJumping;

		float jumpForce = 0.25f;
	};
	JumpInfo jump;
	static const int GROUND_CHECK_RAYS_AMOUNT = 5;
	Ray groundCheckRays[GROUND_CHECK_RAYS_AMOUNT];
	Vector3 groundCheckRaysOffsets[GROUND_CHECK_RAYS_AMOUNT];
	bool isGrounded = false;
	float rayHeightDist;
	const float RAY_EXTENTION = 0.1f;

	static const Vector3 UP;
	static const Vector3 DOWN;
	static const int MAX_SLOPE_ANGLE = 45;

	Ray aboveCheckRay;

	// Gravity
	bool gravityOn = true;
	float gravityForce = 0.6f;
	const float MAX_VELOCITY = 0.65f;
	const float GROUNDED_DRAG = 0.88f;
	const float AERIAL_DRAG = 0.92f;
	

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
	float height = 3.0f;
	float radius = 1.0f;

	// Debug
	bool displayDebugElements = false;

	Light& light;

	void SetRandLight(Vector3 pos);
	Light& randLight;


	friend class GroundedState;
	friend class AerialState;
	// friend class SlidingState;
};

#include "World.h"
#include "PlayerState.h"
#include "GroundedState.h"
#include "AerialState.h"
// #include "SlidingState.h"