#include "Geometry.h"
#include "memory"
#include "Core/Math/MathUtility.h"
#include "Core/Rendering/Widget/ArrangedWidget.h"
#include "Core/Rendering/Widget/SWidget.h"
#include "Core/Rendering/Widget/Margin.h"

FGeometry::FGeometry() 
	: LocalSize(0.0f, 0.0f)
	, AbsoluteScale(1.0f)
	, AbsolutePosition(0.0f, 0.0f)
	, bHasRenderTransform(false)
{
}
 
FGeometry::~FGeometry()
{
}

FGeometry& FGeometry::operator=(const FGeometry& Other)
{
	return *this;
}

FGeometry FGeometry::MakeRoot(const FVector2D& InLocalSize, const FSlateLayoutTransform& InLayoutTransform)
{
	return FGeometry(InLocalSize, InLayoutTransform, FSlateLayoutTransform(), FSlateRenderTransform(), false);
}

FArrangedWidget FGeometry::MakeChild(const std::shared_ptr<SWidget>& ChildWidget, const FLayoutGeometry& LayoutGeometry) const
{
	return MakeChild(ChildWidget, LayoutGeometry.GetSizeInLocalSpace(), LayoutGeometry.GetLocalToParentTransform());
}

FArrangedWidget FGeometry::MakeChild(const std::shared_ptr<SWidget>& ChildWidget, const FVector2D& InLocalSize, const FSlateLayoutTransform& LayoutTransform) const
{
	FSlateRenderTransform RenderTransform = ChildWidget->GetRenderTransformWithRespectToFlowDirection().value();
	if (RenderTransform != FSlateRenderTransform())
	{
		const FVector2D RenderTransformPivot = FVector2D(ChildWidget->GetRenderTransformPivotWithRespectToFlowDirection());
		return FArrangedWidget(ChildWidget, MakeChild(InLocalSize, LayoutTransform, RenderTransform, RenderTransformPivot));
	}
	else
	{
		return FArrangedWidget(ChildWidget, MakeChild(InLocalSize, LayoutTransform));
	}
}

FSlateRect FSlateRotatedRect::ToBoundingRect() const
{
	FVector2D Points[4] =
	{
		TopLeft,
		TopLeft + ExtentX,
		TopLeft + ExtentY,
		TopLeft + ExtentX + ExtentY
	};
	return FSlateRect(
		FMath::Min(Points[0].X, FMath::Min(FMath::Min(Points[1].X, Points[2].X), Points[3].X)),
		FMath::Min(Points[0].Y, FMath::Min(FMath::Min(Points[1].Y, Points[2].Y), Points[3].Y)),
		FMath::Max(Points[0].X, FMath::Max(FMath::Max(Points[1].X, Points[2].X), Points[3].X)),
		FMath::Max(Points[0].Y, FMath::Max(FMath::Max(Points[1].Y, Points[2].Y), Points[3].Y))
	);
}

bool FSlateRotatedRect::IsUnderLocation(const FVector2D& Location) const
{
	const FVector2D Offset = Location - TopLeft;
	const float Det = ExtentX.X * ExtentY.Y - ExtentX.Y * ExtentY.X;

	// Not exhaustively efficient. Could optimize the checks for [0..1] to short circuit faster.
	const float S = (Offset.X * ExtentY.Y - Offset.Y * ExtentY.X) / Det;
	if (((S >= 0.0f) && (S <= 1.f)))
	{
		const float T = (ExtentX.X * Offset.Y - ExtentX.Y * Offset.X) / Det;
		return ((T >= 0.0f) && (T <= 1.f));
	}
	return false;
}

FSlateRotatedRect FSlateRotatedRect::MakeRotatedRect(const FSlateRect& ClipRectInLayoutWindowSpace, const FTransform2& LayoutToRenderTransform)
{
	const FSlateRotatedRect RotatedRect = FSlateRotatedRect::TransformRect(LayoutToRenderTransform, FSlateRotatedRect(ClipRectInLayoutWindowSpace));

	const FVector2D TopRight = RotatedRect.TopLeft + RotatedRect.ExtentX;
	const FVector2D BottomLeft = RotatedRect.TopLeft + RotatedRect.ExtentY;

	return FSlateRotatedRect(
		RotatedRect.TopLeft,
		TopRight - RotatedRect.TopLeft,
		BottomLeft - RotatedRect.TopLeft);
}

FSlateRotatedRect FSlateRotatedRect::MakeSnappedRotatedRect(const FSlateRect& ClipRectInLayoutWindowSpace, const FTransform2& LayoutToRenderTransform)
{
	const FSlateRotatedRect RotatedRect = TransformRect(LayoutToRenderTransform, FSlateRotatedRect(ClipRectInLayoutWindowSpace));

	// Pixel snapping is done here by rounding the resulting floats to ints, we do this before
	// calculating the final extents of the clip box otherwise we'll get a smaller clip rect than a visual
	// rect where each point is individually snapped.
	const FVector2D SnappedTopLeft = ((RotatedRect.TopLeft)).RoundToVector();
	const FVector2D SnappedTopRight = ((RotatedRect.TopLeft) + (RotatedRect.ExtentX)).RoundToVector();
	const FVector2D SnappedBottomLeft = ((RotatedRect.TopLeft) + (RotatedRect.ExtentY)).RoundToVector();

	//NOTE: We explicitly do not re-snap the extent x/y, it wouldn't be correct to snap again in distance space
	// even if two points are snapped, their distance wont necessarily be a whole number if those points are not
	// axis aligned.
	return FSlateRotatedRect(
		SnappedTopLeft,
		SnappedTopRight - SnappedTopLeft,
		SnappedBottomLeft - SnappedTopLeft);
}

FSlateRect FSlateRect::InsetBy(const FMargin& InsetAmount) const
{
	return FSlateRect(Left + InsetAmount.Left, Top + InsetAmount.Top, Right - InsetAmount.Right, Bottom - InsetAmount.Bottom);
}

FSlateRect FSlateRect::ExtendBy(const FMargin& ExtendAmount) const
{
	return FSlateRect(Left - ExtendAmount.Left, Top - ExtendAmount.Top, Right + ExtendAmount.Right, Bottom + ExtendAmount.Bottom);
}

