#include "Transform.h"

FTransform FTransform::ConstructTransformFromMatrixWithDesiredScale(const FMatrix& InMatrix, const FVector& DesiredScale) const
{
	// 1. Translation 추출  
//    Translation은 4번째 행의 앞 세 개 요소에 저장되어 있다고 가정합니다.
	FVector Translation(
		InMatrix.M[3][0],
		InMatrix.M[3][1],
		InMatrix.M[3][2]
	);

	// 2. 회전 행렬 복원  
	//    상위 3x3 부분은 (스케일 * 회전) 행렬입니다.
	//    각 행 i (0,1,2)에 대해, DesiredScale의 해당 성분으로 나누어 정규화된 회전 행렬 R를 구합니다.
	FMatrix RotationMatrix;
	for (int i = 0; i < 3; ++i)
	{
		// 각 행에 대해 스케일 값 선택
		float ScaleComponent = (i == 0) ? DesiredScale.X : (i == 1 ? DesiredScale.Y : DesiredScale.Z);

		// 스케일 값이 0이 아닐 경우에만 나눕니다.
		// (0이면 기본값 0을 유지)
		for (int j = 0; j < 3; ++j)
		{
			RotationMatrix.M[i][j] = (ScaleComponent != 0.0f) ? (InMatrix.M[i][j] / ScaleComponent) : 0.0f;
		}
	}
	// 나머지 행/열은 단위행렬로 설정
	RotationMatrix.M[0][3] = 0.0f;
	RotationMatrix.M[1][3] = 0.0f;
	RotationMatrix.M[2][3] = 0.0f;
	RotationMatrix.M[3][0] = 0.0f;
	RotationMatrix.M[3][1] = 0.0f;
	RotationMatrix.M[3][2] = 0.0f;
	RotationMatrix.M[3][3] = 1.0f;

	// 3. 회전 행렬로부터 쿼터니언 복원  
	FQuat Rotation = FQuat::MakeFromRotationMatrix(RotationMatrix);

	// 4. 결과 할당  
	FTransform OutTransform;
	OutTransform.Position = Translation;
	OutTransform.Rotation = Rotation;
	OutTransform.Scale = DesiredScale;

	return OutTransform;
}

FTransform FTransform::Inverse() const
{
	FVector4 InverseScale = FVector4(1.f / Scale.X, 1.f / Scale.Y, 1.f / Scale.Z, 0.f);
	FQuat InverseRotation = Rotation.GetInverse();

	FVector4 ScaledTranslation = FVector4(InverseScale.X * Position.X, InverseScale.Y * Position.Y, InverseScale.Z * Position.Z, 0.f);
	FVector4 RotatedTranslation = FQuat::VectorQuaternionRotateVector(InverseRotation, ScaledTranslation);
	FVector InverseTranslation = FVector(-RotatedTranslation.X, -RotatedTranslation.Y, -RotatedTranslation.Z);

	return FTransform(InverseTranslation, InverseRotation, FVector(InverseScale.X, InverseScale.Y, InverseScale.Z));
}

