#pragma once
#include "MathUtility.h"
#include "Core/Container/Array.h"

struct FQuat;

struct FVector
{
	float X, Y, Z;
	FVector() : X(0), Y(0), Z(0) {}
	FVector(float X, float Y, float Z) : X(X), Y(Y), Z(Z) {}
	FVector(float Scalar) : X(Scalar), Y(Scalar), Z(Scalar) {}
	// Vector(0, 0, 0)
	static const FVector ZeroVector;
	// Vector(1, 1, 1)
	static const FVector OneVector;
	// Vector(0, 0, 1)
	static const FVector UpVector;
	// Vector(0, 0, -1)
	static const FVector DownVector;
	// Vector(1, 0, 0)
	static const FVector ForwardVector;
	// Vector(1, 0, 0)
	static const FVector BackwardVector;
	// Vector(0, 1, 0)
	static const FVector RightVector;
	// Vector(0, -1, 0)
	static const FVector LeftVector;

	// Unit X Axis Vector (1, 0, 0)
	static const FVector XAxisVector;
	// Unit Y Axis Vector (0, 1, 0)
	static const FVector YAxisVector;
	// Unit Z Axis Vector (0, 0, 1)
	static const FVector ZAxisVector;

public:
	static inline FVector Zero() { return ZeroVector; }
	static inline FVector One() { return OneVector; }

	static inline FVector UnitX() { return XAxisVector; }
	static inline FVector UnitY() { return YAxisVector; }
	static inline FVector UnitZ() { return ZAxisVector; }

	static float DotProduct(const FVector& A, const FVector& B);
	static FVector CrossProduct(const FVector& A, const FVector& B);

	static FVector CompouteNormalFromThreePoints(const FVector& A, const FVector& B, const FVector& C);

	static float Distance(const FVector& V1, const FVector& V2);

	float Length() const;
	float LengthSquared() const;

	bool Normalize(float Tolerance = 1.e-8f);

	FVector GetUnsafeNormal() const;
	FVector GetSafeNormal(float Tolerance = 1.e-8f) const;

	float Dot(const FVector& Other) const;
	FVector Cross(const FVector& Other) const;

	FVector operator+(const FVector& Other) const;
	FVector& operator+=(const FVector& Other);

	FVector operator-(const FVector& Other) const;
	FVector& operator-=(const FVector& Other);

	FVector operator*(const FVector& Other) const;
	FVector operator*(float Scalar) const;
	FVector& operator*=(float Scalar);

	FVector operator/(const FVector& Other) const;
	FVector operator/(float Scalar) const;
	FVector& operator/=(float Scalar);

	FVector operator-() const;

	bool operator==(const FVector& Other) const;
	bool operator!=(const FVector& Other) const;

	inline bool operator>(const FVector& Other) const
	{
		return X > Other.X && Y > Other.Y && Z > Other.Z;
	}

	inline bool operator<(const FVector& Other) const
	{
		return X < Other.X && Y < Other.Y && Z < Other.Z;
	}

	static FVector Min(const FVector& A, const FVector& B)
	{
		return { FMath::Min(A.X, B.X), FMath::Min(A.Y, B.Y), FMath::Min(A.Z, B.Z) };
	}

	static FVector Max(const FVector& A, const FVector& B)
	{
		return { FMath::Max(A.X, B.X), FMath::Max(A.Y, B.Y), FMath::Max(A.Z, B.Z) };
	}

	static void CaculateMinMax(TArray<FVector> vertices, FVector& OutMin, FVector& OutMax);
};

inline float FVector::DotProduct(const FVector& A, const FVector& B)
{
	return A.X * B.X + A.Y * B.Y + A.Z * B.Z;
}

inline FVector FVector::CrossProduct(const FVector& A, const FVector& B)
{
	return {
		A.Y * B.Z - A.Z * B.Y,
		A.Z * B.X - A.X * B.Z,
		A.X * B.Y - A.Y * B.X
	};
}

