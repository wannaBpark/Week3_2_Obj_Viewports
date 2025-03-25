#pragma once
#pragma once
#include "Vector.h"
#include "Matrix.h"
#include "Core/Math/Quat.h"

#define TORAD 0.0174532925199432957f

struct FTransform
{
protected:
	FVector Position;
	FQuat Rotation;
	FVector Scale;

public:
	FTransform()
		: Position(FVector(0, 0, 0))
		, Rotation(FQuat(0, 0, 0, 1))
		, Scale(FVector(1, 1, 1))
	{
	}

	FTransform(FVector InPosition, FVector InRotation, FVector InScale)
		: Position(InPosition)
		, Scale(InScale)
	{
		Rotation = FQuat(InRotation);
	}

	FTransform(FVector InPosition, FQuat InQuat, FVector InScale)
		: Position(InPosition)
		, Rotation(InQuat)
		, Scale(InScale)
	{
	}

	static const FTransform Identity;

	inline bool Equal(const FTransform& Other) const
	{
		return Position == Other.Position && Rotation == Other.Rotation && Scale == Other.Scale;
	}

	// 뷰 행렬을 구하는 함수 (왼손 좌표계, LookAtLH 방식)
	// 카메라의 위치(Position), 카메라가 바라보는 위치(Position + GetForward()), 그리고 카메라의 Up 벡터를 사용함.
	inline FMatrix GetViewMatrix() const
	{
		return FMatrix::LookAtLH(Position, Position + GetForward(), GetUp());
	}

	// 객체의 위치를 설정하는 함수 (벡터를 인자로 받음)
	inline void SetPosition(const FVector& InPosition)
	{
		Position = InPosition;
	}

	// 객체의 위치를 설정하는 함수 (x, y, z 값을 각각 인자로 받음)
	inline void SetPosition(float x, float y, float z)
	{
		Position = { x, y, z };
	}

	// 객체의 회전을 설정하는 함수 (Euler 각도 벡터를 인자로 받아 쿼터니언으로 변환)
	inline void SetRotation(const FVector& InRotation)
	{
		Rotation = FQuat::EulerToQuaternion(InRotation);
	}

	inline void SetRotation(const FQuat& InQuat)
	{
		Rotation = InQuat;
	}

	// 객체의 회전을 설정하는 함수 (x, y, z Euler 각도를 각각 인자로 받음)
	inline void SetRotation(float x, float y, float z)
	{
		SetRotation(FVector(x, y, z));
	}

	// 객체의 스케일을 설정하는 함수 (벡터로 설정)
	inline void SetScale(FVector InScale)
	{
		Scale = InScale;
	}

	// 객체의 스케일에 값을 더하는 함수 (각 축별로 증가)
	inline void AddScale(FVector InScale)
	{
		Scale.X += InScale.X;
		Scale.Y += InScale.Y;
		Scale.Z += InScale.Z;
	}

	// 객체의 스케일을 설정하는 함수 (x, y, z 값을 각각 인자로 받음)
	inline void SetScale(float x, float y, float z)
	{
		Scale = { x, y, z };
	}

	// 객체의 현재 위치를 반환하는 함수
	FVector GetPosition() const
	{
		return Position;
	}

	// 객체의 현재 회전을 반환하는 함수 (쿼터니언으로 반환)
	FQuat GetRotation() const
	{
		return Rotation;
	}

	// 객체의 현재 스케일을 반환하는 함수
	FVector GetScale() const
	{
		return Scale;
	}

	// 스케일 행렬을 반환하는 함수 (객체의 스케일 값으로 구성된 행렬)
	FMatrix GetScaleMatrix() const
	{
		return FMatrix::GetScaleMatrix(Scale.X, Scale.Y, Scale.Z);
	}

	// 전체 변환 행렬을 반환하는 함수
	// 구성 순서: Scale 행렬 * Rotation 행렬 * Translation 행렬
	FMatrix GetMatrix() const
	{
		return FMatrix::GetScaleMatrix(Scale.X, Scale.Y, Scale.Z)
			* FMatrix::GetRotateMatrix(Rotation)
			* FMatrix::GetTranslateMatrix(Position.X, Position.Y, Position.Z);
	}

	// 스케일을 제외한 로컬 변환 행렬(회전 및 이동만 적용된 행렬)을 반환하는 함수
	FMatrix GetLocalMatrixWithOutScale() const
	{
		return FMatrix::GetRotateMatrix(Rotation)
			* FMatrix::GetTranslateMatrix(Position.X, Position.Y, Position.Z);
	}

