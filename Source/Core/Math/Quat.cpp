#include "Quat.h"
#include "Core/Math/Matrix.h"

FQuat FQuat::AxisAngleToQuaternion(const FVector& Axis, float AngleInDegrees) {
    float AngleInRadians = FMath::DegreesToRadians(AngleInDegrees);
    float HalfAngle = AngleInRadians * 0.5f;
    float s = sinf(HalfAngle);
    return FQuat(
        Axis.X * s,
        Axis.Y * s,
        Axis.Z * s,
        cosf(HalfAngle)
    );
}

FQuat::FQuat(const FMatrix& M)
{
    //const MeReal *const t = (MeReal *) tm;
    float s;

    // Check diagonal (trace)
    const float tr = M.M[0][0] + M.M[1][1] + M.M[2][2];

    if (tr > 0.0f)
    {
        float InvS = FMath::InvSqrt(tr + float(1.f));
        this->W = float(float(0.5f) * (float(1.f) / InvS));
        s = float(0.5f) * InvS;

        this->X = ((M.M[1][2] - M.M[2][1]) * s);
        this->Y = ((M.M[2][0] - M.M[0][2]) * s);
        this->Z = ((M.M[0][1] - M.M[1][0]) * s);
    }
    else
    {
        // diagonal is negative
        int32 i = 0;

        if (M.M[1][1] > M.M[0][0])
            i = 1;

        if (M.M[2][2] > M.M[i][i])
            i = 2;

        static constexpr int32 nxt[3] = { 1, 2, 0 };
        const int32 j = nxt[i];
        const int32 k = nxt[j];

        s = M.M[i][i] - M.M[j][j] - M.M[k][k] + (1.0f);

        float InvS = FMath::InvSqrt(s);

        float qt[4];
        qt[i] = float(0.5f) * (float(1.f) / InvS);

        s = float(0.5f) * InvS;

        qt[3] = (M.M[j][k] - M.M[k][j]) * s;
        qt[j] = (M.M[i][j] + M.M[j][i]) * s;
        qt[k] = (M.M[i][k] + M.M[k][i]) * s;

        this->X = qt[0];
        this->Y = qt[1];
        this->Z = qt[2];
        this->W = qt[3];
    }
}

FQuat FQuat::EulerToQuaternion(FVector Euler)
{
    const float roll = FMath::DegreesToRadians(Euler.X);
    const float pitch = FMath::DegreesToRadians(Euler.Y);
    const float yaw = FMath::DegreesToRadians(Euler.Z);

    const float cr = FMath::Cos(roll * 0.5f);
    const float sr = FMath::Sin(roll * 0.5f);
    const float cp = FMath::Cos(pitch * 0.5f);
    const float sp = FMath::Sin(pitch * 0.5f);
    const float cy = FMath::Cos(yaw * 0.5f);
    const float sy = FMath::Sin(yaw * 0.5f);

    FQuat q;
    q.W = cr * cp * cy + sr * sp * sy;
    q.X = sr * cp * cy - cr * sp * sy;
    q.Y = cr * sp * cy + sr * cp * sy;
    q.Z = cr * cp * sy - sr * sp * cy;

    return q;
}

FVector FQuat::QuaternionToEuler(const FQuat& Quat) {
    FVector angles;

    // roll (x-axis rotation)
    const float sinr_cosp = 2.0f * (Quat.W * Quat.X + Quat.Y * Quat.Z);
    const float cosr_cosp = 1.0f - 2.0f * (Quat.X * Quat.X + Quat.Y * Quat.Y);
    angles.X = FMath::Atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    const float sinp = FMath::Sqrt(1.0f + 2.0f * (Quat.W * Quat.Y - Quat.X * Quat.Z));
    const float cosp = FMath::Sqrt(1.0f - 2.0f * (Quat.W * Quat.Y - Quat.X * Quat.Z));
    angles.Y = 2 * FMath::Atan2(sinp, cosp) - PI / 2;

    // yaw (z-axis rotation)
    const float siny_cosp = 2.0f * (Quat.W * Quat.Z + Quat.X * Quat.Y);
    const float cosy_cosp = 1.0f - 2.0f * (Quat.Y * Quat.Y + Quat.Z * Quat.Z);
    angles.Z = FMath::Atan2(siny_cosp, cosy_cosp);

    angles.X = FMath::RadiansToDegrees(angles.X);
    angles.Y = FMath::RadiansToDegrees(angles.Y);
    angles.Z = FMath::RadiansToDegrees(angles.Z);

    return angles;
}

FQuat FQuat::AddQuaternions(const FQuat& q1, const FQuat& q2) {
    return FQuat(
        q1.X + q2.X,
        q1.Y + q2.Y,
        q1.Z + q2.Z,
        q1.W + q2.W
    );
}

FQuat FQuat::MultiplyQuaternions(const FQuat& q1, const FQuat& q2) {
    return FQuat(
        q1.W * q2.X + q1.X * q2.W + q1.Y * q2.Z - q1.Z * q2.Y, // X
        q1.W * q2.Y - q1.X * q2.Z + q1.Y * q2.W + q1.Z * q2.X, // Y
        q1.W * q2.Z + q1.X * q2.Y - q1.Y * q2.X + q1.Z * q2.W, // Z
        q1.W * q2.W - q1.X * q2.X - q1.Y * q2.Y - q1.Z * q2.Z  // W
    );
}

FQuat FQuat::SubtractQuaternions(const FQuat& q1, const FQuat& q2) {
    return FQuat(
        q1.X - q2.X,
        q1.Y - q2.Y,
        q1.Z - q2.Z,
        q1.W - q2.W
    );
}

