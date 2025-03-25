#pragma once

struct FVector4;
struct FVector;
struct FVector2D;
struct FQuat;
struct FQuat2;

struct alignas(16) FMatrix
{
	float M[4][4];

	FMatrix();
	FMatrix(const FVector4& InX, const FVector4& InY, const FVector4& InZ, const FVector4& InW);

	static FMatrix Identity();
	static FMatrix Transpose(const FMatrix& Matrix);
	static FMatrix GetTranslateMatrix(float X, float Y, float Z);
	static FMatrix GetTranslateMatrix(FVector Translation);
	static FMatrix GetScaleMatrix(float X, float Y, float Z);
	static FMatrix GetScaleMatrix(const FVector& InScale);
	static FMatrix GetRotateMatrix(const FQuat& Q);
	static FMatrix LookAtLH(const FVector& EyePosition, const FVector& FocusPoint, const FVector& WorldUp);
	static FMatrix PerspectiveFovLH(float FieldOfView, float AspectRatio, float NearPlane, float FarPlane);
	static FMatrix OrthoForLH(float ViewWidth, float VeiwHeight, float NearPlane, float FarPlane);
	

	FMatrix operator+(const FMatrix& Other) const;
	FMatrix operator+=(const FMatrix& Other);
	FMatrix operator-(const FMatrix& Other) const;
	FMatrix operator-=(const FMatrix& Other);
	FMatrix operator*(const FMatrix& Other) const;
	FMatrix operator*=(const FMatrix& Other);
	FMatrix operator*(float Other) const;
	FMatrix operator*=(float Other);
	bool operator==(const FMatrix& Other) const;
	bool operator!=(const FMatrix& Other) const;

	FMatrix GetTransposed() const;
	float Determinant() const;
	FMatrix Inverse() const;

	FVector GetTranslation() const;
	FVector GetScale() const;
	FVector GetRotation() const;
	FVector TransformVector(const FVector& Vector)const;
	FVector4 TransformVector(const FVector4& Vector)const;
	


	FVector4 TransformVector4(const FVector4& Vector) const;

	class FTransform GetTransform() const;
};

struct FMatrix2x2
{
	float M[2][2];
	FMatrix2x2()
	{
		M[0][0] = 1.0f; M[0][1] = 0.0f;
		M[1][0] = 0.0f;	M[1][1] = 1.0f;
	}

	FMatrix2x2(float In00, float In01, float In10, float In11)
	{
		M[0][0] = In00; M[0][1] = In01;
		M[1][0] = In10; M[1][1] = In11;
	}

	FMatrix2x2(float Scale)
	{
		M[0][0] = Scale; M[0][1] = 0.0f;
		M[1][0] = 0.0f; M[1][1] = Scale;
	};

	FMatrix2x2(const FVector2D& Scale);


	FMatrix2x2(const FQuat2& Quat);


	FVector2D TransformPoint(const FVector2D& Point) const;

	FVector2D TransformVector(const FVector2D& Vector) const;


	FMatrix2x2 Concatenate(const FMatrix2x2& RHS) const;


	FMatrix2x2 Inverse() const;


	inline bool operator== (const FMatrix2x2& Other) const
	{
		return M[0][0] == Other.M[0][0] && M[0][1] == Other.M[0][1] &&
			M[1][0] == Other.M[1][0] && M[1][1] == Other.M[1][1];
	}

	inline bool operator!= (const FMatrix2x2& Other) const
	{
		return !(*this == Other);
	}

	FVector2D GetScaleSquared() const;

	FVector2D GetScale() const;

	inline bool IsIdentity() const
	{
		return M[0][0] == 1.0f && M[0][1] == 0.0f &&
			M[1][0] == 0.0f && M[1][1] == 1.0f;
	}
};