	// 객체가 바라보는 전방 벡터를 반환하는 함수
	// 쿼터니언을 회전 행렬로 변환한 후, 회전 행렬의 첫 번째 열(Forward 벡터)을 추출
	FVector GetForward() const
	{
		FMatrix RotationMatrix = FMatrix::GetRotateMatrix(Rotation);
		FVector Forward = FVector(
			RotationMatrix.M[0][0],
			RotationMatrix.M[1][0],
			RotationMatrix.M[2][0]
		);
		return Forward.GetSafeNormal();
	}

	// 객체의 오른쪽 벡터를 반환하는 함수
	// 글로벌 Up 벡터 (0,0,1)와 전방 벡터의 외적을 계산하여 정규화
	FVector GetRight() const
	{
		return FVector::CrossProduct(FVector(0, 0, 1), GetForward()).GetSafeNormal();
	}

	// 객체의 Up 벡터를 반환하는 함수
	// 전방 벡터와 오른쪽 벡터의 외적을 계산하여 정규화
	FVector GetUp() const
	{
		return FVector::CrossProduct(GetForward(), GetRight()).GetSafeNormal();
	}

	// 로컬 좌표계의 오른쪽 벡터를 반환하는 함수 (회전 행렬의 두 번째 열 사용)
	FVector GetLocalRight() const
	{
		FMatrix RotationMatrix = FMatrix::GetRotateMatrix(Rotation);
		FVector Right = FVector(
			RotationMatrix.M[0][1],
			RotationMatrix.M[1][1],
			RotationMatrix.M[2][1]
		);
		return Right.GetSafeNormal();
	}

	// 로컬 좌표계의 Up 벡터를 반환하는 함수 (회전 행렬의 세 번째 열 사용)
	FVector GetLocalUp() const
	{
		FMatrix RotationMatrix = FMatrix::GetRotateMatrix(Rotation);
		FVector up = FVector(
			RotationMatrix.M[0][2],
			RotationMatrix.M[1][2],
			RotationMatrix.M[2][2]
		);
		return up.GetSafeNormal();
	}

	// 객체를 이동시키는 함수 (현재 위치에 InTranslation 값을 더함)
	void Translate(const FVector& InTranslation)
	{
		Position += InTranslation;
	}

	// Euler 각도(도 단위)를 사용하여 객체를 회전시키는 함수
	// InRotation의 X, Y, Z 성분에 따라 Roll, Pitch, Yaw 회전을 차례로 적용
	void Rotate(const FVector& InRotation)
	{
		RotateRoll(InRotation.X);
		RotatePitch(InRotation.Y);
		RotateYaw(InRotation.Z);
	}

	// Yaw 회전 (Z축 기준 회전)을 적용하는 함수
	void RotateYaw(float Angle)
	{
		FVector Axis = FVector(0, 0, 1);
		// 현재 회전에 Z축을 기준으로 Angle 만큼 회전하는 쿼터니언을 곱함
		Rotation = FQuat::MultiplyQuaternions(Rotation, FQuat(Axis, Angle));
	}

	// Pitch 회전 (Y축 기준 회전)을 적용하는 함수
	void RotatePitch(float Angle)
	{
		FVector Axis = FVector(0, 1, 0).GetSafeNormal();
		Rotation = FQuat::MultiplyQuaternions(Rotation, FQuat(Axis, Angle));
	}

	// Roll 회전 (X축 기준 회전)을 적용하는 함수
	void RotateRoll(float Angle)
	{
		FVector Axis = FVector(1, 0, 0).GetSafeNormal();
		Rotation = FQuat::MultiplyQuaternions(Rotation, FQuat(Axis, Angle));
	}

	// 객체를 로컬 좌표계에서 이동시키는 함수
	// 로컬 변환 행렬을 사용해 InTranslation 벡터를 월드 좌표로 변환한 후 현재 위치에 더함
	void MoveLocal(const FVector& InTranslation)
	{
		FMatrix transfromMatrix = GetMatrix();
		FVector worldDelta = transfromMatrix.TransformVector(InTranslation);
		Position += worldDelta;
	}