inline FVector FVector::CompouteNormalFromThreePoints(const FVector& A, const FVector& B, const FVector& C)
{
	FVector AB = B - A;
	FVector AC = C - A;
	FVector Normal = CrossProduct(AB, AC);
	return Normal;
}

inline float FVector::Distance(const FVector& V1, const FVector& V2)
{
	return FMath::Sqrt(FMath::Square(V2.X - V1.X) + FMath::Square(V2.Y - V1.Y) + FMath::Square(V2.Z - V1.Z));
}

inline float FVector::Length() const
{
	return FMath::Sqrt(X * X + Y * Y + Z * Z);
}

inline float FVector::LengthSquared() const
{
	return X * X + Y * Y + Z * Z;
}

inline FVector FVector::GetUnsafeNormal() const
{
	const float Scale = FMath::InvSqrt(X * X + Y * Y + Z * Z);
	return { X * Scale, Y * Scale, Z * Scale };
}

inline FVector FVector::GetSafeNormal(float Tolerance) const
{
	const float SquareSum = X * X + Y * Y + Z * Z;

	// Not sure if it's safe to add tolerance in there. Might introduce too many errors
	if (SquareSum == 1.f)
	{
		return *this;
	}
	else if (SquareSum < Tolerance)
	{
		return ZeroVector;
	}
	const float Scale = FMath::InvSqrt(SquareSum);
	return { X * Scale, Y * Scale, Z * Scale };
}

inline bool FVector::Normalize(float Tolerance)
{
	const float SquareSum = X * X + Y * Y + Z * Z;
	if (SquareSum > Tolerance)
	{
		const float Scale = FMath::InvSqrt(SquareSum);
		X *= Scale; Y *= Scale; Z *= Scale;
		return true;
	}
	return false;
}

inline float FVector::Dot(const FVector& Other) const
{
	return DotProduct(*this, Other);
}

inline FVector FVector::Cross(const FVector& Other) const
{
	return CrossProduct(*this, Other);
}

inline FVector FVector::operator+(const FVector& Other) const
{
	return { X + Other.X, Y + Other.Y, Z + Other.Z };
}

inline FVector& FVector::operator+=(const FVector& Other)
{
	X += Other.X; Y += Other.Y; Z += Other.Z;
	return *this;
}

inline FVector FVector::operator-(const FVector& Other) const
{
	return { X - Other.X, Y - Other.Y, Z - Other.Z };
}

inline FVector& FVector::operator-=(const FVector& Other)
{
	X -= Other.X; Y -= Other.Y; Z -= Other.Z;
	return *this;
}

inline FVector FVector::operator*(const FVector& Other) const
{
	return { X * Other.X, Y * Other.Y, Z * Other.Z };
}

inline FVector FVector::operator*(float Scalar) const
{
	return { X * Scalar, Y * Scalar, Z * Scalar };
}

inline FVector& FVector::operator*=(float Scalar)
{
	X *= Scalar; Y *= Scalar; Z *= Scalar;
	return *this;
}

inline FVector FVector::operator/(const FVector& Other) const
{
	return { X / Other.X, Y / Other.Y, Z / Other.Z };
}

inline FVector FVector::operator/(float Scalar) const
{
	return { X / Scalar, Y / Scalar, Z / Scalar };
}

inline FVector& FVector::operator/=(float Scalar)
{
	X /= Scalar; Y /= Scalar; Z /= Scalar;
	return *this;
}

inline FVector FVector::operator-() const
{
	return { -X, -Y, -Z };
}

inline bool FVector::operator==(const FVector& Other) const
{
	return X == Other.X && Y == Other.Y && Z == Other.Z;
}

inline bool FVector::operator!=(const FVector& Other) const
{
	return X != Other.X || Y != Other.Y || Z != Other.Z;
}