void FTransform::ToMatrixInternal(FVector& OutDiagonals, FVector& OutAdds, FVector& OutSubtracts) const
{
	const FVector4 MyRotation = Rotation;
	const FVector4 RotationX2Y2Z2 = FVector4(MyRotation.X + MyRotation.X, MyRotation.Y + MyRotation.Y, MyRotation.Z + MyRotation.Z, 0.f);
	const FVector4 RotationXX2YY2ZZ2 = FVector4::MultiplyVector4(RotationX2Y2Z2, MyRotation);

	// The diagonal terms of the rotation matrix are:
	//   (1 - (yy2 + zz2)) * scale
	//   (1 - (xx2 + zz2)) * scale
	//   (1 - (xx2 + yy2)) * scale
	const FVector4 yy2xx2xx2 = FVector4(RotationXX2YY2ZZ2.Y, RotationXX2YY2ZZ2.X, RotationXX2YY2ZZ2.X, RotationXX2YY2ZZ2.X);
	const FVector4 zz2zz2yy2 = FVector4(RotationXX2YY2ZZ2.Z, RotationXX2YY2ZZ2.Z, RotationXX2YY2ZZ2.Y, RotationXX2YY2ZZ2.X);
	const FVector4 diagonalSum = yy2xx2xx2 + zz2zz2yy2;
	const FVector4 diagonals = FVector4(1.f - diagonalSum.X, 1.f - diagonalSum.Y, 1.f - diagonalSum.Z, 1.f - diagonalSum.W);
	OutDiagonals = FVector(diagonals.X * Scale.X, diagonals.Y * Scale.Y, diagonals.Z * Scale.Z);
	// Grouping the non-diagonal elements in the rotation block by operations:
	//    ((x*y2,y*z2,x*z2) + (w*z2,w*x2,w*y2)) * scale.xyz and
	//    ((x*y2,y*z2,x*z2) - (w*z2,w*x2,w*y2)) * scale.yxz
	// Rearranging so the LHS and RHS are in the same order as for +
	//    ((x*y2,y*z2,x*z2) - (w*z2,w*x2,w*y2)) * scale.yxz

	// RotBase = x*y2, y*z2, x*z2
	// RotOffset = w*z2, w*x2, w*y2
	const FVector4 xyx = FVector4(MyRotation.X, MyRotation.Y, MyRotation.X, MyRotation.X);
	const FVector4 y2z2z2 = FVector4(RotationX2Y2Z2.Y, RotationX2Y2Z2.Z, RotationX2Y2Z2.Z, RotationX2Y2Z2.X);
	const FVector4 RotBase = FVector4::MultiplyVector4(xyx, y2z2z2);

	const FVector4 www = FVector4(MyRotation.W, MyRotation.W, MyRotation.W, MyRotation.W);
	const FVector4 z2x2y2 = FVector4(RotationX2Y2Z2.Z, RotationX2Y2Z2.X, RotationX2Y2Z2.Y, RotationX2Y2Z2.X);
	const FVector4 RotOffset = FVector4::MultiplyVector4(www, z2x2y2);

	// Adds = (RotBase + RotOffset)*Scale3D :  (x*y2 + w*z2) * Scale3D.X , (y*z2 + w*x2) * Scale3D.Y, (x*z2 + w*y2) * Scale3D.Z
	// Subtracts = (RotBase - RotOffset)*Scale3DYZX :  (x*y2 - w*z2) * Scale3D.Y , (y*z2 - w*x2) * Scale3D.Z, (x*z2 - w*y2) * Scale3D.X
	const FVector4 Adds = FVector4(RotBase.X + RotOffset.X, RotBase.Y + RotOffset.Y, RotBase.Z + RotOffset.Z, RotBase.W + RotOffset.W);
	OutAdds = FVector(Adds.X * Scale.X, Adds.Y * Scale.Y, Adds.Z * Scale.Z);

	const FVector4 Scale3DYZXW = FVector4(Scale.Y, Scale.Z, Scale.X, 1.0f);
	const FVector4 Subtracts = FVector4(RotBase.X - RotOffset.X, RotBase.Y - RotOffset.Y, RotBase.Z - RotOffset.Z, RotBase.W - RotOffset.W);
	OutSubtracts = FVector(Subtracts.X * Scale3DYZXW.X, Subtracts.Y * Scale3DYZXW.Y, Subtracts.Z * Scale3DYZXW.Z);
}

