#include "SWindow.h"
#include "Core/Rendering/Widget/Reply.h"

void SWindow::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
}

FVector2D SWindow::GetDesiredSizeDesktopPixels() const
{
	return this->GetDesiredSize();
}

FVector2D SWindow::GetInitialDesiredSizeInScreen() const
{
	return InitialDesiredSize;
}

FVector2D SWindow::GetInitialDesiredPositionInScreen() const
{
	return InitialDesiredScreenPosition;
}

FGeometry SWindow::GetWindowGeometryInScreen() const
{
	// We are scaling children for layout, but our pixel bounds are not changing.
	// FGeometry expects Size in Local space, but our size is stored in screen space.
	// So we need to transform Size into the window's local space for FGeometry.
	FSlateLayoutTransform LocalToScreen = GetLocalToScreenTransform();
	return FGeometry::MakeRoot(LocalToScreen.Inverse().TransformVector(Size), LocalToScreen);
}

FGeometry SWindow::GetWindowGeometryInWindow() const
{
	FSlateLayoutTransform LocalToWindow = GetLocalToWindowTransform();
	FVector2D ViewSize = GetViewportSize();
	return FGeometry::MakeRoot(LocalToWindow.Inverse().TransformVector(ViewSize), LocalToWindow);
}

FSlateLayoutTransform SWindow::GetLocalToScreenTransform() const
{
	//TODO : ApplicationScale 받아서 ScreenPosition과 같이 FSlateLayoutTransform 생성자에 넣기
	//return FSlateLayoutTransform(FSlateApplicationBase::Get().GetApplicationScale() * GetDPIScaleFactor(), ScreenPosition);
	return FSlateLayoutTransform();
}

FSlateLayoutTransform SWindow::GetLocalToWindowTransform() const
{
	//TODO : ApplicationScale 받아서 FSlateLayoutTransform 생성자에 넣기
	//return FSlateLayoutTransform(FSlateApplicationBase::Get().GetApplicationScale() * GetDPIScaleFactor());
	return FSlateLayoutTransform();
}

FVector2D SWindow::GetPositionInScreen() const
{
	return ScreenPosition;
}

FVector2D SWindow::GetSizeInScreen() const
{
	return Size;
}

FSlateRect SWindow::GetNonMaximizedRectInScreen() const
{
	int32 X = 0;
	int32 Y = 0;
	int32 Width = 0;
	int32 Height = 0;

	//if (NativeWindow.IsValid() && NativeWindow->GetRestoredDimensions(X, Y, Width, Height))
	//{
	//	return FSlateRect((float)X, (float)Y, static_cast<float>(X + Width), static_cast<float>(Y + Height));
	//}
	//else
	{
		return GetRectInScreen();
	}
}

FSlateRect SWindow::GetRectInScreen() const
{
	if (bVirtualWindow)
	{
		return FSlateRect(0, 0, Size.X, Size.Y);
	}

	return FSlateRect(ScreenPosition, ScreenPosition + Size);
}

FSlateRect SWindow::GetClientRectInScreen() const
{
	if (bVirtualWindow)
	{
		return FSlateRect(0, 0, Size.X, Size.Y);
	}


	return GetRectInScreen()
		.InsetBy(GetWindowBorderSize())
		.InsetBy(FMargin(0.0f, TitleBarSize, 0.0f, 0.0f));
}

FVector2D SWindow::GetClientSizeInScreen() const
{
	return GetClientRectInScreen().GetSize();
}

FSlateRect SWindow::GetClippingRectangleInWindow() const
{
	FVector2D ViewSize = GetViewportSize();
	return FSlateRect(0, 0, ViewSize.X, ViewSize.Y);
}

FMargin SWindow::GetWindowBorderSize(bool bIncTitleBar) const
{

	// @TODO This is not working for Linux. The window is not yet valid when this gets
	// called from SWindow::Construct which is causing a default border to be retured even when the
	// window is borderless. This causes problems for menu positioning.
	//if (NativeWindow.IsValid() && NativeWindow->IsMaximized())
	//{
	//	const float DesktopPixelsToSlateUnits = 1.0f / (FSlateApplicationBase::Get().GetApplicationScale() * GetDPIScaleFactor());
	//	FMargin BorderSize(NativeWindow->GetWindowBorderSize() * DesktopPixelsToSlateUnits);
	//	if (bIncTitleBar)
	//	{
	//		// Add title bar size (whether it's visible or not)
	//		BorderSize.Top += NativeWindow->GetWindowTitleBarSize() * DesktopPixelsToSlateUnits;
	//	}

	//	return BorderSize;
	//}

	return GetNonMaximizedWindowBorderSize();
}

