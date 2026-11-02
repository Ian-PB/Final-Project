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

	void Init();
	void Draw(EditTypes t_editType);
	void Update(EditTypes t_editType);

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
	Vector3 position = {0.0f , 2.0f, 0.0f}; // Middle of object
	Vector3 scale = {1.0f , 1.0f, 1.0f};

private:

	Axis CheckArrowSelectedOn();
	void UpdateArrowBounds();
	// Arrow bounding boxes (x, y, z)
	BoundingBox boundingBoxes[3];
	Axis hoveredAxis = Axis::None;
	Axis selectedAxis = Axis::None;
	float changeSpeedMult = 0.05f;


	float arrowHeadHeight = 0.5f;
	float arrowHeadBase = 0.3f;
	float arrowLength = 3.0f;
	float lineWidth = 0.1f;

	const Camera& CAMERA;
};