FQuat FQuat::MakeFromRotationMatrix(const FMatrix& M)
{
    FQuat Q;

    float trace = M.M[0][0] + M.M[1][1] + M.M[2][2]; // 행렬의 Trace 값 (대각합)

    if (trace > 0.0f)
    {
        float S = sqrtf(trace + 1.0f) * 2.0f; // S는 4배의 W
        Q.W = 0.25f * S;
        Q.X = (M.M[2][1] - M.M[1][2]) / S;
        Q.Y = (M.M[0][2] - M.M[2][0]) / S;
        Q.Z = (M.M[1][0] - M.M[0][1]) / S;
    }
    else
    {
        if (M.M[0][0] > M.M[1][1] && M.M[0][0] > M.M[2][2])
        {
            float S = sqrtf(1.0f + M.M[0][0] - M.M[1][1] - M.M[2][2]) * 2.0f;
            Q.W = (M.M[2][1] - M.M[1][2]) / S;
            Q.X = 0.25f * S;
            Q.Y = (M.M[0][1] + M.M[1][0]) / S;
            Q.Z = (M.M[0][2] + M.M[2][0]) / S;
        }
        else if (M.M[1][1] > M.M[2][2])
        {
            float S = sqrtf(1.0f + M.M[1][1] - M.M[0][0] - M.M[2][2]) * 2.0f;
            Q.W = (M.M[0][2] - M.M[2][0]) / S;
            Q.X = (M.M[0][1] + M.M[1][0]) / S;
            Q.Y = 0.25f * S;
            Q.Z = (M.M[1][2] + M.M[2][1]) / S;
        }
        else
        {
            float S = sqrtf(1.0f + M.M[2][2] - M.M[0][0] - M.M[1][1]) * 2.0f;
            Q.W = (M.M[1][0] - M.M[0][1]) / S;
            Q.X = (M.M[0][2] + M.M[2][0]) / S;
            Q.Y = (M.M[1][2] + M.M[2][1]) / S;
            Q.Z = 0.25f * S;
        }
    }

    return Q;
}

FVector FQuat::RotateVector(const FVector& V) const
{
    // v' = q * v * q^(-1) 구현

    // 벡터를 순수 쿼터니언으로 변환 (실수부 0, 벡터부 V)
    FQuat VectorQuat(V.X, V.Y, V.Z, 0.0f);
    FQuat Conjugate = GetInverse();
    FQuat temp = MultiplyQuaternions(*this, VectorQuat);

    // 쿼터니언 회전 수행: q * v * q^(-1)
    FQuat Result = MultiplyQuaternions(temp, Conjugate);

    // 결과에서 벡터 부분만 추출
    return FVector(Result.X, Result.Y, Result.Z);
}

FVector4 FQuat::VectorQuaternionRotateVector(const FQuat& Quat, FVector4 VectorW0)
{
    const FQuat QW = FQuat(Quat.Z, Quat.Z, Quat.Z, Quat.Z);
    FVector4 T = FVector4::CrossProduct(Quat, VectorW0);
    T = FVector4(T.X + T.X, T.Y + T.Y, T.Z + T.Z, T.W + T.W);

    const FVector4 VTemp0 = FVector4::VectorMultiplyAdd(QW, T, VectorW0);
    const FVector4 VTemp1 = FVector4::CrossProduct(Quat, T);
    const FVector4 Rotated = FVector4(VTemp0.X + VTemp1.X, VTemp0.Y + VTemp1.Y, VTemp0.Z + VTemp1.Z, VTemp0.W + VTemp1.W);
    return Rotated;
}

FVector4 FQuat::VectorQuaternionInverseRotatedVector(const FQuat& Q, const FVector4& W0)
{
    const FQuat QInv = FQuat(-Q.X, -Q.Y, -Q.Z, -Q.W);
    return VectorQuaternionRotateVector(QInv, W0);
}

bool FQuat::Equals(const FQuat& Other, float Tolerance) const
{
    const FVector A(X, Y, Z);
    const FVector B(Other.X, Other.Y, Other.Z);

    FVector NormalDelta = (A - B).GetSafeNormal();
    FVector AbsDelta = FVector(FMath::Abs(NormalDelta.X), FMath::Abs(NormalDelta.Y), FMath::Abs(NormalDelta.Z));

    return AbsDelta.X <= Tolerance && AbsDelta.Y <= Tolerance && AbsDelta.Z <= Tolerance && FMath::Abs(W - Other.W) <= Tolerance;
}

FQuat FQuat::Normalized() const
{
    // 쿼터니언의 제곱 노름(크기의 제곱) 계산
    const float SqNorm = X * X + Y * Y + Z * Z + W * W;

    // 아주 작은 값인지 확인 (SMALL_NUMBER는 충분히 작은 값, 예: 1e-8f)
    if (SqNorm > SMALL_NUMBER)
    {
        // 역제곱근 계산
        const float InvNorm = 1.0f / sqrtf(SqNorm);
        return FQuat(X * InvNorm, Y * InvNorm, Z * InvNorm, W * InvNorm);
    }

    // 크기가 0에 가까우면 항등 쿼터니언 반환 (회전 없음)
    return FQuat(0.0f, 0.0f, 0.0f, 1.0f);
}

FQuat& FQuat::operator*(const FQuat& q)
{
    X *= q.X;
    Y *= q.Y;
    Z *= q.Z;
    W *= q.W;
    
    return *this;
}


const FQuat FQuat::Identity = FQuat(0.0f, 0.0f, 0.0f, 1.0f);
