#pragma once
#include "Vector.h"

#define PIDIV2 3.141592654/2

struct alignas(16) FQuat : public FVector4{
    using FVector::X;
    using FVector::Y;
    using FVector::Z;
    using FVector4::W;

    FQuat() : FVector4(0.0f, 0.0f, 0.0f, 1.0f) {}
	explicit FQuat(float InX, float InY, float InZ, float InW) : FVector4(InX, InY, InZ, InW) {}
    explicit FQuat(FVector Rotation) : FVector4(EulerToQuaternion(Rotation)) {}
    FQuat(const FVector& Axis, float AngleInDegrees) : FVector4(AxisAngleToQuaternion(Axis, AngleInDegrees)) {}


    static FQuat EulerToQuaternion(FVector Euler);
    static FVector QuaternionToEuler(const FQuat& quaternion);
    static FQuat AxisAngleToQuaternion(const FVector& Axis, float AngleInDegrees);

    static FQuat AddQuaternions(const FQuat& q1, const FQuat& q2);
    static FQuat MultiplyQuaternions(const FQuat& q1, const FQuat& q2);
    static FQuat SubtractQuaternions(const FQuat& q1, const FQuat& q2);

    static FQuat MakeFromRotationMatrix(const struct FMatrix& M);
    
    FQuat Inverse() const { return FQuat{ -X,-Y,-Z, W }; }
    FVector RotateVector(const FVector& V) const;
    FVector GetEuler() const { return QuaternionToEuler(*this); }

    FQuat& operator*(const FQuat& q);
};

inline FQuat& FQuat::operator*(const FQuat& q) {
    X *= q.X;
    Y *= q.Y;
    Z *= q.Z;
    W *= q.W;

    return *this;
}

class FQuat2
{
public:
	/** Ctor. initialize to an identity rotation. */
	FQuat2() :Rot(1.0f, 0.0f) {}
	/** Ctor. initialize from a rotation in radians. */
	explicit FQuat2(float RotRadians) :Rot(FMath::Cos(RotRadians), FMath::Sin(RotRadians)) {}
	/** Ctor. initialize from an FVector2D, representing a complex number. */
	explicit FQuat2(const FVector2D& InRot) :Rot((FVector2D)InRot) {}

	/**
	 * Transform a 2D point by the 2D complex number representing the rotation:
	 * In imaginary land: (x + yi) * (u + vi) == (xu - yv) + (xv + yu)i
	 *
	 * Looking at this as a matrix, x == cos(A), y == sin(A)
	 *
	 * [x y] * [ cosA  sinA] == [x y] * [ u v] == [xu-yv xv+yu]
	 *         [-sinA  cosA]            [-v u]
	 *
	 * Looking at the above results, we see the equivalence with matrix multiplication.
	 */
	FVector2D TransformPoint(const FVector2D& Point) const
	{
		return FVector2D(
			Point.X * Rot.X - Point.Y * Rot.Y,
			Point.X * Rot.Y + Point.Y * Rot.X);
	}
	/**
	 * Vector rotation is equivalent to rotating a point.
	 */
	template<typename ArgType>
	FVector2D TransformVector(const FVector2D& Vector) const
	{
		return TransformPoint(Vector);
	}
	/**
	 * Transform 2 rotations defined by complex numbers:
	 * In imaginary land: (A + Bi) * (C + Di) == (AC - BD) + (AD + BC)i
	 *
	 * Looking at this as a matrix, A == cos(theta), B == sin(theta), C == cos(sigma), D == sin(sigma):
	 *
	 * [ A B] * [ C D] == [  AC-BD  AD+BC]
	 * [-B A]   [-D C]    [-(AD+BC) AC-BD]
	 *
	 * If you look at how the vector multiply works out: [X(AC-BD)+Y(-BC-AD)  X(AD+BC)+Y(-BD+AC)]
	 * you can see it follows the same form of the imaginary form. Indeed, check out how the matrix nicely works
	 * out to [ A B] for a visual proof of the results.
	 *        [-B A]
	 */
	FQuat2 Concatenate(const FQuat2& RHS) const
	{
		return FQuat2(TransformPoint(RHS.Rot));
	}
	/**
	 * Invert the rotation  defined by complex numbers:
	 * In imaginary land, an inverse is a complex conjugate, which is equivalent to reflecting about the X axis:
	 * Conj(A + Bi) == A - Bi
	 */
	FQuat2 Inverse() const
	{
		return FQuat2(FVector2D(Rot.X, -Rot.Y));
	}

	/** Equality. */
	bool operator==(const FQuat2& Other) const
	{
		return Rot.X == Other.Rot.X && Rot.Y == Other.Rot.Y;
	}

	/** Inequality. */
	bool operator!=(const FQuat2& Other) const
	{
		return !operator==(Other);
	}

	/** Access to the underlying FVector2D that stores the complex number. */
	const FVector2D& GetVector() const { return Rot; }

private:
	/** Underlying storage of the rotation (X = cos(theta), Y = sin(theta). */
	FVector2D Rot;
};