	// InMatrix 행렬에서 스케일, 회전(쿼터니언), Translation을 추출하여 현재 변환을 설정하는 함수
	void SetFromMatrix(const FMatrix& InMatrix)
	{
		FMatrix matrix = InMatrix;

		// 행렬에서 스케일 추출
		FVector scale = matrix.ExtractScale();
		Scale = scale;

		// 행렬식이 음수면 스케일 부호를 보정
		if (matrix.Determinant() < 0)
		{
			scale *= -1;
			Scale = Scale * FVector(-1.f, 1.f, 1.f);
		}

		// 회전 부분을 쿼터니언으로 변환
		FQuat quat = FQuat(matrix);
		Rotation = quat;

		// Translation(이동) 추출
		FVector translation = matrix.GetTranslation();
		Position = translation;
	}

	// 두 변환(Transform)을 합성하는 함수 (곱셈 순서에 주의)
	// Result = A 변환을 적용한 후 B 변환을 적용.
	// 회전: Result.Rotation = A.Rotation * B.Rotation
	// Translation: Result.Position = B.Rotation.RotateVector(B.Scale * A.Translation) + B.Translation
	// Scale: Result.Scale = A.Scale * B.Scale (성분별 곱셈)
	inline static FTransform MultiPly(const FTransform& A, const FTransform& B)
	{
		FTransform Result;

		FQuat QuatA = A.GetRotation();
		FQuat QuatB = B.GetRotation();
		FVector TranslationA = A.GetPosition();
		FVector TranslationB = B.GetPosition();
		FVector ScaleA = A.GetScale();
		FVector ScaleB = B.GetScale();

		// 회전 합성: A의 회전 후 B의 회전을 적용
		Result.Rotation = FQuat::MultiplyQuaternions(QuatA, QuatB);

		// Translation 합성:
		// A.Translation에 B.Scale을 적용한 후, B의 회전으로 회전시키고, B.Translation을 더함.
		FVector ScaledTransA = TranslationA * ScaleB;
		FVector4 RotatedTranslate = FQuat::VectorQuaternionRotateVector(QuatB, FVector4(ScaledTransA, 0));
		Result.Position = FVector(RotatedTranslate.X + TranslationB.X, RotatedTranslate.Y + TranslationB.Y, RotatedTranslate.Z + TranslationB.Z);

		// 스케일 합성 (성분별 곱셈)
		Result.Scale = ScaleA * ScaleB;

		return Result;
	}

	// 4D 벡터를 이 변환으로 변환하는 함수.
	// 입력 벡터의 W 값이 1이면 점(위치), 0이면 방향으로 취급한다.
	// 변환 공식: V' = Q.Rotate(S * V) + T, 단 x,y,z에만 적용되고, Translation은 W에 따라 적용됨.
	FVector4 TransformVector4(const FVector4& Vector) const
	{
		// 입력 4D 벡터에서 x,y,z를 추출
		FVector vec3 = FVector(Vector.X, Vector.Y, Vector.Z);

		// 스케일 적용
		FVector scaledVec3 = vec3 * Scale;

		// 회전 적용
		FVector rotatedVec3 = Rotation.RotateVector(scaledVec3);

		// Translation은 W 값에 따라 적용 (W가 1이면 적용, 0이면 무시)
		FVector translatedVec3 = rotatedVec3 + (Position * Vector.W);

		FVector4 result;
		result.X = translatedVec3.X;
		result.Y = translatedVec3.Y;
		result.Z = translatedVec3.Z;
		result.W = Vector.W;  // W값은 그대로 유지
		return result;
	}

	// 점(위치) 변환 함수: P' = Q.Rotate(S * P) + T
	FVector TransformPosition(const FVector& Vector) const
	{
		FVector4 InputVectorW0 = FVector4(Vector, 0.0f);
		FQuat NormalizedQuat = Rotation.Normalized();

		FVector4 scaledVector = FVector4(InputVectorW0.X * Scale.X, InputVectorW0.Y * Scale.Y, InputVectorW0.Z * Scale.Z, 0.0f);
		FVector4 rotatedVector = FQuat::VectorQuaternionRotateVector(NormalizedQuat, scaledVector);
		FVector4 translatedVector = FVector4(rotatedVector.X + Position.X, rotatedVector.Y + Position.Y, rotatedVector.Z + Position.Z, 1.0f);
		return FVector(translatedVector.X, translatedVector.Y, translatedVector.Z);
	}

