#include "Matrix.h"

#include "Vector.h"
#include "Quat.h"
#include "Transform.h"
#include "Rotator.h"


FMatrix::FMatrix()
{
	// identity matrix
	M[0][0] = 1.0f; M[0][1] = 0.0f; M[0][2] = 0.0f; M[0][3] = 0.0f;
	M[1][0] = 0.0f; M[1][1] = 1.0f; M[1][2] = 0.0f; M[1][3] = 0.0f;
	M[2][0] = 0.0f; M[2][1] = 0.0f; M[2][2] = 1.0f; M[2][3] = 0.0f;
	M[3][0] = 0.0f; M[3][1] = 0.0f; M[3][2] = 0.0f; M[3][3] = 1.0f;
}

FMatrix::FMatrix(const FVector4& InX, const FVector4& InY, const FVector4& InZ, const FVector4& InW)
{
	// row major
	M[0][0] = InX.X; M[0][1] = InX.Y; M[0][2] = InX.Z; M[0][3] = InX.W;
	M[1][0] = InY.X; M[1][1] = InY.Y; M[1][2] = InY.Z; M[1][3] = InY.W;
	M[2][0] = InZ.X; M[2][1] = InZ.Y; M[2][2] = InZ.Z; M[2][3] = InZ.W;
	M[3][0] = InW.X; M[3][1] = InW.Y; M[3][2] = InW.Z; M[3][3] = InW.W;
}

FMatrix::FMatrix(const FRotator& Rotation)
{
	*this = RotateToMatrix(Rotation.Roll, Rotation.Pitch, Rotation.Yaw);
}

FMatrix FMatrix::Identity()
{
	return FMatrix();
}

FMatrix FMatrix::operator+(const FMatrix& Other) const
{
	FMatrix Result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			Result.M[i][j] = M[i][j] + Other.M[i][j];
		}
	}
	return Result;
}
FMatrix FMatrix::operator+=(const FMatrix& Other)
{
	*this = *this + Other;
	return *this;
}
FMatrix FMatrix::operator-(const FMatrix& Other) const
{
	FMatrix Result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			Result.M[i][j] = M[i][j] - Other.M[i][j];
		}
	}
	return Result;
}
FMatrix FMatrix::operator-=(const FMatrix& Other)
{
	*this = *this - Other;
	return *this;
}
FMatrix FMatrix::operator*(const FMatrix& Other) const
{
	FMatrix Result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			Result.M[i][j] = M[i][0] * Other.M[0][j] +
				M[i][1] * Other.M[1][j] +
				M[i][2] * Other.M[2][j] +
				M[i][3] * Other.M[3][j];
		}
	}
	return Result;
}
FMatrix FMatrix::operator*=(const FMatrix& Other)
{
	*this = *this * Other;
	return *this;
}
FMatrix FMatrix::operator*(float Other) const
{
	FMatrix Result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			Result.M[i][j] = M[i][j] * Other;
		}
	}
	return Result;
}
FMatrix FMatrix::operator*=(float Other)
{
	*this = *this * Other;
	return *this;
}
bool FMatrix::operator==(const FMatrix& Other) const
{
	bool bEqual = true;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (M[i][j] != Other.M[i][j])
			{
				bEqual = false;
				break;
			}
		}
	}

	return bEqual;
}
bool FMatrix::operator!=(const FMatrix& Other) const
{
	return !(*this == Other);
}

FMatrix FMatrix::GetTransposed() const
{
	FMatrix Result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			Result.M[i][j] = M[j][i];
		}
	}
	return Result;
}

