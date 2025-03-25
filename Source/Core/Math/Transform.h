#pragma once
#include "Vector.h"
#include "Matrix.h"
#include "Core/Engine.h"
#include "Core/Math/Plane.h"

#define TORAD 0.0174532925199432957f

struct FTransform
{
protected:
	FVector Position;
	FQuat Rotation;
	FVector Scale;
	int Depth;

public:
	FTransform()
		: Position(FVector(0, 0, 0))
		, Rotation(FQuat(0, 0, 0, 1))
		, Scale(FVector(1, 1, 1))
	{
	}

	FTransform(FVector InPosition, FVector InRotation, FVector InScale)
		: Position(InPosition)
		, Rotation(InRotation)
		, Scale(InScale)
	{
	}

	FTransform(FVector InPosition, FQuat InQuat, FVector InScale)
		: Position(InPosition)
		, Rotation(InQuat)
		, Scale(InScale)
	{
	}

	inline FMatrix GetViewMatrix() const
	{
		return FMatrix::LookAtLH(Position, Position + GetForward(), GetUp());
	}

	inline virtual void SetPosition(const FVector& InPosition)
	{
		Position = InPosition;
	}
	inline virtual void SetPosition(float x, float y, float z)
	{
		Position = { x, y, z };
	}
	inline virtual void SetRotation(const FVector& InRotation) // 확인 필요함~
	{
		Rotation = FQuat::EulerToQuaternion(InRotation);
	}
	inline virtual void SetRotation(const FQuat& InQuat)
	{
		Rotation = InQuat;
	}
	inline virtual void SetRotation(float x, float y, float z)
	{
		SetRotation(FVector(x, y, z));
	}
	inline void SetScale(FVector InScale)
	{
		Scale = InScale;
	}
	inline void AddScale(FVector InScale)
	{
		Scale.X += InScale.X;
		Scale.Y += InScale.Y;
		Scale.Z += InScale.Z;
	}
	inline void SetScale(float x, float y, float z)
	{
		Scale = { x, y, z };
	}
	FVector GetPosition() const
	{
		return Position;
	}
	FQuat GetRotation() const
	{
		return Rotation;
	}

	FVector GetScale() const
	{
		return Scale;
	}

	FMatrix GetMatrix() const
	{
		return FMatrix::GetScaleMatrix(Scale.X, Scale.Y, Scale.Z)
			* FMatrix::GetRotateMatrix(Rotation)
			* FMatrix::GetTranslateMatrix(Position.X, Position.Y, Position.Z);
	}

	FVector GetForward() const
	{
		// 쿼터니언을 회전 행렬로 변환
		FMatrix RotationMatrix = FMatrix::GetRotateMatrix(Rotation);

		// 회전 행렬의 첫 번째 열이 Forward 벡터를 나타냄
		FVector Forward = FVector(
			RotationMatrix.M[0][0],
			RotationMatrix.M[1][0],
			RotationMatrix.M[2][0]
		);

		return Forward.GetSafeNormal();
	}

	FVector GetRight() const
	{
		return FVector::CrossProduct(FVector(0, 0, 1), GetForward()).GetSafeNormal();
	}

	FVector GetUp() const {
		return FVector::CrossProduct(GetForward(), GetRight()).GetSafeNormal();

	}


	FVector GetLocalRight() const
	{
		FMatrix RotationMatrix = FMatrix::GetRotateMatrix(Rotation);

		FVector Right = FVector(
			RotationMatrix.M[0][1],  // 첫 번째 열이 Right 벡터
			RotationMatrix.M[1][1],
			RotationMatrix.M[2][1]
		);

		return Right.GetSafeNormal();
	}

	FVector GetLocalUp() const
	{
		FMatrix RotationMatrix = FMatrix::GetRotateMatrix(Rotation);

		FVector up = FVector(
			RotationMatrix.M[0][2],  // 첫 번째 열이 Right 벡터
			RotationMatrix.M[1][2],
			RotationMatrix.M[2][2]
		);

		return up.GetSafeNormal();
	}

	void Translate(const FVector& InTranslation)
	{
		Position += InTranslation;
	}

	// InRotate는 Degree 단위
	void Rotate(const FVector& InRotation)
	{
		RotateRoll(InRotation.X);
		RotatePitch(InRotation.Y);
		RotateYaw(InRotation.Z);
	}