inline void FVector::CaculateMinMax(TArray<FVector> vertices, FVector& OutMin, FVector& OutMax)
{
	if (vertices.Num() == 0)
	{
		OutMin = FVector::ZeroVector;
		OutMax = FVector::ZeroVector;
		return;
	}

	// 초기값: 첫번째 정점으로 설정
	OutMin = vertices[0];
	OutMax = vertices[0];

	// 각 정점을 순회하며 최소/최대값 갱신
	for (const FVector& vertex : vertices)
	{
		OutMin.X = FMath::Min(OutMin.X, vertex.X);
		OutMin.Y = FMath::Min(OutMin.Y, vertex.Y);
		OutMin.Z = FMath::Min(OutMin.Z, vertex.Z);

		OutMax.X = FMath::Max(OutMax.X, vertex.X);
		OutMax.Y = FMath::Max(OutMax.Y, vertex.Y);
		OutMax.Z = FMath::Max(OutMax.Z, vertex.Z);
	}
}
struct FVector4 : public FVector
{
	using FVector::X;
	using FVector::Y;
	using FVector::Z;

	float W;

	FVector4(const FVector& InVector, float InW)
		: FVector(InVector), W(InW)
	{
	}
	FVector4()
		: FVector(0, 0, 0), W(0)
	{
	}
	FVector4(float InX, float InY, float InZ, float InW)
		: FVector(InX, InY, InZ), W(InW)
	{
	}

	FVector4 operator+(const FVector4& Other) const;
	FVector4 operator*(float Scalar) const;

	static FVector4 MultiplyVector4(const FVector4& a, const FVector4& b);
	static FVector4 VectorQuaternionRotateVector(const FVector4& Q, const FVector4& V);
	static FVector4 CrossProduct(const FVector4& A, const FVector4& B);

	static FVector4 VectorMultiplyAdd(const FVector4& A, const FVector4& B, const FVector4& C)
	{
		return { A.X * B.X + C.X, A.Y * B.Y + C.Y, A.Z * B.Z + C.Z, A.W * B.W + C.W };
	}

	static FVector4 VectorMax(const FVector4& A, const FVector4& B)
	{
		return { FMath::Max(A.X, B.X), FMath::Max(A.Y, B.Y), FMath::Max(A.Z, B.Z), FMath::Max(A.W, B.W) };
	}

	static const FVector4 ONE;
	static const FVector4 ONENULL;

	static const FVector4 ZERO;
	static const FVector4 ZERONULL;
	static const FVector4 LEFT;
	static const FVector4 RIGHT;
	static const FVector4 UP;
	static const FVector4 DOWN;
	static const FVector4 FORWARD;
	static const FVector4 BACKWARD;

	static const FVector4 WHITE;
	static const FVector4 RED;
	static const FVector4 GREEN;
	static const FVector4 BLUE;
	static const FVector4 BLACK;

	// 두 개의 채널이 1인 색상들
	static const FVector4 YELLOW;    // 빨강 + 초록 = 노랑
	static const FVector4 CYAN;      // 초록 + 파랑 = 청록색/시안
	static const FVector4 MAGENTA;   // 빨강 + 파랑 = 자홍색/마젠타

	// 추가적인 색
	static const FVector4 ORANGE;    // 주황색
	static const FVector4 PURPLE;    // 보라색
	static const FVector4 TEAL;

	FVector4& operator/=(float Scalar);
};

inline FVector4& FVector4::operator/=(float Scalar)
{
	X /= Scalar; Y /= Scalar; Z /= Scalar; W /= Scalar;
	return *this;
}

struct FVector2D
{
    float X, Y;

    FVector2D() : X(0), Y(0) {}
    FVector2D(float InX, float InY) : X(InX), Y(InY) {}

    static const FVector2D ZeroVector;
    static const FVector2D OneVector;

	FVector2D operator+(const FVector2D& Other) const;
	FVector2D& operator+=(const FVector2D& Other);

	FVector2D operator-(const FVector2D& Other) const;
	FVector2D& operator-=(const FVector2D& Other);

	bool operator==(const FVector2D& Other) const;
	bool operator!=(const FVector2D& Other) const;

	FVector2D operator*(float Scalar) const;

    FVector2D operator/(const FVector2D& Other) const;

    FVector2D RoundToVector() const;
};