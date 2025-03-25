#pragma once
#include <Core/Math/Transform.h>

typedef FTransform2 FSlateRenderTransform;

class FArrangedWidget;
class SWidget;
struct FMargin;

struct FSlateRect
{
public:
	float Left;
	float Top;
	float Right;
	float Bottom;


	explicit FSlateRect(float InLeft = -1, float InTop = -1, float InRight = -1, float InBottom = -1)
		: Left(InLeft)
		, Top(InTop)
		, Right(InRight)
		, Bottom(InBottom)
	{
	}

	FSlateRect(const FVector2D& InStartPos, const FVector2D& InEndPos)
		: Left(InStartPos.X)
		, Top(InStartPos.Y)
		, Right(InEndPos.X)
		, Bottom(InEndPos.Y)
	{
	}

	static inline FSlateRect TransformRect(const FSlateLayoutTransform& Transform, const FSlateRect& Rect)
	{
		FVector2D TopLeftTransformed = Transform.TransformPoint(FVector2D(Rect.Left, Rect.Top));
		FVector2D BottomRightTransformed = Transform.TransformPoint(FVector2D(Rect.Right, Rect.Bottom));

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

	FSlateRect InsetBy(const struct FMargin& InsetAmount) const;

	FSlateRect ExtendBy(const FMargin& ExtendAmount) const;

	inline FVector2D GetTopLeft() const
	{
		return FVector2D(Left, Top);
	}

	inline FVector2D GetSize() const
	{
		return FVector2D(Right - Left, Bottom - Top);
	}
};

struct FSlateRotatedRect
{
public:
	/** Default ctor. */
	FSlateRotatedRect() {}

	/** Construct a rotated rect from a given aligned rect. */
	explicit FSlateRotatedRect(const FSlateRect& AlignedRect)
		: TopLeft(AlignedRect.Left, AlignedRect.Top)
		, ExtentX(AlignedRect.Right - AlignedRect.Left, 0.0f)
		, ExtentY(0.0f, AlignedRect.Bottom - AlignedRect.Top)
	{
	}

	/** Per-element constructor. */
	FSlateRotatedRect(const FVector2D& InTopLeft, const FVector2D& InExtentX, const FVector2D& InExtentY)
		: TopLeft(InTopLeft)
		, ExtentX(InExtentX)
		, ExtentY(InExtentY)
	{
	}

public:

	/** transformed Top-left corner. */
	FVector2D TopLeft;
	/** transformed X extent (right-left). */
	FVector2D ExtentX;
	/** transformed Y extent (bottom-top). */
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

	/** Convert to a bounding, aligned rect. */
	FSlateRect ToBoundingRect() const;

	/** Point-in-rect test. */
	bool IsUnderLocation(const FVector2D& Location) const;

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
	* Used to construct a rotated rect from an aligned clip rect and a set of layout and render transforms from the geometry, snapped to pixel boundaries. Returns a float or float16 version of the rect based on the typedef.
	*/
	static  FSlateRotatedRect MakeSnappedRotatedRect(const FSlateRect& ClipRectInLayoutWindowSpace, const FTransform2& LayoutToRenderTransform);

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


class FLayoutGeometry
{
public:
	FLayoutGeometry()
		: LocalSize(1.0f, 1.0f)
	{
	}

	explicit FLayoutGeometry(const FSlateLayoutTransform& InLocalToParent, const FVector2D& SizeInLocalSpace)
		: LocalToParent(InLocalToParent)
		, LocalSize(SizeInLocalSpace)
	{
	}

	const FSlateLayoutTransform& GetLocalToParentTransform() const
	{
		return LocalToParent;
	}

	FVector2D GetSizeInLocalSpace() const
	{
		return LocalSize;
	}

	FVector2D GetSizeInParentSpace() const
	{
		return LocalToParent.TransformVector(LocalSize);
	}

	FVector2D GetOffsetInParentSpace() const
	{
		return LocalToParent.GetTranslation();
	}

	FSlateRect GetRectInLocalSpace() const
	{
		return FSlateRect(FVector2D(0.0f, 0.0f), LocalSize);
	}

	FSlateRect GetRectInParentSpace() const
	{
		return FSlateRect::TransformRect(LocalToParent, GetRectInLocalSpace());
	}

private:
	FSlateLayoutTransform LocalToParent;
	FVector2D LocalSize;
};

/**
 * A Paint geometry contains the window-space (draw-space) info to draw an element on the screen.
 *
 * It contains the size of the element in the element's local space along with the
 * transform needed to position the element in window space.
 *
 * DrawPosition/DrawSize/DrawScale are maintained for legacy reasons, but are deprecated:
 *
 *		The DrawPosition and DrawSize are already positioned and
 *		scaled for immediate consumption by the draw code.
 *
 *		The DrawScale is only applied to the internal aspects of the draw primitives. e.g. Line thickness, 3x3 grid margins, etc.
 */
struct FPaintGeometry
{
	/**
	 * !!! DEPRECATED!!! Drawing should only happen in local space to ensure render transforms work.
	 *	WARNING: This legacy member represents is LAYOUT space, and does not account for render transforms.
	 *
	 * Render-space position at which we will draw.
	 *
	 *
	 */
	FVector2D DrawPosition;