float FMatrix::Determinant() const
{
	const float* m = &M[0][0];
	return
		m[0] * (m[5] * (m[10] * m[15] - m[11] * m[14]) - m[6] * (m[9] * m[15] - m[11] * m[13]) + m[7] * (m[9] * m[14] - m[10] * m[13])) -
		m[1] * (m[4] * (m[10] * m[15] - m[11] * m[14]) - m[6] * (m[8] * m[15] - m[11] * m[12]) + m[7] * (m[8] * m[14] - m[10] * m[12])) +
		m[2] * (m[4] * (m[9] * m[15] - m[11] * m[13]) - m[5] * (m[8] * m[15] - m[11] * m[12]) + m[7] * (m[8] * m[13] - m[9] * m[12])) -
		m[3] * (m[4] * (m[9] * m[14] - m[10] * m[13]) - m[5] * (m[8] * m[14] - m[10] * m[12]) + m[6] * (m[8] * m[13] - m[9] * m[12]));
}

FMatrix FMatrix::InverseGaussJordan(FMatrix& mat)
{
	FMatrix augmented = mat;
	FMatrix identity = FMatrix();
	for (int col = 0; col < 4; col++) {
		int pivotRow = col;
		for (int row = col + 1; row < 4; row++) {
			if (fabs(augmented.M[row][col]) > fabs(augmented.M[pivotRow][col])) {
				pivotRow = row;
			}
		}
		if (fabs(augmented.M[pivotRow][col]) < 1e-6f) {
			std::cerr << "Matrix is singular and cannot be inverted (Gauss-Jordan)." << std::endl;
			return FMatrix();
		}
		std::swap(augmented.M[col], augmented.M[pivotRow]);
		std::swap(identity.M[col], identity.M[pivotRow]);
		float pivot = augmented.M[col][col];
		for (int j = 0; j < 4; j++) {
			augmented.M[col][j] /= pivot;
			identity.M[col][j] /= pivot;
		}
		for (int row = 0; row < 4; row++) {
			if (row == col) continue;
			float factor = augmented.M[row][col];
			for (int j = 0; j < 4; j++) {
				augmented.M[row][j] -= factor * augmented.M[col][j];
				identity.M[row][j] -= factor * identity.M[col][j];
			}
		}
	}
	return identity;
}

