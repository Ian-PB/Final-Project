#pragma once
#include <raylib.h>
#include <raymath.h>

enum class EditTypes
{
	None,
	Move,
	Rotate,
	Scale
};
enum class Axis
{
	None = -1,
	X = 0,
	Y = 1,
	Z = 2
};

class BaseBuildingBlock
{
public:
	BaseBuildingBlock(const Camera& t_cam);
	virtual void InitModel();

	void Draw(EditTypes t_editType);
	void Update(EditTypes t_editType);

	RayCollision CheckIfClicked(const Ray& ray);

	bool IsDragging() const { return isDragging; }

	void SetSelected(bool s) { selected = s; }

	bool ChangePointDensity(Vector3 t_point, float& t_density);

	void SetPos(Vector3 t_pos) { position = t_pos; UpdateTransformMatrix(); UpdateArrowBounds(); }
	Vector3 GetPos() const { return position; }

	BoundingBox GetBounds() const { return bounds; }
	float GetBoundingSkin() { return SIGNED_DIST_SKIN; }

protected:
	Vector3 GetHalfDimensions() const { return Vector3Scale(scale, 0.5f); }

	void Move();
	void MoveUpdate();
	void MoveDraw();
	void Rotate();
	void RotateUpdate();
	void RotateDraw();
	void Scale();
	void ScaleUpdate();
	void ScaleDraw();

	virtual void UpdateBounds();
	void UpdateTransformMatrix();
	void UpdateArrowBounds();

	Model model;
	Matrix transform{};
	BoundingBox bounds;

	Vector3 position = {0.0f , 2.0f, 0.0f}; // Middle of object
	Vector3 scale = {1.2f , 1.2f, 1.2f};
	Vector3 rotation = {0.0f , 0.0f, 0.0f};

	static const float SIGNED_DIST_SKIN;

private:
	virtual float SignedDist(Vector3 t_pointPos);
	static const float MIN_SCALE;


	Axis CheckArrowSelectedOn();
	Axis CheckCircleSelectedOn();
	// Arrow bounding boxes (x, y, z)
	BoundingBox boundingBoxes[3];
	Axis hoveredAxis = Axis::None;
	Axis selectedAxis = Axis::None;
	bool isDragging = false;
	float changeSpeedMult = 0.05f;

	bool selected = false;

	float arrowHeadHeight = 0.5f;
	float arrowHeadBase = 0.3f;
	float arrowLength = 3.0f;
	float lineWidth = 0.1f;
	const float ROTATION_CIRCLE_RADIUS_MULT = 1.1f;

	const Camera& CAMERA;
};