	// 스케일을 적용하지 않고 점(위치) 변환: P' = Q.Rotate(P) + T
	FVector TransformPositionNoScale(const FVector& Vector) const
	{
		FVector4 InputVectorW0 = FVector4(Vector, 0.0f);

		FVector4 rotatedVector = FQuat::VectorQuaternionRotateVector(Rotation, InputVectorW0);
		FVector4 translatedVector = FVector4(rotatedVector.X + Position.X, rotatedVector.Y + Position.Y, rotatedVector.Z + Position.Z, 1.0f);

		return FVector(translatedVector.X, translatedVector.Y, translatedVector.Z);
	}

	// 벡터(방향) 변환 함수: V' = Q.Rotate(S * V)
	// Translation은 방향 벡터에 적용되지 않음.
	FVector TransformVector(const FVector& Vector) const
	{
		FVector scaledVec3 = Vector * Scale;
		FVector rotatedVec3 = Rotation.RotateVector(scaledVec3);
		return rotatedVec3;
	}

	// 스케일 없이 벡터(방향) 변환: V' = Q.Rotate(V)
	FVector TransformVectorNoScale(const FVector& Vector) const
	{
		FVector rotatedVec3 = Rotation.RotateVector(Vector);
		return rotatedVec3;
	}

	// 회전 없이 스케일만 적용한 벡터 변환 함수.
	FVector TransformVectorNoRotation(const FVector& Vector) const
	{
		FVector scaledVec3 = Vector * Scale;
		return scaledVec3;
	}

	// 점(위치)에 대해 역변환을 수행하는 함수.
	// 역변환 공식: P = (Q^-1.Rotate(P' - T)) / Scale  (각 축별 나눔)
	FVector InverseTransformPosition(const FVector& Vector) const
	{
		// Translation 제거
		FVector InputVec = Vector;

		FVector translatedVec = InputVec - Position;

		FVector4 VR = FQuat::VectorQuaternionInverseRotatedVector(Rotation, FVector4(translatedVec, 0.0f));

		FVector VResult = FVector(VR.X / Scale.X, VR.Y / Scale.Y, VR.Z / Scale.Z);

		return VResult;
	}

	// 스케일 없이 점(위치)에 대한 역변환: P = Q^-1.Rotate(P' - T)
	FVector InverseTransformPositionNoScale(const FVector& Vector) const
	{
		FVector temp = { Vector.X - Position.X, Vector.Y - Position.Y, Vector.Z - Position.Z };
		FVector rotated = Rotation.GetInverse().RotateVector(temp);
		return rotated;
	}

	// 벡터(방향)에 대한 역변환 함수: V = (Q^-1.Rotate(V)) / Scale
	FVector InverseTransformVector(const FVector& Vector) const
	{
		FVector rotated = Rotation.GetInverse().RotateVector(Vector);
		return { rotated.X / Scale.X, rotated.Y / Scale.Y, rotated.Z / Scale.Z };
	}

	// 스케일 없이 벡터(방향)에 대한 역변환: V = Q^-1.Rotate(V)
	FVector InverseTransformVectorNoScale(const FVector& Vector) const
	{
		FVector rotated = Rotation.GetInverse().RotateVector(Vector);
		return rotated;
	}

	// 입력 쿼터니언(InRotation)을 현재 회전(Rotation)과 합성하여 회전 변환을 적용하는 함수.
	// 결과 회전 = 현재 Rotation * InRotation.
	FQuat TransformRotation(const FQuat& InRotation) const
	{
		return FQuat::MultiplyQuaternions(Rotation, InRotation);
	}

	// 입력 쿼터니언(InRotation)에 대해 현재 회전의 역을 적용하여 역회전 변환을 수행하는 함수.
	// 결과 = 현재 Rotation^-1 * InRotation.
	FQuat InverseTransformRotation(const FQuat& InRotation) const
	{
		return FQuat::MultiplyQuaternions(Rotation.GetInverse(), InRotation);
	}

