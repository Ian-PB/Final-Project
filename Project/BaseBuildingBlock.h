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
	virtual void MoveDraw();
	virtual void MoveUpdate();
	virtual void RotateDraw();
	virtual void RotateUpdate();
	virtual void ScaleDraw();
	virtual void ScaleUpdate();

	Vector3 position = {0.0f , 2.0f, 0.0f}; // Middle of object

private:

	Axis CheckArrowSelectedOn();
	void UpdateArrowBounds();
	// Arrow bounding boxes (x, y, z)
	BoundingBox boundingBoxes[3];
	Axis selectedAxis = Axis::None;


	float arrowHeadHeight = 0.5f;
	float arrowHeadBase = 0.3f;
	float arrowLength = 3.0f;
	float lineWidth = 0.1f;

	const Camera& CAMERA;
};