	void RotateYaw(float Angle)
	{
		FVector Axis = FVector(0, 0, 1);
		Rotation = FQuat::MultiplyQuaternions(FQuat(Axis, Angle),Rotation); //현재 쿼터니언 * Z축으로 돌리는 쿼터니언(오른쪽에서 왼쪽으로 계산)

		//Rotation = FQuat::MultiplyQuaternions(Rotation, FQuat(0, 0, sin(Angle * TORAD / 2), cos(Angle * TORAD / 2)));
	}

	void RotatePitch(float Angle)
	{
		FVector Axis = FVector(0, 1, 0).GetSafeNormal();
		Rotation = FQuat::MultiplyQuaternions(FQuat(Axis, Angle), Rotation);
	}

	void RotateRoll(float Angle)
	{
		FVector Axis = FVector(1, 0, 0).GetSafeNormal();
		Rotation = FQuat::MultiplyQuaternions(FQuat(Axis, Angle), Rotation);
	}

	void MoveLocal(const FVector& LocalDelta)
	{
		// 현재 변환 행렬 (Scale * Rotation * Translation)
		FMatrix TransformMatrix = GetMatrix();

		// 로컬 이동 벡터를 월드 이동 벡터로 변환
		FVector WorldDelta = TransformMatrix.TransformVector(LocalDelta);

		// 최종적으로 이동 적용
		Position += WorldDelta;
	}




};

struct FSlateLayoutTransform;

struct FTransform2
{
public:
	FTransform2(FVector2D InTranslation = FVector2D(0.0f, 0.0f))
		: Translation(InTranslation)
	{
	}

	FTransform2(float InScale, FVector2D InTranslation = FVector2D(0.0f, 0.0f))
		: Matrix(FMatrix2x2(InScale))
		, Translation(InTranslation)
	{
	}

	FTransform2(const FVector2D& InScale, FVector2D InTranslation = FVector2D(0.0f, 0.0f))
		: Matrix(FMatrix2x2(InScale))
		, Translation(InTranslation)
	{
	}

	FTransform2(const FQuat2& Rot, FVector2D InTranslation = FVector2D(0.0f, 0.0f))
		: Matrix(FMatrix2x2(Rot))
		, Translation(InTranslation)
	{
	}

	FTransform2(const FMatrix2x2& InMatrix, FVector2D InTranslation = FVector2D(0.0f, 0.0f))
		: Matrix(InMatrix)
		, Translation(InTranslation)
	{
	}

	FVector2D TransformPoint(const FVector2D& Point) const
	{
		FVector2D Result = Matrix.TransformPoint(Point);
		Result.X += Translation.X;
		Result.Y += Translation.Y;

		return Result;
	}

	FVector2D TransformVector(const FVector2D& Vector) const
	{
		return Matrix.TransformVector(Vector);
	}

	FTransform2 Concatenate(const FTransform2& RHS) const
	{
		FMatrix2x2 CombinedMatrix = Matrix.Concatenate(RHS.Matrix);
		FVector2D TransfomredTranslation = RHS.TransformPoint(Translation);
		FVector2D CombinedTranslation = FVector2D(TransfomredTranslation.X + RHS.Translation.X, TransfomredTranslation.Y + RHS.Translation.Y);

		return FTransform2(CombinedMatrix, CombinedTranslation);
	}

	FTransform2 Concatenate(const FSlateLayoutTransform& RHS) const;

	FTransform2 Inverse() const
	{
		FMatrix2x2 InverseMatrix = Matrix.Inverse();
		FVector2D InverseTranslation = InverseMatrix.TransformPoint(FVector2D(-Translation.X, -Translation.Y));
		return FTransform2(InverseMatrix, InverseTranslation);
	}

	bool operator==(const FTransform2& Other) const
	{
		return Matrix == Other.Matrix && (Translation.X == Other.Translation.X && Translation.Y == Other.Translation.Y);
	}

	bool operator!=(const FTransform2& Other) const
	{
		return !operator==(Other);
	}

	const FMatrix2x2& GetMatrix() const
	{
		return Matrix;
	}

	const FVector2D& GetTranslation() const
	{
		return Translation;
	}

	void SetTranslation(const FVector2D& InTranslation)
	{
		Translation = InTranslation;
	}

	void SetMatrix(const FMatrix2x2& InMatrix)
	{
		Matrix = InMatrix;
	}

