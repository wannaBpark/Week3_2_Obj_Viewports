#pragma once
#include <Core/Math/Transform.h>

typedef FTransform2 FSlateRenderTransform;

class FArrangedWidget;
class SWidget;
struct FMargin;

/**
 * FSlateRect
 * 
 * Slate 위젯의 레이아웃 또는 클리핑 영역을 나타내는 사각형을 표현합니다.
 */
struct FSlateRect
{
public:
	float Left;   // 사각형의 왼쪽 경계
	float Top;    // 사각형의 위쪽 경계
	float Right;  // 사각형의 오른쪽 경계
	float Bottom; // 사각형의 아래쪽 경계

	// 생성자: 각 경계를 지정하며 기본값은 -1 입니다.
	explicit FSlateRect(float InLeft = -1, float InTop = -1, float InRight = -1, float InBottom = -1)
		: Left(InLeft)
		, Top(InTop)
		, Right(InRight)
		, Bottom(InBottom)
	{
	}

	// 생성자: 시작 위치와 끝 위치를 이용하여 사각형을 생성합니다.
	FSlateRect(const FVector2D& InStartPos, const FVector2D& InEndPos)
		: Left(InStartPos.X)
		, Top(InStartPos.Y)
		, Right(InEndPos.X)
		, Bottom(InEndPos.Y)
	{
	}

	/**
	 * TransformRect
	 * 
	 * 주어진 레이아웃 변환(Scale+Translate)을 Rect에 적용하여 변환된 사각형을 반환합니다.
	 */
	static inline FSlateRect TransformRect(const FSlateLayoutTransform& Transform, const FSlateRect& Rect)
	{
		FVector2D TopLeftTransformed = Transform.TransformPoint(FVector2D(Rect.Left, Rect.Top));
		FVector2D BottomRightTransformed = Transform.TransformPoint(FVector2D(Rect.Right, Rect.Bottom));

		// 변환 후 좌표 순서가 뒤바뀌었으면 스왑합니다.
		if (TopLeftTransformed.X > BottomRightTransformed.X)
		{
			std::swap(TopLeftTransformed.X, BottomRightTransformed.X);
		}
		if (TopLeftTransformed.Y > BottomRightTransformed.Y)
		{
			std::swap(TopLeftTransformed.Y, BottomRightTransformed.Y);
		}

		return FSlateRect(TopLeftTransformed, BottomRightTransformed);
	}

	FSlateRect InsetBy(const struct FMargin& InsetAmount) const; // 지정된 여백만큼 안으로 축소한 사각형을 반환
	FSlateRect ExtendBy(const FMargin& ExtendAmount) const;       // 지정된 여백만큼 바깥으로 확장한 사각형을 반환

	inline FVector2D GetTopLeft() const
	{
		return FVector2D(Left, Top); // 사각형의 왼쪽 위 모서리 좌표 반환
	}

	inline FVector2D GetSize() const
	{
		return FVector2D(Right - Left, Bottom - Top); // 사각형의 가로, 세로 크기 반환
	}
};


/**
 * FSlateRotatedRect
 * 
 * 회전된 사각형을 나타냅니다. 이는 정렬된 사각형을 회전시킨 결과로, 회전 후의 왼쪽 위 좌표와 X, Y 방향의 확장(길이)를 저장합니다.
 */
struct FSlateRotatedRect
{
public:
    /** 기본 생성자. */
    FSlateRotatedRect() {}

    /** 주어진 정렬된 사각형(AlignedRect)으로부터 회전된 사각형을 생성합니다. */
    explicit FSlateRotatedRect(const FSlateRect& AlignedRect)
        : TopLeft(AlignedRect.Left, AlignedRect.Top)
        , ExtentX(AlignedRect.Right - AlignedRect.Left, 0.0f)
        , ExtentY(0.0f, AlignedRect.Bottom - AlignedRect.Top)
    {
    }

    /** 각 요소를 개별적으로 지정하는 생성자. */
    FSlateRotatedRect(const FVector2D& InTopLeft, const FVector2D& InExtentX, const FVector2D& InExtentY)
        : TopLeft(InTopLeft)
        , ExtentX(InExtentX)
        , ExtentY(InExtentY)
    {
    }

public:
    /** 변환된(회전된) 왼쪽 위 모서리 좌표. */
    FVector2D TopLeft;
    /** X축 방향(오른쪽-왼쪽)의 확장량. */
    FVector2D ExtentX;
    /** Y축 방향(아래-위)의 확장량. */
    FVector2D ExtentY;

public:
    bool operator == (const FSlateRotatedRect& Other) const
    {
        return
            TopLeft == Other.TopLeft &&
            ExtentX == Other.ExtentX &&
            ExtentY == Other.ExtentY;
    }

public:
    /** 회전된 사각형을 감싸는 정렬된 사각형(Bounding Rect)으로 변환합니다. */
    FSlateRect ToBoundingRect() const;

