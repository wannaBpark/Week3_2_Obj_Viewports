#include "SWindow.h"
#include "Core/Rendering/Widget/Reply.h"

void SWindow::Tick(const FGeometry& AllottedGeometry, const float InDeltaTime)
{
	Super::Tick(AllottedGeometry, InDeltaTime);
}

SWindow::SWindow(const FVector2D& InSize)
{
	// 초기 창 Geometry 생성: 부모가 없으므로 MakeRoot를 사용하며, 아이덴티티 변환 적용
	MyGeometry = FGeometry::MakeRoot(InSize, FSlateLayoutTransform(1.0f, FVector2D(0, 0)));
	// 기본 루트 위젯을 생성하거나 외부에서 설정할 수 있도록 함 (여기서는 기본 SWidget 사용)
	RootWidget = shared_from_this();
}

void SWindow::UpdateWindowGeometry(const FSlateLayoutTransform& NewTransform)
{
	// 창의 현재 크기는 WindowGeometry에 저장되어 있는 LocalSize를 사용합니다.
	// 창의 위치, 스케일 등의 변경 사항을 반영하여 새 Geometry를 생성합니다.
	FVector2D CurrentSize = MyGeometry.GetLocalSize();
	MyGeometry = FGeometry::MakeRoot(CurrentSize, NewTransform);

	std::cout << "Window geometry updated: Scale = " << NewTransform.GetScale()
			  << ", Translation = (" << NewTransform.GetTranslation().X << ", " << NewTransform.GetTranslation().Y << ")\n";

}

void SWindow::ArrangeWindow() const
{
	// 자식 위젯들의 재배치를 위해, 루트 위젯의 Tick과 ArrangeChildren을 호출합니다.
	if (RootWidget)
	{
		// DeltaTime은 프레임 간 시간(예: 16ms)을 전달 (여기서는 고정값 사용)
		RootWidget->Tick(MyGeometry, 0.016f);

		// TArray<FArrangedWidget>은 배열 컨테이너입니다. (TArray는 Unreal Engine 스타일의 배열이라 가정)
		TArray<FArrangedWidget> ArrangedChildren;
		RootWidget->OnArrangeChildren(MyGeometry, ArrangedChildren);

		std::cout << "Window arranged " << ArrangedChildren.Num() << " child widget(s).\n";
	}
}

void SWindow::SetRootWidget(const std::shared_ptr<SWidget>& InRootWidget)
{
	if (InRootWidget != shared_from_this())
	{
		RootWidget = InRootWidget;
		RootWidget->AddChild(shared_from_this());
	}
	else
	{
		RootWidget = shared_from_this();
	}
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
	return FSlateLayoutTransform(1, MyGeometry.AbsolutePosition);
}

FSlateLayoutTransform SWindow::GetLocalToWindowTransform() const
{
	return FSlateLayoutTransform(1);
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
	return GetNonMaximizedWindowBorderSize();
}

FMargin SWindow::GetNonMaximizedWindowBorderSize() const
{
	return LayoutBorder;
}

void SWindow::MoveWindowTo(const FVector2D NewPosition)
{
	InitialDesiredScreenPosition = NewPosition;
}

void SWindow::Resize(FVector2D NewClientSize)
{
	ResizeWindowSize(GetWindowSizeFromClientSize(NewClientSize));
}

void SWindow::SetCachedScreenPosition(FVector2D NewPosition)
{
	ScreenPosition = NewPosition;
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

bool SWindow::IsScreenspaceMouseWithin(FVector2D ScreenspaceMouseCoordinate) const
{
	// 창의 절대 위치(화면 상의 좌측 상단 위치)를 가져옵니다.
	FVector2D WindowTopLeft = MyGeometry.GetAbsolutePosition();
	// 창의 크기를 가져옵니다.
	FVector2D WindowSize = MyGeometry.GetLocalSize();
    
	// 창의 경계 사각형의 오른쪽 아래 좌표 계산
	FVector2D WindowBottomRight = WindowTopLeft + WindowSize;
    
	// 주어진 스크린 좌표가 창 내부에 있는지 검사합니다.
	return (ScreenspaceMouseCoordinate.X >= WindowTopLeft.X &&
			ScreenspaceMouseCoordinate.X <= WindowBottomRight.X &&
			ScreenspaceMouseCoordinate.Y >= WindowTopLeft.Y &&
			ScreenspaceMouseCoordinate.Y <= WindowBottomRight.Y);
}

FCursorReply SWindow::OnCursorQuery(const FPointer& InPointer) const
{
	return FCursorReply::Cursor(EMouseCursor::Default);
}

FReply SWindow::OnFocusReceived()
{
	return FReply();
}

FReply SWindow::OnMouseButtonDown(EMouseButton InMouseButton, const FPointer& InPointer)
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

FReply SWindow::OnMouseButtonUp(EMouseButton InMouseButton, const FPointer& InPointer)
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

FReply SWindow::OnMouseMove(const FPointer& InPointer)
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
}