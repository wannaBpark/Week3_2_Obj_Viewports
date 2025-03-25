#pragma once
#include "Vector.h"
#include "Matrix.h"
#include "Core/Engine.h"
#include "Core/Math/Plane.h"
#include "Serialization/Archive.h"

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

	void Serialize(FArchive& ar) const
	{
		ar << Position << Rotation << Scale;
	}
	void Deserialize(FArchive& ar)
	{
		ar >> Position >> Rotation >> Scale;
	}

};