    /** 지정된 위치가 이 회전된 사각형 내부에 있는지 여부를 판단합니다. */
    bool IsUnderLocation(const FVector2D& Location) const;

    // 여러 가지 방식으로 회전된 사각형을 만드는 정적 메서드들:
    static FSlateRotatedRect MakeRotatedRect(const FSlateRect& ClipRectInLayoutWindowSpace, const FSlateLayoutTransform& InverseLayoutTransform, const FSlateRenderTransform& RenderTransform)
    {
        return MakeRotatedRect(ClipRectInLayoutWindowSpace, InverseLayoutTransform.Concatenate(RenderTransform));
    }

    static FSlateRotatedRect MakeRotatedRect(const FSlateRect& ClipRectInLayoutWindowSpace, const FTransform2& LayoutToRenderTransform);

    static FSlateRotatedRect MakeSnappedRotatedRect(const FSlateRect& ClipRectInLayoutWindowSpace, const FSlateLayoutTransform& InverseLayoutTransform, const FSlateRenderTransform& RenderTransform)
    {
        return MakeSnappedRotatedRect(ClipRectInLayoutWindowSpace, InverseLayoutTransform.Concatenate(RenderTransform));
    }

    /**
     * 픽셀 경계에 맞게 스냅된(rotated) 사각형을 생성합니다.
     * 이 함수는 정렬된 클립 사각형과 지오메트리의 레이아웃 및 렌더 변환 집합을 사용합니다.
     * 반환값은 float 또는 float16 버전의 사각형입니다(typedef에 따라).
     */
    static FSlateRotatedRect MakeSnappedRotatedRect(const FSlateRect& ClipRectInLayoutWindowSpace, const FTransform2& LayoutToRenderTransform);

    inline static FSlateRotatedRect TransformRect(const FTransform2& Transform, const FSlateRotatedRect& Rect)
    {
        return FSlateRotatedRect
        (
            Transform.TransformPoint(Rect.TopLeft),
            Transform.TransformVector(Rect.ExtentX),
            Transform.TransformVector(Rect.ExtentY)
        );
    }
};



/**
 * FLayoutGeometry
 * 
 * 위젯의 레이아웃 기하학 정보를 나타냅니다.
 * 이 구조체는 위젯의 로컬 공간에서의 크기, 부모 좌표계로의 변환(레이아웃 변환)을 저장하며,
 * 이를 통해 위젯이 부모 내에서 차지하는 영역이나 위치를 계산할 수 있습니다.
 */
class FLayoutGeometry
{
public:
	FLayoutGeometry()
		: LocalSize(1.0f, 1.0f)
	{
	}

	// 로컬 크기와 부모에 대한 로컬 변환(레이아웃 변환)을 지정하여 생성
	explicit FLayoutGeometry(const FSlateLayoutTransform& InLocalToParent, const FVector2D& SizeInLocalSpace)
		: LocalToParent(InLocalToParent)
		, LocalSize(SizeInLocalSpace)
	{
	}

	/** 부모 좌표계로의 로컬 변환(레이아웃 변환)을 반환합니다. */
	const FSlateLayoutTransform& GetLocalToParentTransform() const
	{
		return LocalToParent;
	}

	/** 위젯의 로컬 공간에서의 크기를 반환합니다. */
	FVector2D GetSizeInLocalSpace() const
	{
		return LocalSize;
	}

	/** 부모 좌표계에서의 크기를 반환합니다. */
	FVector2D GetSizeInParentSpace() const
	{
		return LocalToParent.TransformVector(LocalSize);
	}

	/** 부모 좌표계에서의 오프셋(위젯의 위치)을 반환합니다. */
	FVector2D GetOffsetInParentSpace() const
	{
		return LocalToParent.GetTranslation();
	}

	/** 로컬 공간에서의 사각형(위젯 영역)을 반환합니다. */
	FSlateRect GetRectInLocalSpace() const
	{
		return FSlateRect(FVector2D(0.0f, 0.0f), LocalSize);
	}

	/** 부모 좌표계에서의 사각형(위젯 영역)을 반환합니다. */
	FSlateRect GetRectInParentSpace() const
	{
		return FSlateRect::TransformRect(LocalToParent, GetRectInLocalSpace());
	}

private:
	FSlateLayoutTransform LocalToParent; // 부모로부터의 로컬 변환
	FVector2D LocalSize;                 // 로컬 공간에서의 크기
};


/**
 * FPaintGeometry
 * 
 * FPaintGeometry는 화면(윈도우) 공간(드로잉 공간)에서 요소를 그리기 위한 기하학적 정보를 포함합니다.
 * 여기에는 위젯의 로컬 공간에서의 크기와 해당 요소를 윈도우 공간에 배치하기 위해 필요한 변환 정보가 포함됩니다.
 *
 * DrawPosition, DrawSize, DrawScale 등은 과거 유산(legacy)으로 남아있으며, 이제는 사용되지 않아야 합니다.
 *
 *    - DrawPosition과 DrawSize는 이미 변환되어 그리기 코드에서 바로 사용될 수 있는 값입니다.
 *
 *    - DrawScale은 내부적으로 개별 그리기 요소(예: 선 두께, 3x3 그리드 여백 등)에만 적용됩니다.
 */