	/**
	 * @brief 현재 변환(this)이 주어진 Other 변환에 대해 상대 변환을 계산합니다.
	 *
	 * 두 변환 A (this)와 B (Other)가 있을 때, 상대 변환 T_rel을 아래와 같이 계산합니다.
	 *
	 *   T_rel = B⁻¹ * A
	 *
	 * 각 구성 요소는 다음과 같이 계산됩니다:
	 *  - 상대 스케일: S(A) / S(B) (컴포넌트별 나눗셈)
	 *  - 상대 회전: Q(B)⁻¹ * Q(A)
	 *  - 상대 Translation: (1 / S(B)) * [ Q(B)⁻¹ * (T(A) - T(B)) ]
	 *
	 * @param Other 기준 변환(B). 이 변환에 대해 현재 변환의 상대값을 구합니다.
	 * @return FTransform Other에 대한 현재 변환의 상대 변환.
	 */
	FTransform GetRelativeTransform(const FTransform& Other) const
	{
		FTransform Result;

		// Scale = S(A)/S(B)
		FVector VScale3D = Scale / Other.Scale;

		//VQTranslation = (  ( T(A).X - T(B).X ),  ( T(A).Y - T(B).Y ), ( T(A).Z - T(B).Z), 0.f );
		FVector VQTranslation = Position - Other.Position;

		// Inverse RotatedTranslation
		FVector VR = Other.Rotation.GetInverse().RotateVector(VQTranslation);

		//Translation = 1/S(B)
		FVector VTranslation = VR * Other.Scale;

		// Rotation = Q(B)(-1) * Q(A)	
		FQuat VRotation = FQuat::MultiplyQuaternions(Other.Rotation.GetInverse(), Rotation);

		Result.Scale = VScale3D;
		Result.Position = VTranslation;
		Result.Rotation = VRotation;

		return Result;
	}

	/**
	* @brief 반대(Reverse) 상대 변환을 계산합니다.
	*
	* 두 변환 A (this)와 B (Other)가 있을 때, 아래와 같이 반대 상대 변환을 정의합니다.
	*
	*   상대 스케일 = S(B) / S(A)
	*   상대 회전 = Q(B) * Q(A)⁻¹
	*   상대 Translation = T(B) - [ 상대 스케일 * (RelativeRotation.RotateVector(T(A)) ) ]
	*
	* 이 함수는 A의 역변환을 적용한 후 B가 나오도록 하는 변환을 계산합니다.
	*
	* @param Other 대상 변환(B). 이 변환에 대해 반대 상대 변환을 구합니다.
	* @return FTransform 계산된 반대 상대 변환.
	*/
	FTransform GetRelativeTransformReverse(const FTransform& Other) const
	{
		FTransform Result;

		// 상대 스케일: S(B) / S(A)
		// (각 축별 나눗셈을 수행합니다.)
		FVector RelativeScale(
			Other.Scale.X / this->Scale.X,
			Other.Scale.Y / this->Scale.Y,
			Other.Scale.Z / this->Scale.Z
		);

		// 상대 회전: Q_rel = Q(B) * Q(A)⁻¹
		// 정규화된 쿼터니언의 경우 Inverse는 Conjugate와 동일합니다.
		FQuat InverseRotation = this->Rotation.GetInverse();
		FQuat RelativeRotation = FQuat::MultiplyQuaternions(Other.Rotation, InverseRotation);

		// RotatedTranslation:
		// this->Translation에 상대 회전을 적용하여 회전된 벡터를 구합니다.
		FVector RotatedTranslation = RelativeRotation.RotateVector(this->Position);

		// 최종 Translation:
		// Other.Translation에서 상대 스케일(컴포넌트별 곱셈)한 RotatedTranslation을 뺍니다.
		FVector RelativeTranslation(
			Other.Position.X - RelativeScale.X * RotatedTranslation.X,
			Other.Position.Y - RelativeScale.Y * RotatedTranslation.Y,
			Other.Position.Z - RelativeScale.Z * RotatedTranslation.Z
		);

		// 결과 할당
		Result.Scale = RelativeScale;
		Result.Rotation = RelativeRotation;
		Result.Position = RelativeTranslation;

		// (필요시, 결과 값에 NaN 검사를 추가할 수 있습니다.)
		// Result.DiagnosticCheckNaN_All();

		return Result;
	}

