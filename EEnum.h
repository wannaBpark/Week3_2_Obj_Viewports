#pragma once
enum class EProjectionType {
	PERSPECTIVE = 1 << 0,
	ORTHOGONAL = 1 << 1,
};

enum class ECollisionDetection {
	OBB,
	AABB,
};

enum class ESelectedAxis : unsigned int
{
	None,
	X,
	Y,
	Z
};

enum class EGizmoType : unsigned int
{
	Translate,
	Rotate,
	Scale,
	Max
};

enum EShowFlag : unsigned int {
	Grid = 1 << 0,
	Primitive = 1 << 1,
	Text = 1 << 2,
	BoundingBox = 1 << 3,
};

struct FHitColliderInfo {
	float minX = { 0 };
	float maxX = { 0 };
	float minY = { 0 };
	float maxY = { 0 };
	float minZ = { 0 };
	float maxZ = { 0 };
};