struct FPaintGeometry
{
    /**
     * !!! DEPRECATED!!!
     * 드로잉은 이제 로컬 공간에서만 이루어져야 합니다. (렌더 트랜스폼이 제대로 작동하도록)
     * WARNING: 이 레거시 멤버는 LAYOUT 공간에 해당하며, 렌더 트랜스폼은 고려하지 않습니다.
     *
     * 요소를 그릴 때 사용할 렌더 공간(실제 그리기 공간) 위치입니다.
     */
    FVector2D DrawPosition;

    /**
     * !!! DEPRECATED!!!
     * 드로잉은 이제 로컬 공간에서만 이루어져야 합니다. (렌더 트랜스폼이 제대로 작동하도록)
     * WARNING: 이 레거시 멤버는 LAYOUT 공간에 해당하며, 렌더 트랜스폼은 고려하지 않습니다.
     *
     * 개별 그리기 요소의 그리기 관련 속성에만 영향을 줍니다. (예: 선 두께, 3x3 그리드 여백 등)
     */
    float DrawScale;

    /** 로컬 공간에서의 기하학적 크기를 반환합니다.
     *  레거시 생성자가 사용된 경우, 먼저 CommitTransformsIfUsingLegacyConstructor()를 호출해야 합니다.
     */
    FVector2D GetLocalSize() const { return LocalSize; }

    /** 최종 렌더 트랜스폼(로컬 공간 → 윈도우 공간)으로 변환된 값을 반환합니다.
     *  레거시 생성자가 사용된 경우, 먼저 CommitTransformsIfUsingLegacyConstructor()를 호출해야 합니다.
     */
    const FSlateRenderTransform& GetAccumulatedRenderTransform() const { return AccumulatedRenderTransform; }

    /**
     * 레거시 생성자를 사용하여 윈도우 공간에서 생성된 기하학적 정보를 지원합니다.
     * 이 경우, 모든 멤버 변경이 완료될 때까지 RenderTransform과 LocalSize의 계산을 지연시킵니다.
     * WARNING: 레거시 방식은 렌더 트랜스폼을 지원하지 않습니다!
     */
    void CommitTransformsIfUsingLegacyConstructor() const
    {
        if (!bUsingLegacyConstructor) return;

        AccumulatedRenderTransform = FSlateRenderTransform(DrawScale, DrawPosition);
        FSlateLayoutTransform AccumulatedLayoutTransform = FSlateLayoutTransform(DrawScale, DrawPosition);
        LocalSize = AccumulatedLayoutTransform.Inverse().TransformVector(DrawSize);
    }

    bool HasRenderTransform() const { return bHasRenderTransform; }

private:
    mutable FVector2D DrawSize;  // 레거시 생성자를 지원하기 위해 mutable로 선언 (렌더 트랜스폼 고려하지 않음)
    mutable FVector2D LocalSize; // 레거시 생성자 지원을 위한 mutable
    mutable FSlateRenderTransform AccumulatedRenderTransform; // 최종 렌더 트랜스폼 (로컬 → 윈도우)
    uint8 bUsingLegacyConstructor : 1; // 레거시 생성자 사용 여부
    uint8 bHasRenderTransform : 1;       // 렌더 트랜스폼이 있는지 여부

public:
    /** 기본 생성자. */
    FPaintGeometry()
        : DrawPosition(0.0f, 0.0f)
        , DrawScale(1.0f)
        , DrawSize(0.0f, 0.0f)
        , LocalSize(0.0f, 0.0f)
        , AccumulatedRenderTransform()
        , bUsingLegacyConstructor(true)
        , bHasRenderTransform(false)
    {
    }

    /**
     * 새로운 인스턴스를 생성 및 초기화합니다.
     *
     * @param InLocalSize                   로컬 공간에서의 크기
     * @param InAccumulatedLayoutTransform    누적 레이아웃 변환 (FGeometry에서 가져옴)
     * @param InAccumulatedRenderTransform    누적 렌더 트랜스폼 (FGeometry에서 가져옴)
     */
    FPaintGeometry(const FSlateLayoutTransform& InAccumulatedLayoutTransform, const FSlateRenderTransform& InAccumulatedRenderTransform, const FVector2D& InLocalSize, bool bInHasRenderTransform)
        : DrawPosition(InAccumulatedLayoutTransform.GetTranslation())
        , DrawScale(InAccumulatedLayoutTransform.GetScale())
        , DrawSize(0.0f, 0.0f)
        , LocalSize(InLocalSize)
        , AccumulatedRenderTransform(InAccumulatedRenderTransform)
        , bUsingLegacyConstructor(false)
        , bHasRenderTransform(bInHasRenderTransform)
    {
    }

