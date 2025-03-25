#include "Vector.h"

const FVector FVector::ZeroVector = FVector(0, 0, 0);
const FVector FVector::OneVector = FVector(1, 1, 1);

const FVector2D FVector2D::ZeroVector = FVector2D(0, 0);

FVector2D FVector2D::operator+(const FVector2D& Other) const
{
	return FVector2D(X + Other.X, Y + Other.Y);
}

FVector2D& FVector2D::operator+=(const FVector2D& Other)
{
	X += Other.X;
	Y += Other.Y;
	return *this;
}

FVector2D FVector2D::operator-(const FVector2D& Other) const
{
	return FVector2D(X - Other.X, Y - Other.Y);
}

FVector2D& FVector2D::operator-=(const FVector2D& Other)
{
	X -= Other.X;
	Y -= Other.Y;
	return *this;
}

bool FVector2D::operator==(const FVector2D& Other) const
{
	return X == Other.X && Y == Other.Y;
}

bool FVector2D::operator!=(const FVector2D& Other) const
{
	return X != Other.X || Y != Other.Y;
}

FVector2D FVector2D::operator*(float Scalar) const
{
	return FVector2D(X * Scalar, Y * Scalar);
}

FVector2D FVector2D::operator/(const FVector2D& Other) const 
{
	return FVector2D(X / Other.X, Y / Other.Y);
}

FVector2D FVector2D::RoundToVector() const
{
	return FVector2D(std::floor(X + 0.5f), std::floor(Y + 0.5f));
}