FMatrix FMatrix::Inverse() const
{
	const float Det = Determinant();
	if (FMath::Abs(Det) < 1.0e-6f)
	{
		return {};
	}

	FMatrix Result;
	const float* m = &M[0][0];
	const float InvDet = 1.0f / Det;

	Result.M[0][0] = InvDet * (m[5] * (m[10] * m[15] - m[11] * m[14]) - m[6] * (m[9] * m[15] - m[11] * m[13]) + m[7] * (m[9] * m[14] - m[10] * m[13]));
	Result.M[0][1] = -InvDet * (m[1] * (m[10] * m[15] - m[11] * m[14]) - m[2] * (m[9] * m[15] - m[11] * m[13]) + m[3] * (m[9] * m[14] - m[10] * m[13]));
	Result.M[0][2] = InvDet * (m[1] * (m[6] * m[15] - m[7] * m[14]) - m[2] * (m[5] * m[15] - m[7] * m[13]) + m[3] * (m[5] * m[14] - m[6] * m[13]));
	Result.M[0][3] = -InvDet * (m[1] * (m[6] * m[11] - m[7] * m[10]) - m[2] * (m[5] * m[11] - m[7] * m[9]) + m[3] * (m[5] * m[10] - m[6] * m[9]));

	Result.M[1][0] = -InvDet * (m[4] * (m[10] * m[15] - m[11] * m[14]) - m[6] * (m[8] * m[15] - m[11] * m[12]) + m[7] * (m[8] * m[14] - m[10] * m[12]));
	Result.M[1][1] = InvDet * (m[0] * (m[10] * m[15] - m[11] * m[14]) - m[2] * (m[8] * m[15] - m[11] * m[12]) + m[3] * (m[8] * m[14] - m[10] * m[12]));
	Result.M[1][2] = -InvDet * (m[0] * (m[6] * m[15] - m[7] * m[14]) - m[2] * (m[4] * m[15] - m[7] * m[12]) + m[3] * (m[4] * m[14] - m[6] * m[12]));
	Result.M[1][3] = InvDet * (m[0] * (m[6] * m[11] - m[7] * m[10]) - m[2] * (m[4] * m[11] - m[7] * m[8]) + m[3] * (m[4] * m[10] - m[6] * m[8]));

	Result.M[2][0] = InvDet * (m[4] * (m[9] * m[15] - m[11] * m[13]) - m[5] * (m[8] * m[15] - m[11] * m[12]) + m[7] * (m[8] * m[13] - m[9] * m[12]));
	Result.M[2][1] = -InvDet * (m[0] * (m[9] * m[15] - m[11] * m[13]) - m[1] * (m[8] * m[15] - m[11] * m[12]) + m[3] * (m[8] * m[13] - m[9] * m[12]));
	Result.M[2][2] = InvDet * (m[0] * (m[5] * m[15] - m[7] * m[13]) - m[1] * (m[4] * m[15] - m[7] * m[12]) + m[3] * (m[4] * m[13] - m[5] * m[12]));
	Result.M[2][3] = -InvDet * (m[0] * (m[5] * m[11] - m[7] * m[9]) - m[1] * (m[4] * m[11] - m[7] * m[8]) + m[3] * (m[4] * m[9] - m[5] * m[8]));

	Result.M[3][0] = -InvDet * (m[4] * (m[9] * m[14] - m[10] * m[13]) - m[5] * (m[8] * m[14] - m[10] * m[12]) + m[6] * (m[8] * m[13] - m[9] * m[12]));
	Result.M[3][1] = InvDet * (m[0] * (m[9] * m[14] - m[10] * m[13]) - m[1] * (m[8] * m[14] - m[10] * m[12]) + m[2] * (m[8] * m[13] - m[9] * m[12]));
	Result.M[3][2] = -InvDet * (m[0] * (m[5] * m[14] - m[6] * m[13]) - m[1] * (m[4] * m[14] - m[6] * m[12]) + m[2] * (m[4] * m[13] - m[5] * m[12]));
	Result.M[3][3] = InvDet * (m[0] * (m[5] * m[10] - m[6] * m[9]) - m[1] * (m[4] * m[10] - m[6] * m[8]) + m[2] * (m[4] * m[9] - m[5] * m[8]));

	return Result;
}

FMatrix FMatrix::Transpose(const FMatrix& Matrix)
{
	FMatrix Result;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			Result.M[i][j] = Matrix.M[j][i]; // ��� ���� �ٲ� ��ġ
		}
	}
	return Result;
}

FMatrix FMatrix::GetTranslateMatrix(float X, float Y, float Z)
{
	FMatrix Result;
	Result.M[3][0] = X;
	Result.M[3][1] = Y;
	Result.M[3][2] = Z;
	return Result;
}

FMatrix FMatrix::GetTranslateMatrix(FVector Translation)
{
	return GetTranslateMatrix(Translation.X, Translation.Y, Translation.Z);
}

FMatrix FMatrix::GetScaleMatrix(float X, float Y, float Z)
{
	FMatrix Result;
	Result.M[0][0] = X;
	Result.M[1][1] = Y;
	Result.M[2][2] = Z;
	Result.M[3][3] = 1.f;
	return Result;
}

FMatrix FMatrix::GetScaleMatrix(const FVector& InScale)
{
	return GetScaleMatrix(InScale.X, InScale.Y, InScale.Z);
}