    // !!! DEPRECATED!!! 레거시 생성자, 제거되어야 함.
    FPaintGeometry(const FVector2D& InDrawPosition, const FVector2D& InDrawSize, float InDrawScale)
        : DrawPosition(InDrawPosition)
        , DrawScale(InDrawScale)
        , DrawSize(InDrawSize)
        , LocalSize(0.0f, 0.0f)
        , AccumulatedRenderTransform()
        , bUsingLegacyConstructor(true)
        , bHasRenderTransform(false)
    {
    }

    /**
     * 레이아웃 변환을 PaintGeometry에 추가하는 특수 함수.
     * 이 함수는 FGeometry가 데스크톱 공간에 배치되어 있고, 루트 데스크톱 변환을 취소하여 윈도우 공간으로 전환해야 할 때 사용됩니다.
     * 만약 이 함수를 사용하려 한다면, 더 나은 방법이 있는지 문의하십시오.
     *
     * @param LayoutTransform    PaintGeometry에 추가할 추가 레이아웃 변환.
     */
    void AppendTransform(const FSlateLayoutTransform& LayoutTransform)
    {
        FTransform2 LayoutTransform2x2 = FTransform2(LayoutTransform.GetScale(), LayoutTransform.GetTranslation());
        AccumulatedRenderTransform = AccumulatedRenderTransform.Concatenate(LayoutTransform2x2);
        DrawPosition = LayoutTransform.TransformPoint(DrawPosition);
        DrawScale = LayoutTransform.GetScale() * DrawScale;
    }

    /**
     * 레이아웃 변환 대신, 렌더 트랜스폼을 새로 지정하는 특수 함수.
     *
     * @param RenderTransform    PaintGeometry에 설정할 렌더 트랜스폼.
     */
    void SetRenderTransform(const FSlateRenderTransform& RenderTransform)
    {
        AccumulatedRenderTransform = RenderTransform;
    }
};



class FGeometry
{
public:
    FGeometry();
    ~FGeometry();

    FGeometry(const FGeometry& Other) = default;
    FGeometry& operator=(const FGeometry& Other);

    FGeometry(const FVector2D& OffsetFromParent, const FVector2D& ParentAbsolutePosition, const FVector2D& InLocalSize, float InScale)
		: LocalSize(InLocalSize)
		, AbsoluteScale(1.0f)
		, AbsolutePosition(0.0f, 0.0f)
		, bHasRenderTransform(false)
	{
		FVector2D LayOutOffset = FVector2D(InScale * OffsetFromParent.X, InScale * OffsetFromParent.Y);

		FSlateLayoutTransform ParentAccumulatedLayoutTransform(InScale, ParentAbsolutePosition);
		FSlateLayoutTransform LocalLayoutTransform(LayOutOffset);

		FSlateLayoutTransform AccumulatedLayoutTransform = ParentAccumulatedLayoutTransform.Concatenate(LocalLayoutTransform);

		AccumulatedRenderTransform = FSlateRenderTransform(AccumulatedLayoutTransform.GetScale(), AccumulatedLayoutTransform.GetTranslation());
	
		AbsolutePosition = AccumulatedLayoutTransform.GetTranslation();
		AbsoluteScale = AccumulatedLayoutTransform.GetScale();
		LocalPosition = LocalLayoutTransform.GetTranslation();
	}

private:
	// 생성자: 로컬 렌더 변환 및 피벗 보정을 포함하여 부모 누적 변환과 합성된 상태로 초기화합니다.
	FGeometry(const FVector2D& InLocalSize, const FSlateLayoutTransform& InLocalLayoutTransform, const FSlateRenderTransform& InLocalRenderTransform, const FVector2D& InLocalRenderTransformPivot, const FSlateLayoutTransform& ParentAccumulatedLayoutTransform, const FSlateRenderTransform& ParentAccumulatedRenderTransform)
		: LocalSize(InLocalSize)
		, AbsoluteScale(1.0f)
		, AbsolutePosition(0.0f, 0.0f)
		, bHasRenderTransform(false)
    {
        // 1. 피벗을 로컬 공간의 원점으로 옮기기 위한 오프셋 계산 (음수 스케일)
        FVector2D negativePivotOffset = FVector2D(
            -InLocalRenderTransformPivot.X * InLocalSize.X,
            -InLocalRenderTransformPivot.Y * InLocalSize.Y
        );

        // 2. 로컬 렌더 변환 행렬 적용
        FMatrix2x2 renderMat = InLocalRenderTransform.GetMatrix();
        FVector2D transformedNegativeOffset = renderMat.TransformPoint(negativePivotOffset);
        FVector2D renderTranslation = InLocalRenderTransform.GetTranslation();
        FVector2D combinedRenderTranslation = transformedNegativeOffset + renderTranslation;
        FTransform2 pivotOffsetTransform = FTransform2(renderMat, combinedRenderTranslation);

        // 3. 원래 피벗 위치 (스케일 적용)
        FVector2D scaledPivot = FVector2D(
            InLocalRenderTransformPivot.X * InLocalSize.X,
            InLocalRenderTransformPivot.Y * InLocalSize.Y
        );

        // 4. 피벗 오프셋 변환에 원래 피벗을 적용하여 피벗 보정
        FMatrix2x2 pivotOffsetMat = pivotOffsetTransform.GetMatrix();
        FVector2D transformedPivot = pivotOffsetMat.TransformPoint(scaledPivot);
        FVector2D pivotOffsetTranslation = pivotOffsetTransform.GetTranslation();
        FVector2D combinedPivotTranslation = transformedPivot + pivotOffsetTranslation;
        FTransform2 pivotCorrectedTransform = FTransform2(pivotOffsetMat, combinedPivotTranslation);

        // 5. 로컬 렌더 변환과 부모 누적 변환을 순차적으로 적용
        FTransform2 localAndRenderTransform = pivotCorrectedTransform.Concatenate(InLocalRenderTransform);
        AccumulatedRenderTransform = localAndRenderTransform.Concatenate(ParentAccumulatedRenderTransform);
    
		FSlateLayoutTransform AccumulatedLayoutTransform = InLocalLayoutTransform.Concatenate(ParentAccumulatedLayoutTransform);
		AbsolutePosition = AccumulatedLayoutTransform.GetTranslation();
		AbsoluteScale = AccumulatedLayoutTransform.GetScale();
		LocalPosition = InLocalLayoutTransform.GetTranslation();
    }