	/**
	 * !!! DEPRECATED!!! Drawing should only happen in local space to ensure render transforms work.
	 *	WARNING: This legacy member represents is LAYOUT space, and does not account for render transforms.
	 *
	 * Only affects the draw aspects of individual draw elements. e.g. line thickness, 3x3 grid margins
	 */
	float DrawScale;

	/** Get the Size of the geometry in local space. Must call CommitTransformsIfUsingLegacyConstructor() first if legacy ctor is used. */
	FVector2D GetLocalSize() const { return LocalSize; }

	/** Access the final render transform. Must call CommitTransformsIfUsingLegacyConstructor() first if legacy ctor is used. */
	const FSlateRenderTransform& GetAccumulatedRenderTransform() const { return AccumulatedRenderTransform; }

	/**
	 * Support mutable geometries constructed in window space, and possibly mutated later, as all legacy members are public.
	 * In these cases we defer creating of the RenderTransform and LocalSize until rendering time to ensure that all member changes have finished.
	 * WARNING: Legacy usage does NOT support render transforms!
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
	// Mutable to support legacy constructors. Doesn't account for render transforms.
	mutable FVector2D DrawSize;

	// Mutable to support legacy constructors.
	mutable FVector2D LocalSize;

	// final render transform for drawing. Transforms from local space to window space for the draw element.
	// Mutable to support legacy constructors.
	mutable FSlateRenderTransform AccumulatedRenderTransform;

	// Support legacy constructors.
	uint8 bUsingLegacyConstructor : 1;

	uint8 bHasRenderTransform : 1;

public:
	/** Default ctor. */
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
	 * Creates and initializes a new instance.
	 *
	 * @param InLocalSize					The size in local space
	 * @param InAccumulatedLayoutTransform	The accumulated layout transform (from an FGeometry)
	 * @param InAccumulatedRenderTransform	The accumulated render transform (from an FGeometry)
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

	// !!! DEPRECATED!!! This is legacy and should be removed!
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
	 * Special case method to append a layout transform to a paint geometry.
	 * This is used in cases where the FGeometry was arranged in desktop space
	 * and we need to undo the root desktop translation to get into window space.
	 * If you find yourself wanting to use this function, ask someone if there's a better way.
	 *
	 * @param LayoutTransform	An additional layout transform to append to this paint geometry.
	 */
	void AppendTransform(const FSlateLayoutTransform& LayoutTransform)
	{
		FTransform2 LayoutTransform2x2 = FTransform2(LayoutTransform.GetScale(), LayoutTransform.GetTranslation());
		AccumulatedRenderTransform = AccumulatedRenderTransform.Concatenate(LayoutTransform2x2);
		DrawPosition = LayoutTransform.TransformPoint(DrawPosition);
		DrawScale = LayoutTransform.GetScale() * DrawScale;
	}

