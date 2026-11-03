#pragma once
#include <raylib.h>

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

	void Draw(EditTypes t_editType);
	void Update(EditTypes t_editType);

	RayCollision CheckIfClicked(const Ray& ray);

	bool IsDragging() const { return isDragging; }

	void SetSelected(bool s) { selected = s; }

protected:
	void Move();
	virtual void MoveUpdate();
	virtual void MoveDraw();
	void Rotate();
	virtual void RotateUpdate();
	virtual void RotateDraw();
	void Scale();
	virtual void ScaleUpdate();
	virtual void ScaleDraw();

	Model model;
	Matrix transform{};
	Vector3 position = {0.0f , 2.0f, 0.0f}; // Middle of object
	Vector3 scale = {1.0f , 1.0f, 1.0f};
	Vector3 rotation = {0.0f , 0.0f, 0.0f};

private:

	static const float MIN_SCALE;

	void UpdateTransformMatrix();

	Axis CheckArrowSelectedOn();
	Axis CheckCircleSelectedOn();
	RayCollision GetCircleCollision(const Ray& ray, float radius);
	void UpdateArrowBounds();
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
	const float ROTATION_CIRCLE_RADIUS_MULT = 1.25f;

	const Camera& CAMERA;
};