FMatrix FMatrix::GetRotateMatrix(const FQuat& Q)
{
	// 쿼터니언 요소 추출
	const float x = Q.X, y = Q.Y, z = Q.Z, w = Q.W;

	// 중간 계산값
	const float xx = x * x, yy = y * y, zz = z * z;
	const float xy = x * y, xz = x * z, yz = y * z;
	const float wx = w * x, wy = w * y, wz = w * z;

	// 회전 행렬 구성
	FMatrix Result;

	Result.M[0][0] = 1.0f - 2.0f * (yy + zz);
	Result.M[0][1] = 2.0f * (xy - wz);
	Result.M[0][2] = 2.0f * (xz + wy);
	Result.M[0][3] = 0.0f;

	Result.M[1][0] = 2.0f * (xy + wz);
	Result.M[1][1] = 1.0f - 2.0f * (xx + zz);
	Result.M[1][2] = 2.0f * (yz - wx);
	Result.M[1][3] = 0.0f;

	Result.M[2][0] = 2.0f * (xz - wy);
	Result.M[2][1] = 2.0f * (yz + wx);
	Result.M[2][2] = 1.0f - 2.0f * (xx + yy);
	Result.M[2][3] = 0.0f;

	Result.M[3][0] = 0.0f;
	Result.M[3][1] = 0.0f;
	Result.M[3][2] = 0.0f;
	Result.M[3][3] = 1.0f; // 4x4 행렬이므로 마지막 값은 1

	return Result;
}

FMatrix FMatrix::GetQuatToRotationMatrixScaleMatrix(const FQuat& q, const FVector& scale)
{
	// 쿼터니언 요소 추출
	const float x = q.X, y = q.Y, z = q.Z, w = q.W;

	// 중간 계산값
	const float xx = x * x, yy = y * y, zz = z * z;
	const float xy = x * y, xz = x * z, yz = y * z;
	const float wx = w * x, wy = w * y, wz = w * z;

	// 회전 행렬 구성
	FMatrix Result;

	Result.M[0][0] = 1.0f - 2.0f * (yy + zz) * scale.X;
	Result.M[0][1] = 2.0f * (xy - wz);
	Result.M[0][2] = 2.0f * (xz + wy);
	Result.M[0][3] = 0.0f;

	Result.M[1][0] = 2.0f * (xy + wz);
	Result.M[1][1] = 1.0f - 2.0f * (xx + zz) * scale.Y;
	Result.M[1][2] = 2.0f * (yz - wx);
	Result.M[1][3] = 0.0f;

	Result.M[2][0] = 2.0f * (xz - wy);
	Result.M[2][1] = 2.0f * (yz + wx);
	Result.M[2][2] = 1.0f - 2.0f * (xx + yy) * scale.Z;
	Result.M[2][3] = 0.0f;

	Result.M[3][0] = 0.0f;
	Result.M[3][1] = 0.0f;
	Result.M[3][2] = 0.0f;
	Result.M[3][3] = 1.0f; // 4x4 행렬이므로 마지막 값은 1

	return Result;
}

FMatrix FMatrix::LookAtLH(const FVector& EyePosition, const FVector& FocusPoint, const FVector& WorldUp)
{
	FVector Forward = (FocusPoint - EyePosition).GetSafeNormal();
	FVector Right = FVector::CrossProduct(WorldUp, Forward).GetSafeNormal();
	FVector Up = FVector::CrossProduct(Forward, Right).GetSafeNormal();

	// row major
	FMatrix Result;
	Result.M[0][0] = Right.X; Result.M[0][1] = Up.X; Result.M[0][2] = Forward.X; Result.M[0][3] = 0.0f;
	Result.M[1][0] = Right.Y; Result.M[1][1] = Up.Y; Result.M[1][2] = Forward.Y; Result.M[1][3] = 0.0f;
	Result.M[2][0] = Right.Z; Result.M[2][1] = Up.Z; Result.M[2][2] = Forward.Z; Result.M[2][3] = 0.0f;
	Result.M[3][0] = -FVector::DotProduct(Right, EyePosition); Result.M[3][1] = -FVector::DotProduct(Up, EyePosition); Result.M[3][2] = -FVector::DotProduct(Forward, EyePosition); Result.M[3][3] = 1.0f;

	return Result;
}