	/**
	 * Special case method to replace the render transform on a paint geometry.
	 *
	 * @param RenderTransform	An additional layout transform to append to this paint geometry.
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

	inline static FGeometry MakeRoot(const FVector2D& InLocalSize, const FSlateLayoutTransform& InLayoutTransform)
	{
		return FGeometry(InLocalSize, InLayoutTransform, FSlateLayoutTransform(), FSlateRenderTransform(), false);
	}

	inline static FGeometry MakeRoot(const FVector2D& InLocalSize, const FSlateLayoutTransform& InLayoutTransform, const FSlateRenderTransform& InRenderTransform)
	{
		return FGeometry(InLocalSize, InLayoutTransform, FSlateLayoutTransform(), FSlateRenderTransform(), !InRenderTransform.IsIdentity());
	}

	inline FGeometry MakeChild(const FVector2D& InLocalSize, const FSlateLayoutTransform& LayoutTransform, const FSlateRenderTransform& RenderTransform, const FVector2D& RenderTransformPivot) const
	{
		return FGeometry(InLocalSize, LayoutTransform, RenderTransform, RenderTransformPivot, GetAccumulatedLayoutTransform(), GetAccumulatedRenderTransform());
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
	inline FGeometry MakeChild(const FSlateRenderTransform& RenderTransform, const FVector2D& RenderTransformPivot = FVector2D(0.5f, 0.5f)) const
	{
		return FGeometry(GetLocalSize(), FSlateLayoutTransform(), RenderTransform, RenderTransformPivot, GetAccumulatedLayoutTransform(), GetAccumulatedRenderTransform());
	}

	FArrangedWidget MakeChild(const std::shared_ptr<SWidget>& ChildWidget, const FLayoutGeometry& LayoutGeometry) const;

	/**
	 * Create a child geometry+widget relative to this one with a given local space size and layout transform.
	 * The Geometry inherits the child widget's render transform.
	 * For example, a widget with a 5x5 margin will create a geometry for it's child contents having a LayoutTransform of Translate(5,5) and a LocalSize 10 units smaller
	 * than it's own.
	 *
	 * @param ChildWidget		The child widget this geometry is being created for.
	 * @param LocalSize			The size of the child geometry in local space.
	 * @param LayoutTransform	Layout transform of the new child relative to this Geometry.
	 *
	 * @return					The new child geometry+widget.
	 */
	FArrangedWidget MakeChild(const std::shared_ptr<SWidget>& ChildWidget, const FVector2D& InLocalSize, const FSlateLayoutTransform& LayoutTransform) const;


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
	 * Create a paint geometry relative to this one that whose local space is "inflated" by the specified amount in each direction.
	 * The paint geometry inherits the widget's render transform.
	 *
	 * @param InflateAmount	Amount by which to "inflate" the geometry in each direction around the center point. Effectively defines a layout transform offset and an inflation of the LocalSize.
	 *
	 * @return				The new paint geometry derived from this one.
	 */
	inline FPaintGeometry ToInflatedPaintGeometry(const FVector2D& InflateAmount) const
	{
		// This essentially adds (or subtracts) a border around the widget. We scale the size then offset by the border amount.
		// Note this is not scaling child widgets, so the scale is not changing.
		FVector2D NewSize = LocalSize + InflateAmount * 2;
		return ToPaintGeometry(NewSize, FSlateLayoutTransform(FVector2D(-InflateAmount.X, -InflateAmount.Y) ));
	}

	/**
	 * Absolute coordinates could be either desktop or window space depending on what space the root of the widget hierarchy is in.
	 *
	 * @return true if the provided location in absolute coordinates is within the bounds of this geometry.
	 */
	inline bool IsUnderLocation(const FVector2D& AbsoluteCoordinate) const
	{
		// this render transform invert is a little expensive. We might consider caching it?
		FSlateRotatedRect Rect = FSlateRotatedRect::TransformRect(GetAccumulatedRenderTransform(), FSlateRotatedRect(FSlateRect(FVector2D(0.0f, 0.0f), LocalSize)));
		return Rect.IsUnderLocation(AbsoluteCoordinate);
	}

	/**
	 * Absolute coordinates could be either desktop or window space depending on what space the root of the widget hierarchy is in.
	 *
	 * @return Transforms AbsoluteCoordinate into the local space of this Geometry.
	 */
	inline FVector2D AbsoluteToLocal(const FVector2D& AbsoluteCoordinate) const
	{
		// this render transform invert is a little expensive. We might consider caching it.
		return GetAccumulatedRenderTransform().Inverse().TransformPoint(AbsoluteCoordinate);
	}

	/**
	 * Translates local coordinates into absolute coordinates
	 *
	 * Absolute coordinates could be either desktop or window space depending on what space the root of the widget hierarchy is in.
	 *
	 * @return  Absolute coordinates
	 */
	inline FVector2D LocalToAbsolute(const FVector2D& LocalCoordinate) const
	{
		return GetAccumulatedRenderTransform().TransformPoint(LocalCoordinate);
	}

	/**
	 * Translates the local coordinates into local coordinates that after being transformed into absolute space will be rounded
	 * to a whole number or approximately a whole number.  This is important for cases where you want to show a popup or a tooltip
	 * and not have the window start on a half pixel, which can cause the contents to jitter in relation to each other as the tooltip
	 * or popup moves around.
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

	inline FVector2D GetDrawSize() const
	{
		return FVector2D(GetAccumulatedLayoutTransform().TransformVector(LocalSize));
	}

	inline FVector2D GetLocalSize() const
	{
		return LocalSize;
	}

	inline FSlateLayoutTransform GetAccumulatedLayoutTransform() const
	{
		return FSlateLayoutTransform(AbsoluteScale, AbsolutePosition);
	}

	inline FSlateRenderTransform GetAccumulatedRenderTransform() const
	{
		return AccumulatedRenderTransform;
	}

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

	inline FVector2D GetAbsoluteSize() const
	{
		return GetAccumulatedLayoutTransform().TransformVector(LocalSize);
	}

	inline FVector2D GetAbsolutePositionAtCoordinates(const FVector2D& NormalCoordinates) const
	{
		return GetAccumulatedRenderTransform().TransformPoint(FVector2D(NormalCoordinates.X * LocalSize.X, NormalCoordinates.Y * LocalSize.Y));
	}

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

