#pragma once
#include <Core/Math/Transform.h>

struct FVector2
{
    float x, y;
    FVector2() : x(0), y(0) {}
    FVector2(float inX, float inY) : x(inX), y(inY) {}

	FVector2 operator+(const FVector2& Other) const { return FVector2(x + Other.x, y + Other.y); }
	FVector2 operator-(const FVector2& Other) const { return FVector2(x - Other.x, y - Other.y); }
	FVector2 operator*(float Scale) const { return FVector2(x * Scale, y * Scale); }
	FVector2 operator/(float Scale) const { return FVector2(x / Scale, y / Scale); }
	FVector2 operator-() const { return FVector2(-x, -y); }

	FVector2 operator+=(const FVector2& Other) { x += Other.x; y += Other.y; return *this; }
	FVector2 operator-=(const FVector2& Other) { x -= Other.x; y -= Other.y; return *this; }
	FVector2 operator*=(float Scale) { x *= Scale; y *= Scale; return *this; }
	FVector2 operator/=(float Scale) { x /= Scale; y /= Scale; return *this; }

	bool operator==(const FVector2& Other) const { return x == Other.x && y == Other.y; }
	bool operator!=(const FVector2& Other) const { return x != Other.x || y != Other.y; }
	bool operator<(const FVector2& Other) const { return x < Other.x && y < Other.y; }
	bool operator>(const FVector2& Other) const { return x > Other.x && y > Other.y; }
	bool operator<=(const FVector2& Other) const { return x <= Other.x && y <= Other.y; }
	bool operator>=(const FVector2& Other) const { return x >= Other.x && y >= Other.y; }

	float& operator[](int Index) { return (&x)[Index]; }
	const float& operator[](int Index) const { return (&x)[Index]; }

	float SizeSquared() const { return x * x + y * y; }
	float Size() const { return sqrtf(SizeSquared()); }
};

FORCEINLINE FVector2 operator*(float Scale, const FVector2& V) { return V * Scale; }

class FGeometry
{
public:
    FGeometry();
    ~FGeometry();

    FGeometry(const FGeometry& Other) = default;
    FGeometry& operator=(const FGeometry& Other);

    FGeometry(const FVector2& OffsetFromParent, const FVector2& ParentAbsolutePosition, const FVector2& InLocalSize, float InScale)
		: LocalSize(InLocalSize)
		, AbsoluteScale(1.0f)
		, AbsolutePosition(0.0f, 0.0f)
		, bHasRenderTransform(false)
	{
		FVector2 LayOutOffset = InScale * FVector2(ParentAbsolutePosition);
	}



public:
    const FVector2 LocalSize;
    const float AbsoluteScale;
	const FVector2 AbsolutePosition;
	const FVector2 LocalPosition;

private:
    FTransform AccumulatedRenderTransform;
	const bool bHasRenderTransform;
};