	// 생성자: 렌더 변환 없이 레이아웃 변환만을 부모 누적 변환과 합성하여 초기화합니다.
	FGeometry(
		const FVector2D& InLocalSize,
		const FSlateLayoutTransform& InLocalLayoutTransform,
		const FSlateLayoutTransform& ParentAccumulatedLayoutTransform,
		const FSlateRenderTransform& ParentAccumulatedRenderTransform,
		bool bParentHasRenderTransform)
		: LocalSize(InLocalSize)
		, AbsoluteScale(1.0f)
		, AbsolutePosition(0.0f, 0.0f)
		, bHasRenderTransform(bParentHasRenderTransform)
	{
		FSlateLayoutTransform AccumulatedLayoutTransform = InLocalLayoutTransform.Concatenate(ParentAccumulatedLayoutTransform);
		FMatrix renderMat = AccumulatedLayoutTransform.ToMatrix();
		FVector2D renderTranslation = AccumulatedLayoutTransform.GetTranslation();
		FMatrix2x2 renderMat2x2 = FMatrix2x2(renderMat.M[0][0], renderMat.M[0][1], renderMat.M[1][0], renderMat.M[1][1]);
		AccumulatedRenderTransform = FSlateRenderTransform(renderMat2x2, renderTranslation);

		AbsolutePosition = AccumulatedLayoutTransform.GetTranslation();
		AbsoluteScale = AccumulatedLayoutTransform.GetScale();
		LocalPosition = InLocalLayoutTransform.GetTranslation();
	}

public:
	bool operator== (const FGeometry& Other) const
	{
		return LocalSize == Other.LocalSize && AbsoluteScale == Other.AbsoluteScale && AbsolutePosition == Other.AbsolutePosition && LocalPosition == Other.LocalPosition;
	}

	bool operator!= (const FGeometry& Other) const
	{
		return !(*this == Other);
	}

	//-------------------------------------------------------------------
	// 주어진 로컬 공간 크기, 레이아웃 변환, 그리고 동일한 렌더 변환(identity render transform)을 사용하여
	// 이 Geometry에 상대적인 자식 Geometry를 생성합니다.
	// 예를 들어, 5x5 여백을 가진 위젯은 자식 내용에 대해 Translate(5,5)의 LayoutTransform과
	// 자신의 크기보다 10 단위 작은 LocalSize를 가지는 Geometry를 생성합니다.
	//
	// @param LocalSize         로컬 공간에서 자식 Geometry의 크기.
	// @param LayoutTransform   이 Geometry에 상대적인 새로운 자식의 레이아웃 변환.
	//                          (자식의 레이아웃 공간에서 해당 위젯의 레이아웃 공간으로의 변환)
	//
	// @return                  새로운 자식 Geometry.
	inline static FGeometry MakeRoot(const FVector2D& InLocalSize, const FSlateLayoutTransform& InLayoutTransform);

    inline static FGeometry MakeRoot(const FVector2D& InLocalSize, const FSlateLayoutTransform& InLayoutTransform, const FSlateRenderTransform& InRenderTransform)
	{
		return FGeometry(InLocalSize, InLayoutTransform, FSlateLayoutTransform(), FSlateRenderTransform(),
		                 !InRenderTransform.IsIdentity());
	}