FMatrix FMatrix::PerspectiveFovLH(float FieldOfView, float AspectRatio, float NearPlane, float FarPlane)
{
	FMatrix Result;
	float YScale = 1.0f / tan(FieldOfView / 2.0f);
	float XScale = YScale / AspectRatio;
	Result.M[0][0] = XScale;
	Result.M[1][1] = YScale;
	Result.M[2][2] = FarPlane / (FarPlane - NearPlane);
	Result.M[2][3] = 1.0f;
	Result.M[3][2] = -NearPlane * FarPlane / (FarPlane - NearPlane);
	Result.M[3][3] = 0.0f;
	return Result;
}

FMatrix FMatrix::OrthoForLH(float ViewWidth, float VeiwHeight, float NearPlane, float FarPlane)
{
	FMatrix Result;
	Result.M[0][0] = 2 / ViewWidth;
	Result.M[1][1] = 2 / VeiwHeight;
	Result.M[2][2] = 1 / (FarPlane - NearPlane);
	Result.M[3][2] = NearPlane / (NearPlane - FarPlane);
	Result.M[3][3] = 1.0f;

	// 일반적으로 left, right, top, bottom을 받는 경우와 비교하여
	// ViewWidth = right - left;
	// ViewHeight = top - bottom
	// 으로 접근하여 작성하였습니다.

	return Result;
}

FVector FMatrix::GetTranslation() const
{
	return FVector(M[3][0], M[3][1], M[3][2]);
}

FVector FMatrix::GetScale() const
{
	float X = FVector(M[0][0], M[0][1], M[0][2]).Length();
	float Y = FVector(M[1][0], M[1][1], M[1][2]).Length();
	float Z = FVector(M[2][0], M[2][1], M[2][2]).Length();
	return { X, Y, Z };

	//return FVector(M[0][0], M[1][1], M[2][2]);
}

FVector FMatrix::GetRotation() const
{
	FQuat Q = FQuat::MakeFromRotationMatrix(*this);

	FVector Euler = Q.GetEuler();
	return Euler;
}

FVector FMatrix::TransformVector(const FVector& Vector) const
{
	return {
			Vector.X * M[0][0] + Vector.Y * M[1][0] + Vector.Z * M[2][0],
			Vector.X * M[0][1] + Vector.Y * M[1][1] + Vector.Z * M[2][1],
			Vector.X * M[0][2] + Vector.Y * M[1][2] + Vector.Z * M[2][2]
	};
}

FVector4 FMatrix::TransformVector(const FVector4& Vector) const
{
	return {
		Vector.X * M[0][0] + Vector.Y * M[1][0] + Vector.Z * M[2][0] + Vector.W * M[3][0],
		Vector.X * M[0][1] + Vector.Y * M[1][1] + Vector.Z * M[2][1] + Vector.W * M[3][1],
		Vector.X * M[0][2] + Vector.Y * M[1][2] + Vector.Z * M[2][2] + Vector.W * M[3][2],
		Vector.X * M[0][3] + Vector.Y * M[1][3] + Vector.Z * M[2][3] + Vector.W * M[3][3],
	};
}

FVector4 FMatrix::TransformVector4(const FVector4& Vector) const
{
	return {
			Vector.X * M[0][0] + Vector.Y * M[1][0] + Vector.Z * M[2][0] + Vector.W * M[3][0],
			Vector.X * M[0][1] + Vector.Y * M[1][1] + Vector.Z * M[2][1] + Vector.W * M[3][1],
			Vector.X * M[0][2] + Vector.Y * M[1][2] + Vector.Z * M[2][2] + Vector.W * M[3][2],
			Vector.X * M[0][3] + Vector.Y * M[1][3] + Vector.Z * M[2][3] + Vector.W * M[3][3]
	};
}

FTransform FMatrix::GetTransform() const
{
	FQuat RotationQuat = FQuat::MakeFromRotationMatrix(*this);
	return FTransform(GetTranslation(), RotationQuat, GetScale());
}