	/**
	* @brief 부모 변환(ParentTransform)에 대해 현재 변환(this)을 로컬(상대) 변환으로 업데이트합니다.
	*
	* 이 함수는 현재 변환을 부모 변환에 대한 상대(로컬) 변환으로 변경합니다.
	* 계산 과정은 다음과 같습니다:
	*  - Scale: 현재 Scale을 부모의 Scale로 나눔 (컴포넌트별)
	*  - Translation:
	*       (1) 부모의 위치를 빼서 위치 차이를 구함
	*       (2) 부모의 회전의 역수를 적용하여 부모 좌표계로 변환
	*       (3) 부모의 Scale로 나누어 보정
	*  - Rotation: 부모의 회전의 역수 * 현재 회전
	*
	* @param ParentTransform 부모 변환. 이 변환에 대해 현재 변환을 상대(로컬) 변환으로 변경합니다.
	*/
	inline static FTransform SetToRelativeTransform(const FTransform& myTransform, const FTransform& ParentTransform)
	{
		FTransform Result;
		// 1. Scale: S(this) / S(Parent) (각 축별 나눗셈)
		Result.Scale = FVector(
			myTransform.Scale.X / ParentTransform.Scale.X,
			myTransform.Scale.Y / ParentTransform.Scale.Y,
			myTransform.Scale.Z / ParentTransform.Scale.Z
		);

		// 2. Translation:
		// 우선 부모의 Translation을 빼서 차이를 구합니다.
		FVector Diff = myTransform.Position - ParentTransform.Position;

		// 부모의 회전의 역수를 구합니다 (정규화된 쿼터니언에서는 Conjugate가 Inverse와 동일)
		FQuat InverseParentRot = ParentTransform.Rotation.GetInverse();

		// 차이에 부모 회전 역수를 적용하여 회전된 Translation을 구합니다.
		FVector RotatedDiff = InverseParentRot.RotateVector(Diff);

		// 그리고 부모의 Scale로 나누어 보정합니다 (각 축별 나눗셈)
		Result.Position = FVector(
			RotatedDiff.X / ParentTransform.Scale.X,
			RotatedDiff.Y / ParentTransform.Scale.Y,
			RotatedDiff.Z / ParentTransform.Scale.Z
		);

		// 3. Rotation:
		// 상대 회전 = Inverse(Parent.Rotation) * this->Rotation
		Result.Rotation = FQuat::MultiplyQuaternions(InverseParentRot, myTransform.Rotation);
		return Result;
	}

	/**
	* @brief 행렬 연산을 사용하여 스케일을 포함한 상대 변환을 계산합니다.
	*
	* Base 변환과 Relative 변환을 각각 행렬로 변환한 후,
	* 다음 단계를 거쳐 상대 변환을 계산합니다:
	*   1. Base와 Relative 변환을 행렬로 변환 (스케일 포함)
	*   2. 원하는 상대 스케일 DesiredScale을 계산: S(Base) / S(Relative) (컴포넌트별)
	*   3. Relative 행렬 BM의 역행렬을 구하고, Base 행렬 AM과 곱하여 상대 행렬 M을 구함.
	*   4. 구해진 행렬 M과 DesiredScale을 이용하여 최종 상대 변환을 구성.
	*
	* @param Base Base 변환(A).
	* @param Relative Relative 변환(B).
	* @return FTransform 계산된 상대 변환.
	*/
	FTransform GetRelativeTransformUsingMatrixWithScale(const FTransform* Base, const FTransform* Relative) const
	{
		// 목표: 올바른 회전(Orientation)을 얻기 위해 행렬을 사용합니다.
		// 단, Translation은 여전히 스케일을 고려해야 합니다.

		// 1. Base와 Relative를 스케일을 포함한 행렬로 변환합니다.
		FMatrix AM = Base->GetMatrix();
		FMatrix BM = Relative->GetMatrix();

		// 2. 결합 스케일 계산
		//    Scale = S(Base) / S(Relative) (각 축별 나눗셈)
		FVector DesiredScale(
			Base->Scale.X / Relative->Scale.X,
			Base->Scale.Y / Relative->Scale.Y,
			Base->Scale.Z / Relative->Scale.Z
		);

		// 3. BM의 역행렬을 구하고, AM과 곱하여 상대 행렬 M을 구합니다.
		FMatrix InvBM = BM.Inverse();
		FMatrix M = AM * InvBM;

		// 4. 구해진 행렬 M과 원하는 스케일(DesiredScale)을 이용해 OutTransform을 구성합니다.
		return ConstructTransformFromMatrixWithDesiredScale(M, DesiredScale);
	}

	// ConstructTransformFromMatrixWithDesiredScale 함수
	// InMatrix : 변환 행렬 (스케일이 포함된 회전+이동 행렬)
	// DesiredScale : 원하는 스케일 (컴포넌트별 값)
	// OutTransform : 복원된 변환(Translation, Rotation, Scale)을 저장할 대상
	FTransform ConstructTransformFromMatrixWithDesiredScale(const FMatrix& InMatrix, const FVector& DesiredScale) const;

	FTransform Inverse() const;

	void ToMatrixInternal(FVector& OutDiagonals, FVector& OutAdds, FVector& OutSubtracts) const;

	FMatrix ToMatrixWithScale() const;
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