	bool IsIdentity() const
	{
		return Matrix.IsIdentity() && (Translation.X == 0 && Translation.Y == 0);
	}

	FMatrix To3DMatrix() const
	{
		FMatrix Result;
		Result.M[0][0] = Matrix.M[0][0]; Result.M[0][1] = Matrix.M[0][1]; Result.M[0][2] = 0.0f; Result.M[0][3] = 0.0f;
		Result.M[1][0] = Matrix.M[1][0]; Result.M[1][1] = Matrix.M[1][1]; Result.M[1][2] = 0.0f; Result.M[1][3] = 0.0f;
		Result.M[2][0] = 0.0f; Result.M[2][1] = 0.0f; Result.M[2][2] = 1.0f; Result.M[2][3] = 0.0f;
		Result.M[3][0] = Translation.X;	Result.M[3][1] = Translation.Y;	Result.M[3][2] = 0.0f; Result.M[3][3] = 1.0f;
		return Result;
	}

private:
	FMatrix2x2 Matrix;
	FVector2D Translation;
};

struct FSlateLayoutTransform
{
public:
	FSlateLayoutTransform(float InScale = 1.0f)
		: Scale(InScale)
		, Translation(FVector2D())
	{
	}

	/** Ctor from a scale followed by translate. Shortcut to Concatenate(InScale, InTranslation). */
	explicit FSlateLayoutTransform(float InScale, const FVector2D& InTranslation)
		: Scale(InScale)
		, Translation(InTranslation)
	{
	}

	explicit FSlateLayoutTransform(const FVector2D& InTranslation)
		:Scale(1.0f)
		, Translation(InTranslation)
	{
	}

	/** Access to the 2D translation */
	FVector2D GetTranslation() const
	{
		return Translation;
	}

	/** Access to the scale. */
	float GetScale() const
	{
		return Scale;
	}

	/** Support for converting to an FMatrix. */
	FMatrix ToMatrix() const
	{
		FMatrix Matrix;
		Matrix.M[0][0] = Scale;
		Matrix.M[1][1] = Scale;
		Matrix.M[2][2] = Scale;
		Matrix.M[3][3] = 1.0f;

		Matrix.M[3][0] = Translation.X;
		Matrix.M[3][1] = Translation.Y;
		Matrix.M[3][2] = 0.0f;

		return Matrix;
	}

	FORCEINLINE FVector2D TransformPoint(const FVector2D& Point) const
	{
		FVector2D Result = FVector2D(Point.X * Scale, Point.Y * Scale);
		Result = FVector2D(Result.X + Translation.X, Result.Y + Translation.Y);

		return Result;
	}

	FORCEINLINE FVector2D TransformVector(const FVector2D& Vector) const
	{
		FVector2D Result = FVector2D(Vector.X * Scale, Vector.Y * Scale);
		return Result;
	}

	/**
	 * This works by transforming the origin through LHS then RHS.
	 * In matrix form, looks like this:
	 * [ Sa  0   0 ]   [ Sb  0   0 ]
	 * [ 0   Sa  0 ] * [ 0   Sb  0 ]
	 * [ Tax Tay 1 ]   [ Tbx Tby 1 ]
	 */
	FSlateLayoutTransform Concatenate(const FSlateLayoutTransform& RHS) const
	{
		// New Translation is essentially: RHS.TransformPoint(TransformPoint(FVector2D::ZeroVector))
		// Since Zero through LHS -> Translation we optimize it slightly to skip the zero multiplies.
		return FSlateLayoutTransform(Scale * RHS.Scale, RHS.TransformPoint(Translation));
	}

	FTransform2 Concatenate(const FTransform2& RHS) const
	{
		return FTransform2(FVector2D(Scale, Scale), Translation).Concatenate(RHS);
	}

	/** Invert the transform/scale. */
	FSlateLayoutTransform Inverse() const
	{
		return FSlateLayoutTransform(1 / Scale, FVector2D(-Translation.X * 1/Scale, -Translation.Y * 1 / Scale));
	}

	/** Equality. */
	bool operator==(const FSlateLayoutTransform& Other) const
	{
		return Scale == Other.Scale && (Translation.X == Other.Translation.X && Translation.Y == Other.Translation.Y);
	}

	/** Inequality. */
	bool operator!=(const FSlateLayoutTransform& Other) const
	{
		return !operator==(Other);
	}

private:
	float Scale;
	FVector2D Translation;
};