FMatrix FTransform::ToMatrixWithScale() const
{
	FMatrix OutMatrix;
	FVector Diagonals, Adds, Subtracts;

	ToMatrixInternal(Diagonals, Adds, Subtracts);

	FVector4 Diagonals4 = FVector4(Diagonals.X, Diagonals.Y, Diagonals.Z, 0.0f);

	// OutMatrix.M[0][0] = (1.0f - (yy2 + zz2)) * Scale.X;    // Diagonal.X
	// OutMatrix.M[0][1] = (xy2 + wz2) * Scale.X;             // Adds.X
	// OutMatrix.M[0][2] = (xz2 - wy2) * Scale.X;             // Subtracts.Z
	// OutMatrix.M[0][3] = 0.0f;                              // DiagonalsXYZ_W0.W
	FVector4 AddX_DC_DiagX_DC = FVector4(Adds.X, Adds.X, Diagonals4.X, Diagonals4.X);
	FVector4 SubZ_DC_DiagX_DC = FVector4(Subtracts.Z, Diagonals4.X, Diagonals4.W, Diagonals4.X);
	FVector4 Row0 = FVector4(AddX_DC_DiagX_DC.Z, AddX_DC_DiagX_DC.X, SubZ_DC_DiagX_DC.X, SubZ_DC_DiagX_DC.Z);


	// OutMatrix.M[1][0] = (xy2 - wz2) * Scale.Y;             // Subtracts.X
	// OutMatrix.M[1][1] = (1.0f - (xx2 + zz2)) * Scale.Y;    // Diagonal.Y
	// OutMatrix.M[1][2] = (yz2 + wx2) * Scale.Y;             // Adds.Y
	// OutMatrix.M[1][3] = 0.0f;                            // DiagonalsXYZ_W0.W
	FVector4 SubX_DC_DiagY_DC = FVector4(Subtracts.X, Subtracts.X, Diagonals4.Y, Diagonals4.X);
	FVector4 AddY_DC_DiagW_DC = FVector4(Adds.Y, Adds.X, Diagonals4.W, Diagonals4.X);
	FVector4 Row1 = FVector4(SubX_DC_DiagY_DC.X, SubX_DC_DiagY_DC.Z, AddY_DC_DiagW_DC.X, AddY_DC_DiagW_DC.Z);

	// OutMatrix.M[2][0] = (xz2 + wy2) * Scale.Z;             // Adds.Z
	// OutMatrix.M[2][1] = (yz2 - wx2) * Scale.Z;             // Subtracts.Y
	// OutMatrix.M[2][2] = (1.0f - (xx2 + yy2)) * Scale.Z;    // Diagonals.Z
	// OutMatrix.M[2][3] = 0.0f;                              // DiagonalsXYZ_W0.W
	FVector4 AddZ_DC_SubY_DC = FVector4(Adds.Z, Subtracts.X, Diagonals4.Y, Diagonals4.X);
	FVector4 Row2 = FVector4(AddZ_DC_SubY_DC.X, AddZ_DC_SubY_DC.Z, Diagonals4.Z, Diagonals4.W);

	// OutMatrix.M[3][0] = Translation.X;
	// OutMatrix.M[3][1] = Translation.Y;
	// OutMatrix.M[3][2] = Translation.Z;
	// OutMatrix.M[3][3] = 1.0f;
	FVector4 Row3 = FVector4(Position.X, Position.Y, Position.Z, 1.0f);

	OutMatrix.M[0][0] = Row0.X; OutMatrix.M[0][1] = Row0.Y; OutMatrix.M[0][2] = Row0.Z; OutMatrix.M[0][3] = Row0.W;
	OutMatrix.M[1][0] = Row1.X;	OutMatrix.M[1][1] = Row1.Y; OutMatrix.M[1][2] = Row1.Z; OutMatrix.M[1][3] = Row1.W;
	OutMatrix.M[2][0] = Row2.X; OutMatrix.M[2][1] = Row2.Y; OutMatrix.M[2][2] = Row2.Z; OutMatrix.M[2][3] = Row2.W;
	OutMatrix.M[3][0] = Row3.X; OutMatrix.M[3][1] = Row3.Y; OutMatrix.M[3][2] = Row3.Z; OutMatrix.M[3][3] = Row3.W;

	return OutMatrix;
}

const FTransform FTransform::Identity = {};

FTransform2 FTransform2::Concatenate(const FSlateLayoutTransform& RHS) const
{
	return Concatenate(FTransform2(FVector2D(RHS.GetScale(), RHS.GetScale()), Translation));
}