FMatrix FMatrix::RotateRoll(float Angle)
{
	Angle = FMath::DegreesToRadians(Angle);

	FMatrix Result;

	float C = cos(Angle);
	float S = sin(Angle);

	Result.M[1][1] = C;
	Result.M[1][2] = -S;
	Result.M[2][1] = S;
	Result.M[2][2] = C;

	return Result;
}

FMatrix FMatrix::RotatePitch(float Angle)
{
	Angle = FMath::DegreesToRadians(Angle);

	FMatrix Result;

	float C = cos(Angle);
	float S = sin(Angle);

	Result.M[0][0] = C;
	Result.M[0][2] = -S;
	Result.M[2][0] = S;
	Result.M[2][2] = C;
	return Result;
}

FMatrix FMatrix::RotateYaw(float Angle)
{
	Angle = FMath::DegreesToRadians(Angle);
	FMatrix Result;

	float C = cos(Angle);
	float S = sin(Angle);

	Result.M[0][0] = C;  // ù ��° ���� ù ��° ��
	Result.M[0][1] = -S;  // ù ��° ���� �� ��° ��
	Result.M[1][0] = S; // �� ��° ���� ù ��° ��
	Result.M[1][1] = C;  // �� ��° ���� �� ��° ��

	return Result;
}

// TODO: 벡터 받아서 Rotate 해주는 것이니까 네이밍 제대로 해주기.
FMatrix FMatrix::RotateToMatrix(float X, float Y, float Z)
{
	if (abs(Y) == 90)
	{
		return RotateRoll(X) * RotatePitch(Y) * RotateYaw(Z);
	}
	return  RotateRoll(X) * RotateYaw(Z) * RotatePitch(Y);
}

FVector FMatrix::ExtractScale(float Tolerance)
{
	FVector Scale3D(0, 0, 0);

	// For each row, find magnitude, and if its non-zero re-scale so its unit length.
	const float SquareSum0 = (M[0][0] * M[0][0]) + (M[0][1] * M[0][1]) + (M[0][2] * M[0][2]);
	const float SquareSum1 = (M[1][0] * M[1][0]) + (M[1][1] * M[1][1]) + (M[1][2] * M[1][2]);
	const float SquareSum2 = (M[2][0] * M[2][0]) + (M[2][1] * M[2][1]) + (M[2][2] * M[2][2]);

	if (SquareSum0 > Tolerance)
	{
		float Scale0 = FMath::Sqrt(SquareSum0);
		Scale3D.X = Scale0;
		float InvScale0 = 1.f / Scale0;
		M[0][0] *= InvScale0;
		M[0][1] *= InvScale0;
		M[0][2] *= InvScale0;
	}
	else
	{
		Scale3D.X = 0;
	}

	if (SquareSum1 > Tolerance)
	{
		float Scale1 = FMath::Sqrt(SquareSum1);
		Scale3D.X = Scale1;
		float InvScale1 = 1.f / Scale1;
		M[1][0] *= InvScale1;
		M[1][1] *= InvScale1;
		M[1][2] *= InvScale1;
	}
	else
	{
		Scale3D.Y = 0;
	}

	if (SquareSum2 > Tolerance)
	{
		float Scale2 = FMath::Sqrt(SquareSum2);
		Scale3D.Z = Scale2;
		float InvScale2 = 1.f / Scale2;
		M[2][0] *= InvScale2;
		M[2][1] *= InvScale2;
		M[2][2] *= InvScale2;
	}
	else
	{
		Scale3D.Z = 0;
	}

	return Scale3D;
}

void FMatrix::RemoveScaling(float Tolerance)
{
	// 첫 3행(회전-스케일 블록) 각각에 대해 스케일 크기를 구한 뒤, 
	// 해당 행의 요소들을 단위 길이로 만듭니다.
	for (int i = 0; i < 3; ++i)
	{
		float SquareSum = M[i][0] * M[i][0] + M[i][1] * M[i][1] + M[i][2] * M[i][2];
		float Scale = (SquareSum > Tolerance) ? FMath::InvSqrt(SquareSum) : float(1);
		M[i][0] *= Scale;
		M[i][1] *= Scale;
		M[i][2] *= Scale;
	}
}

