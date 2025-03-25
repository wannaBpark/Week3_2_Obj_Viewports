#pragma once
#include "Core/EngineTypes.h"

struct FIntPoint
{
public:
	int32 X, Y;

	static const FIntPoint ZeroValue;

	FIntPoint() = default;

	FIntPoint(int32 InX, int32 InY)
		: X(InX)
		, Y(InY)
	{
	}

	bool operator==(const FIntPoint& Other)
	{
		return X == Other.X && Y == Other.Y;
	}

	bool operator!=(const FIntPoint& Other) const
	{
		return (X != Other.X) || (Y != Other.Y);
	}

	FIntPoint& operator*=(int32 Scale)
	{
		X *= Scale;
		Y *= Scale;

		return *this;
	}
};

struct FIntRect
{
public:
	FIntPoint Min, Max;

	FIntRect(int32 X0, int32 Y0, int32 X1, int32 Y1)
		: Min(X0, Y0)
		, Max(X1, Y1)
	{
	}

	FIntRect(FIntPoint InMin, FIntPoint InMax)
		: Min(InMin)
		, Max(InMax)
	{
	}

	int32 Height() const
	{
		return Max.Y - Min.Y;
	}

	int32 Width() const
	{
		return Max.X - Min.X;
	}

	FIntPoint Size() const
	{
		return FIntPoint(Max.X - Min.X, Max.Y - Min.Y);
	}
};