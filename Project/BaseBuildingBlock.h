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
	bool IsSelected() const { return selected; }

	bool ChangePointDensity(Vector3 t_point, float& t_density);

	void SetPos(Vector3 t_pos) { position = t_pos; UpdateTransformMatrix(); UpdateArrowBounds(); }
	Vector3 GetPos() const { return position; }

	void SetScale(Vector3 t_scale) { scale = t_scale; UpdateTransformMatrix(); UpdateArrowBounds(); }
	Vector3 GetScale() const { return scale; }

	void SetRotation(Vector3 t_rot) { rotation = t_rot; UpdateTransformMatrix(); UpdateArrowBounds(); }
	Vector3 GetRotation() const { return rotation; }

	BoundingBox GetBounds() const { return bounds; }
	float GetBoundingSkin() { return SIGNED_DIST_SKIN; }

	Axis GetHoveredAxis() const { return hoveredAxis; }

	Color GetColor() const { return color; }
	void SetColor(Color t_col) { color = t_col; }

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

	bool selected = false;

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


	float arrowHeadHeight = 0.5f;
	float arrowHeadBase = 0.3f;
	float arrowLength = 3.0f;
	float lineWidth = 0.1f;
	const float ROTATION_CIRCLE_RADIUS_MULT = 1.1f;

	const Camera& CAMERA;

	Color color;
};