	// 자식 위젯의 렌더 변환을 고려하여 자식 Geometry를 생성합니다.
	inline FGeometry MakeChild(const FVector2D& InLocalSize, const FSlateLayoutTransform& LayoutTransform, const FSlateRenderTransform& RenderTransform, const FVector2D& RenderTransformPivot) const
	{
		return FGeometry(InLocalSize, LayoutTransform, RenderTransform, RenderTransformPivot,
		                 GetAccumulatedLayoutTransform(), GetAccumulatedRenderTransform());
	}

	/**
	 * Create a child geometry relative to this one with a given local space size, layout transform, and identity render transform.
	 * For example, a widget with a 5x5 margin will create a geometry for it's child contents having a LayoutTransform of Translate(5,5) and a LocalSize 10 units smaller 
	 * than it's own.
	 *
	 * @param LocalSize			The size of the child geometry in local space.
	 * @param LayoutTransform	Layout transform of the new child relative to this Geometry. Goes from the child's layout space to the this widget's layout space.
	 *
	 * @return					The new child geometry.
	 */
	inline FGeometry MakeChild(const FVector2D& InLocalSize, const FSlateLayoutTransform& LayoutTransform) const
    {
    	return FGeometry(InLocalSize, LayoutTransform, GetAccumulatedLayoutTransform(), GetAccumulatedRenderTransform(), bHasRenderTransform);
    }
	inline FGeometry MakeChild(const FSlateRenderTransform& RenderTransform, const FVector2D& RenderTransformPivot = FVector2f(0.5f, 0.5f)) const
    {
    	return FGeometry(GetLocalSize(), FSlateLayoutTransform(), RenderTransform, RenderTransformPivot, GetAccumulatedLayoutTransform(), GetAccumulatedRenderTransform());
    }
	/**
	 * 주어진 로컬 공간 크기와 레이아웃 변환을 사용하여
	 * 이 Geometry에 상대적인 자식 Geometry+위젯을 생성합니다.
	 * 이 Geometry는 자식 위젯의 렌더 변환을 상속받습니다.
	 * 예를 들어, 5x5 여백을 가진 위젯은 자식 내용에 대해 Translate(5,5)의 LayoutTransform과
	 * 자신의 크기보다 10 단위 작은 LocalSize를 가지는 Geometry를 생성합니다.
	 *
	 * @param ChildWidget       이 Geometry가 생성되는 자식 위젯.
	 * @param LayoutGeometry
	 * @param LocalSize         로컬 공간에서 자식 Geometry의 크기.
	 * @param LayoutTransform   이 Geometry에 상대적인 새로운 자식의 레이아웃 변환.
	 *
	 * @return                  새로운 자식 Geometry+위젯.
	 */
	FArrangedWidget MakeChild(const std::shared_ptr<SWidget>& ChildWidget, const FLayoutGeometry& LayoutGeometry) const;

	/**
	 * 주어진 로컬 공간 크기와 레이아웃 변환을 사용하여
	 * 이 Geometry에 상대적인 자식 Geometry+위젯을 생성합니다.
	 * 이 Geometry는 자식 위젯의 렌더 변환을 상속받습니다.
	 * 예를 들어, 5x5 여백을 가진 위젯은 자식 내용에 대해 Translate(5,5)의 LayoutTransform과
	 * 자신의 크기보다 10 단위 작은 LocalSize를 가지는 Geometry를 생성합니다.
	 *
	 * @param ChildWidget       이 Geometry가 생성되는 자식 위젯.
	 * @param LocalSize         로컬 공간에서 자식 Geometry의 크기.
	 * @param LayoutTransform   이 Geometry에 상대적인 새로운 자식의 레이아웃 변환.
	 *
	 * @return                  새로운 자식 Geometry+위젯.
	 */
	FArrangedWidget MakeChild(const std::shared_ptr<SWidget>& ChildWidget, const FVector2D& InLocalSize, const FSlateLayoutTransform& LayoutTransform) const;


	// FPaintGeometry를 생성하여 반환합니다.
	inline FPaintGeometry ToPaintGeometry() const
	{
		return FPaintGeometry(GetAccumulatedLayoutTransform(), GetAccumulatedRenderTransform(), LocalSize, bHasRenderTransform);
	}

	inline FPaintGeometry ToPaintGeometry(const FVector2D& InLocalSize, const FSlateLayoutTransform& InLayoutTransform) const
	{
		FSlateLayoutTransform NewAccumulatedLayoutTransform = InLayoutTransform.Concatenate(GetAccumulatedLayoutTransform());
		return FPaintGeometry(NewAccumulatedLayoutTransform, InLayoutTransform.Concatenate(GetAccumulatedRenderTransform()), InLocalSize, bHasRenderTransform);
	}