FMargin SWindow::GetNonMaximizedWindowBorderSize() const
{
	return LayoutBorder;
}

void SWindow::MoveWindowTo(FVector2D NewPosition)
{
	InitialDesiredScreenPosition = NewPosition;
}

void SWindow::ReshapeWindow(FVector2D NewPosition, FVector2D NewSize)
{
	return;
}

void SWindow::ReshapeWindow(const FSlateRect& InNewShape)
{
	return;
}

void SWindow::Resize(FVector2D NewClientSize)
{
	ResizeWindowSize(GetWindowSizeFromClientSize(NewClientSize));
}

void SWindow::SetCachedScreenPosition(FVector2D NewPosition)
{
	ScreenPosition = NewPosition;

	//TODO : Delagate
	//OnWindowMoved.ExecuteIfBound( SharedThis( this ) );
}

void SWindow::SetCachedSize(FVector2D NewSize)
{
	if (Size != NewSize)
	{
		Size = NewSize;
	}
}

void SWindow::SetContent(std::shared_ptr<SWidget> InContent)
{
	//TODO : ContentSlot
}

void SWindow::RequestDestroyWindow()
{
	//TODO : Delagate하고 Applcation 딴에서의 작업
	//if (RequestDestroyWindowOverride.IsBound())
	//{
	//	RequestDestroyWindowOverride.Execute(SharedThis(this));
	//}
	//else
	//{
	//	FSlateApplicationBase::Get().RequestDestroyWindow(SharedThis(this));
	//}
}

bool SWindow::AcceptsInput() const
{
	return true;
}

void SWindow::ResizeWindowSize(FVector2D NewWindowSize)
{
	NewWindowSize.X = FMath::Max(MinWidth.value(), NewWindowSize.X);
	NewWindowSize.X = FMath::Min(MaxWidth.value(), NewWindowSize.X);

	NewWindowSize.Y = FMath::Max(MinHeight.value(), NewWindowSize.Y);
	NewWindowSize.Y = FMath::Min(MaxHeight.value(), NewWindowSize.Y);

	// Ceil (Minus a tad for float precision) to ensure contents are not a sub-pixel larger than the window, which will create unnecessary scroll bars 
	FIntPoint CurrentIntSize = FIntPoint(FMath::CeilToInt32(Size.X - KINDA_SMALL_NUMBER), FMath::CeilToInt32(Size.Y - KINDA_SMALL_NUMBER));
	FIntPoint NewIntSize = FIntPoint(FMath::CeilToInt32(NewWindowSize.X - KINDA_SMALL_NUMBER), FMath::CeilToInt32(NewWindowSize.Y - KINDA_SMALL_NUMBER));

	if (CurrentIntSize != NewIntSize)
	{
		{
			InitialDesiredSize = FVector2D(NewIntSize.X, NewIntSize.Y);
		}
	}
	SetCachedSize(FVector2D(NewIntSize.X, NewIntSize.Y));
}

FVector2D SWindow::GetWindowSizeFromClientSize(FVector2D InClientSize)
{
	{
		const FMargin BorderSize = GetWindowBorderSize();

		InClientSize.X += BorderSize.Left + BorderSize.Right;
		InClientSize.Y += BorderSize.Bottom + BorderSize.Top;
	}

	return InClientSize;
}

const TArray<std::shared_ptr<SWindow>>& SWindow::GetChildWindows() const
{
	return ChildWindowPtrs;
}

TArray<std::shared_ptr<SWindow>>& SWindow::GetChildWindows()
{
	return ChildWindowPtrs;
}