FTransform FMatrix::ConstructTransformFromMatrixWithDesiredScale(const FMatrix& AMatrix, const FMatrix& BMatrix, FVector DesiredScale) const
{
	FMatrix M = AMatrix * BMatrix;
	M.RemoveScaling();

	M.SetAxis(0, M.GetAxis(0));
	M.SetAxis(1, M.GetAxis(1));
	M.SetAxis(2, M.GetAxis(2));

	FQuat Rotation = FQuat(M);
	Rotation.Normalize();

	FTransform Result;

	Result.SetScale(DesiredScale);
	Result.SetRotation(Rotation);
	M.GetTranslation().Normalize();
	Result.SetPosition(M.GetTranslation());

	return Result;
}

void FMatrix::SetAxis(int32 i, const FVector& Axis)
{
	M[i][0] = Axis.X;
	M[i][1] = Axis.Y;
	M[i][2] = Axis.Z;
}

FVector FMatrix::GetAxis(int32 i) const
{
	if (0) // x
	{
		return FVector(M[0][0], M[0][1], M[0][2]);
	}
	else if (1) // y
	{
		return FVector(M[1][0], M[1][1], M[2][2]);
	}
	else // z
	{
		return FVector(M[2][0], M[2][1], M[2][2]);
	}
}

FMatrix2x2::FMatrix2x2(const FVector2D& Scale)
{
	M[0][0] = Scale.X; M[0][1] = 0.0f;
	M[1][0] = 0.0f; M[1][1] = Scale.Y;
}

FMatrix2x2::FMatrix2x2(const FQuat2& Quat)
{
	M[0][0] = Quat.GetVector().X; M[0][1] = -Quat.GetVector().Y;
	M[1][0] = Quat.GetVector().Y; M[1][1] = Quat.GetVector().X;
}

FVector2D FMatrix2x2::TransformPoint(const FVector2D& Point) const
{
	return FVector2D(
		Point.X * M[0][0] + Point.Y * M[0][1],
		Point.X * M[1][0] + Point.Y * M[1][1]
	);
}

FVector2D FMatrix2x2::TransformVector(const FVector2D& Vector) const
{
	return FVector2D(
		Vector.X * M[0][0] + Vector.Y * M[0][1],
		Vector.X * M[1][0] + Vector.Y * M[1][1]
	);
}

FMatrix2x2 FMatrix2x2::Concatenate(const FMatrix2x2& RHS) const
{
	FMatrix2x2 Result = FMatrix2x2(
		M[0][0] * RHS.M[0][0] + M[0][1] * RHS.M[1][0], M[0][0] * RHS.M[0][1] + M[0][1] * RHS.M[1][1],
		M[1][0] * RHS.M[0][0] + M[1][1] * RHS.M[1][0], M[1][0] * RHS.M[0][1] + M[1][1] * RHS.M[1][1]
	);

	return Result;
}

FMatrix2x2 FMatrix2x2::Inverse() const
{
	float Det = M[0][0] * M[1][1] - M[0][1] * M[1][0];
	if (Det == 0.0f)
	{
		return FMatrix2x2();
	}
	float InvDet = 1.0f / Det;
	return FMatrix2x2(
		M[1][1] * InvDet, -M[0][1] * InvDet,
		-M[1][0] * InvDet, M[0][0] * InvDet
	);
}

FVector2D FMatrix2x2::GetScaleSquared() const
{
	return FVector2D(M[0][0] * M[0][0] + M[0][1] * M[0][1], M[1][0] * M[1][0] + M[1][1] * M[1][1]);
}

FVector2D FMatrix2x2::GetScale() const
{
	return FVector2D(FMath::Sqrt(M[0][0] * M[0][0] + M[0][1] * M[0][1]), FMath::Sqrt(M[1][0] * M[1][0] + M[1][1] * M[1][1]));
}