	inline FPaintGeometry ToPaintGeometry(const FVector2D& InLocalSize, const FSlateLayoutTransform& InLayoutTransform, const FSlateRenderTransform& RenderTransform, const FVector2D& RenderTransformPivot = FVector2D(0.5f, 0.5f)) const
	{
		return MakeChild(InLocalSize, InLayoutTransform, RenderTransform, RenderTransformPivot).ToPaintGeometry();
	}

	inline FPaintGeometry ToPaintGeometry(const FSlateLayoutTransform& LayoutTransform) const
	{
		return ToPaintGeometry(LocalSize, LayoutTransform);
	}

	inline FPaintGeometry ToPaintGeometry(const FVector2D& InLocalOffset, const FVector2D& InLocalSize, float InLocalScale = 1.0f) const
	{
		return ToPaintGeometry(InLocalSize, FSlateLayoutTransform(InLocalScale, FVector2D(InLocalScale * InLocalOffset.X, InLocalScale * InLocalOffset.Y)));
	}


	inline FPaintGeometry ToOffsetPaintGeometry(const FVector2D& LocalOffset) const
	{
		return ToPaintGeometry(FSlateLayoutTransform(LocalOffset));
	}

	/**
	 * 지정된 만큼 각 방향으로 로컬 공간이 "확장(inflate)"된 이 Geometry에 상대적인
	 * 페인트 Geometry를 생성합니다.
	 * 페인트 Geometry는 위젯의 렌더 변환을 상속받습니다.
	 *
	 * @param InflateAmount   중심점을 기준으로 각 방향으로 Geometry를 "확장"할 양.
	 *                        이는 레이아웃 변환 오프셋과 LocalSize의 확장을 효과적으로 정의합니다.
	 *
	 * @return                이 Geometry로부터 파생된 새로운 페인트 Geometry.
	 */
	inline FPaintGeometry ToInflatedPaintGeometry(const FVector2D& InflateAmount) const
	{
		// 이 방식은 위젯 주변에 (또는 위젯 내부로) 여백(border)을 추가하는 효과와 같습니다.
		// 크기를 확장한 후, 여백만큼 오프셋을 적용합니다.
		// 단, 이는 자식 위젯의 스케일에는 영향을 주지 않습니다.
		FVector2D NewSize = LocalSize + InflateAmount * 2;
		return ToPaintGeometry(NewSize, FSlateLayoutTransform(FVector2D(-InflateAmount.X, -InflateAmount.Y) ));
	}

	/**
	 * 절대 좌표는 위젯 계층의 루트가 위치한 공간에 따라 데스크탑 공간이나 윈도우 공간일 수 있습니다.
	 *
	 * @return 절대 좌표의 위치가 이 Geometry의 경계 내에 있으면 true를 반환합니다.
	 */
	inline bool IsUnderLocation(const FVector2D& AbsoluteCoordinate) const
	{
		// 이 렌더 변환의 역변환은 다소 비용이 많이 듭니다. 캐싱하는 방안을 고려할 수 있습니다.
		FSlateRotatedRect Rect = FSlateRotatedRect::TransformRect(GetAccumulatedRenderTransform(), FSlateRotatedRect(FSlateRect(FVector2D(0.0f, 0.0f), LocalSize)));
		return Rect.IsUnderLocation(AbsoluteCoordinate);
	}

	/**
	 * 절대 좌표는 위젯 계층의 루트가 위치한 공간에 따라 데스크탑 공간이나 윈도우 공간일 수 있습니다.
	 *
	 * @return 절대 좌표를 이 Geometry의 로컬 공간으로 변환합니다.
	 */
	inline FVector2D AbsoluteToLocal(const FVector2D& AbsoluteCoordinate) const
	{
		// 이 렌더 변환의 역변환은 다소 비용이 많이 듭니다. 캐싱하는 방안을 고려할 수 있습니다.
		return GetAccumulatedRenderTransform().Inverse().TransformPoint(AbsoluteCoordinate);
	}

	/**
	 * 로컬 좌표를 절대 좌표로 변환합니다.
	 *
	 * 절대 좌표는 위젯 계층의 루트가 위치한 공간에 따라 데스크탑 공간이나 윈도우 공간일 수 있습니다.
	 *
	 * @return 절대 좌표
	 */
	inline FVector2D LocalToAbsolute(const FVector2D& LocalCoordinate) const
	{
		return GetAccumulatedRenderTransform().TransformPoint(LocalCoordinate);
	}

	/**
	 * 로컬 좌표를 절대 공간으로 변환한 후, 반올림되어 정수 값 또는 근사값으로 나타내는
	 * 로컬 좌표로 변환합니다.
	 * 이는 팝업이나 툴팁을 표시할 때, 창이 반 픽셀 위치에서 시작하지 않아 내용들이
	 * 서로 약간씩 어긋나는(jitter) 현상을 방지하는 데 중요합니다.
	 */
	inline FVector2D LocalToRoundedLocal(const FVector2D& LocalCoordinate) const
	{
		const FVector2D AbsoluteCoordinate = LocalToAbsolute(LocalCoordinate);
		const FVector2D AbsoluteCoordinateRounded = FVector2D(std::floor(AbsoluteCoordinate.X + 0.5f), std::floor(AbsoluteCoordinate.Y + 0.5f));

		return AbsoluteToLocal(AbsoluteCoordinateRounded);
	}