void SWindow::AddChildWindow(const std::shared_ptr<SWindow>& ChildWindow)
{
	std::shared_ptr<SWindow> PreviousParent = ChildWindow->ParentWindowPtr.lock ();
	if (PreviousParent != nullptr)
	{
		// This child already had a parent, so we are actually re-parenting it
		const bool bRemovedSuccessfully = PreviousParent->RemoveDescendantWindow(ChildWindow);
	}

	ChildWindow->ParentWindowPtr = dynamic_pointer_cast<SWindow>(shared_from_this());

	//FSlateApplicationBase::Get().ArrangeWindowToFrontVirtual(ChildWindows, ChildWindow);
}

std::shared_ptr<SWindow> SWindow::GetParentWindow() const
{
	return ParentWindowPtr.lock();
}

std::shared_ptr<SWindow> SWindow::GetTopmostAncestor()
{
	std::shared_ptr<SWindow> TopmostParentSoFar = dynamic_pointer_cast<SWindow>(shared_from_this());;
	while (TopmostParentSoFar->ParentWindowPtr.lock() != nullptr)
	{
		TopmostParentSoFar = TopmostParentSoFar->ParentWindowPtr.lock();
	}

	return TopmostParentSoFar;
}

bool SWindow::RemoveDescendantWindow(const std::shared_ptr<SWindow>& DescendantToRemove)
{
	const bool bRemoved = 0 != ChildWindowPtrs.Remove(DescendantToRemove);

	for (int32 ChildIndex = 0; ChildIndex < ChildWindowPtrs.Num(); ++ChildIndex)
	{
		std::shared_ptr<SWindow>& ChildWindow = ChildWindowPtrs[ChildIndex];
		if (ChildWindow->RemoveDescendantWindow(DescendantToRemove))
		{
			return true;
		}
	}

	return false;
}

bool SWindow::IsActive() const
{
	return true;
}

bool SWindow::HasActiveChildren() const
{
	for (int32 i = 0; i < ChildWindowPtrs.Num(); ++i)
	{
		if (ChildWindowPtrs[i]->IsActive() || ChildWindowPtrs[i]->HasActiveChildren())
		{
			return true;
		}
	}

	return false;
}

bool SWindow::HasActiveParent() const
{
	std::shared_ptr<SWindow> ParentWindow = ParentWindowPtr.lock();
	if (ParentWindow != nullptr)
	{
		if (ParentWindow->IsActive())
		{
			return true;
		}

		return ParentWindow->HasActiveParent();
	}

	return false;
}

FCursorReply SWindow::OnCursorQuery(const FGeometry& MyGeometry, const FPointer& InPointer) const
{
	return FCursorReply::Cursor(EMouseCursor::Default);
}

FReply SWindow::OnFocusReceived(const FGeometry& MyGeometry)
{
	return FReply();
}

FReply SWindow::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointer& InPointer)
{
	//if (bDragAnywhere && MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	//{
	//	MoveResizeZone = WindowZone;
	//	return FReply::Handled().CaptureMouse(SharedThis(this));
	//}
	//else
	//{
	//	return FReply::Unhandled();
	//}
	return FReply();
}

FReply SWindow::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointer& InPointer)
{
	//if (bDragAnywhere && this->HasMouseCapture() && MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	//{
	//	MoveResizeZone = EWindowZone::Unspecified;
	//	return FReply::Handled().ReleaseMouseCapture();
	//}
	//else
	//{
	//	return FReply::Unhandled();
	//}
	return FReply();
}

FReply SWindow::OnMouseMove(const FGeometry& MyGeometry, const FPointer& InPointer)
{
	//if (bDragAnywhere && this->HasMouseCapture() && MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) && MoveResizeZone != EWindowZone::TitleBar)
	//{
	//	this->MoveWindowTo(ScreenPosition + MouseEvent.GetCursorDelta());
	//	return FReply::Handled();
	//}
	//else
	//{
	//	return FReply::Unhandled();
	//}
	return FReply();
}

int32 SWindow::OnPaint(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, int32 LayerId, bool bParentEnabled) const
{
	int32 MaxLayer = SWidget::OnPaint(AllottedGeometry, MyCullingRect, LayerId,  bParentEnabled);
	return MaxLayer;
}

FVector2D SWindow::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	return SWidget::ComputeDesiredSize(LayoutScaleMultiplier) * LayoutScaleMultiplier;
}

bool SWindow::ComputeVolatility() const
{
	return false;
}