	inline FSlateRect GetLayoutBoundingRect() const
	{
		return GetLayoutBoundingRect(FSlateRect(FVector2D(0.0f, 0.0f), LocalSize));
	}

	inline FSlateRect GetLayoutBoundingRect(const FMargin& LocalSpaceExtendBy) const
	{
		return GetLayoutBoundingRect(FSlateRect(FVector2D::ZeroVector, LocalSize).ExtendBy(LocalSpaceExtendBy));
	}

	inline FSlateRect GetLayoutBoundingRect(const FSlateRect& LocalSpaceRect) const
	{
		FTransform2 AccumulatedLayoutTransform2 = FTransform2(GetAccumulatedLayoutTransform().GetScale(), GetAccumulatedLayoutTransform().GetTranslation());
		return FSlateRotatedRect::TransformRect(AccumulatedLayoutTransform2, FSlateRotatedRect(LocalSpaceRect)).ToBoundingRect();
	}

	inline FSlateRect GetRenderBoundingRect() const
	{
		return GetRenderBoundingRect(FSlateRect(FVector2D(0.0f, 0.0f), LocalSize));
	}

	inline FSlateRect GetRenderBoundingRect(const FMargin& LocalSpaceExtendBy) const
	{
		return GetRenderBoundingRect(FSlateRect(FVector2D::ZeroVector, LocalSize).ExtendBy(LocalSpaceExtendBy));
	}

	inline FSlateRect GetRenderBoundingRect(const FSlateRect& LocalSpaceRect) const
	{
		return FSlateRotatedRect::TransformRect(GetAccumulatedRenderTransform(), FSlateRotatedRect(LocalSpaceRect)).ToBoundingRect();
	}

public:
    FVector2D LocalSize;
    float AbsoluteScale;
	FVector2D AbsolutePosition;
	FVector2D LocalPosition;

	// 로컬 크기에 누적 레이아웃 변환을 적용한 결과를 반환합니다.
	inline FVector2D GetDrawSize() const
	{
		return FVector2D(GetAccumulatedLayoutTransform().TransformVector(LocalSize));
	}

	inline FVector2D GetLocalSize() const
	{
		return LocalSize;
	}

	// 누적 레이아웃 변환을 생성합니다.
	inline FSlateLayoutTransform GetAccumulatedLayoutTransform() const
	{
		return FSlateLayoutTransform(AbsoluteScale, AbsolutePosition);
	}

	inline FSlateRenderTransform GetAccumulatedRenderTransform() const
	{
		return AccumulatedRenderTransform;
	}

	// 추가적인 레이아웃 변환을 누적 변환에 결합합니다.
	inline void AppendTransform(const FSlateLayoutTransform& LayoutTransform)
	{
		FSlateLayoutTransform AccumulatedLayoutTransform = GetAccumulatedLayoutTransform().Concatenate(LayoutTransform);
		AccumulatedRenderTransform = AccumulatedRenderTransform.Concatenate(LayoutTransform);
		AbsolutePosition = FVector2D(AccumulatedLayoutTransform.GetTranslation());
		AbsoluteScale = AccumulatedLayoutTransform.GetScale();
	}

	inline FVector2D GetAbsolutePosition() const
	{
		return AbsolutePosition;
	}

	// 로컬 크기에 누적 레이아웃 변환을 적용한 절대 크기를 반환합니다.
	inline FVector2D GetAbsoluteSize() const
	{
		return GetAccumulatedLayoutTransform().TransformVector(LocalSize);
	}

	// 정규 좌표(0~1)를 로컬 크기에 곱하여 절대 좌표로 변환합니다.
	inline FVector2D GetAbsolutePositionAtCoordinates(const FVector2D& NormalCoordinates) const
	{
		return GetAccumulatedRenderTransform().TransformPoint(FVector2D(NormalCoordinates.X * LocalSize.X, NormalCoordinates.Y * LocalSize.Y));
	}

	// 정규 좌표(0~1)를 로컬 크기에 곱하여 로컬 좌표를 계산합니다.
	inline FVector2D GetLocalPositionAtCoordinates(const FVector2D& NormalCoordinates) const
	{
		return LocalPosition + FVector2D(NormalCoordinates.X * LocalSize.X, NormalCoordinates.Y * LocalSize.Y);
	}

	inline bool HasRenderTransform() const
	{
		return bHasRenderTransform;
	}

	inline float GetAbsoluteScale() const
	{
		return AbsoluteScale;
	}

	inline FVector2D GetLocalPosition() const
	{
		return LocalPosition;
	}
private:
	FSlateRenderTransform AccumulatedRenderTransform;
	bool bHasRenderTransform;
};
