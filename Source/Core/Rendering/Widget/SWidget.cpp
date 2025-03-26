#include "SWidget.h"
#include "ArrangedWidget.h"
#include "Margin.h"
#include "Core/Rendering/Widget/Reply.h"

SWidget::SWidget()
	: LocalSize(100.0f, 100.0f)
	, LocalPosition(0.0f, 0.0f)
	, LayoutTransform(1.0f, LocalPosition)
	, bCanSupportFocus(true)
	, bCanHaveChildren(true)
	, bIsHoveredAttributeSet(false)
	, bDesiredSizeSet(false)
	, bCanChildrenBeAccessible(true)
	// Note we are defaulting to tick for backwards compatibility
	, VisibilityAttribute(EVisibility::Visible)
	, HoveredAttribute(false)
	,bCanTick(true)
	,EnabledAttribute(true)
{
}

SWidget::~SWidget()
{
}

int32 SWidget::Paint(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, int32 LayerId, bool bParentEnabled)
{
}

FReply SWidget::OnKeyDown(const EKeyCode& InKeyCode)
{
	if (bCanSupportFocus)
	{
		//TODO : 입력처리
		//EUINavigation Direction = FSlateApplicationBase::Get().GetNavigationDirectionFromKey(InKeyEvent);
		//// It's the left stick return a navigation request of the correct direction
		//if (Direction != EUINavigation::Invalid)
		//{
		//	const ENavigationGenesis Genesis = InKeyEvent.GetKey().IsGamepadKey() ? ENavigationGenesis::Controller : ENavigationGenesis::Keyboard;
		//	return FReply::Handled().SetNavigation(Direction, Genesis);
		//}
	}
	return FReply();
}

FReply SWidget::OnMouseButtonDown(EMouseButton InMouseButton, const FPointer& InPointer)
{
	FReply result = FReply();
	// 만약 이미 마우스 캡처 상태라면 추가 처리는 하지 않습니다.
	if (bIsMouseCaptured)
	{
		return result;
	}
	
	FVector2D localPos = MyGeometry.AbsoluteToLocal(InPointer.ScreenSpacePosition);

	if (localPos.X >= 0 && localPos.X <= LocalSize.X &&
		localPos.Y >= 0 && localPos.Y <= LocalSize.Y)
	{
		std::cout << "SWidget::OnMouseButtonDown - Button: " 
				  << static_cast<int>(InMouseButton) << ", Local MousePos: (" 
				  << localPos.X << ", " << localPos.Y << ")\n";
		
		result.CaptureMouse(shared_from_this()).SetUserFocus(shared_from_this());
		bIsMouseCaptured = true;
		APlayerInput::Get().SetMouseCaptor(result.GetMouseCaptor());
	}
	return result;
}

FReply SWidget::OnMouseButtonUp(EMouseButton InMouseButton, const FPointer& InPointer)
{
	FReply result = FReply();
	// 만약 이미 마우스 캡처 상태라면 추가 처리는 하지 않습니다.
	if (bIsMouseCaptured)
	{
		return result;
	}

	const FVector2D localPos = MyGeometry.AbsoluteToLocal(InPointer.ScreenSpacePosition);

	if (localPos.X >= 0 && localPos.X <= LocalSize.X &&
		localPos.Y >= 0 && localPos.Y <= LocalSize.Y)
	{
		std::cout << "SWidget::OnMouseButtonUp - Button: " 
				  << static_cast<int>(InMouseButton) << ", Local MousePos: (" 
				  << localPos.X << ", " << localPos.Y << ")\n";
		result.ReleaseMouseCapture();
		bIsMouseCaptured = true;
		APlayerInput::Get().ClearMouseCaptor();
	}
	return result;
}

FReply SWidget::OnMouseMove(const FPointer& InPointer)
{
	FReply result = FReply();
	// 만약 마우스 캡처 상태라면 (즉, 이 위젯이 캡처를 가지고 있다면) 드래그 등 캡처 관련 처리를 수행합니다.
	if (bIsMouseCaptured)
	{
		FVector2D localPos = MyGeometry.AbsoluteToLocal(InPointer.ScreenSpacePosition);
		// 캡처 상태에서는 추가 드래그 로직 등을 수행할 수 있습니다.
		return result;
	}
	else
	{
		// 캡처되지 않은 상태에서는 이전 프레임의 위치와 현재 위치를 비교하여
		// 위젯 내부로 처음 진입(OnMouseEnter)하거나 벗어날 때(OnMouseLeave)를 감지합니다.
		const bool bWasInside = IsInsideWidget(InPointer.LastScreenSpacePosition, MyGeometry);
		const bool bIsInside  = IsInsideWidget(InPointer.ScreenSpacePosition, MyGeometry);

		if (!bWasInside && bIsInside)
		{
			// 이전에는 영역 밖이었으나, 현재 영역 내부로 진입한 경우
			OnMouseEnter(InPointer);
		}
		else if (bWasInside && !bIsInside)
		{
			// 이전에는 영역 내부였으나, 현재 영역 밖으로 벗어난 경우
			OnMouseLeave(InPointer);
		}
		return result;
	}
}

void SWidget::OnMouseEnter(const FPointer& InPointer)
{
	if (!bIsHoveredAttributeSet)
	{
		HoveredAttribute = true;
		SetHover(true);
	}
}

void SWidget::OnMouseLeave(const FPointer& InPointer)
{
	if (bIsHoveredAttributeSet)
	{
		HoveredAttribute = false;
		SetHover(false);
	}
}

FCursorReply SWidget::OnCursorQuery(const FPointer& InPointer) const
{
	// TODO : 커서 쿼리 처리
	//TOptional<EMouseCursor::Type> TheCursor = GetCursor();
	//return (TheCursor.IsSet())
	//	? FCursorReply::Cursor(TheCursor.GetValue())
	//	: FCursorReply::Unhandled();
	return FCursorReply::Cursor(EMouseCursor::Default);
}

FReply SWidget::OnMouseButtonDoubleClick(const FPointer& InPointer)
{
	// TODO : 마우스 더블클릭 처리
	//if (TSharedPtr<FSlateMouseEventsMetaData> Data = GetMetaData<FSlateMouseEventsMetaData>())
	//{
	//	if (Data->MouseDoubleClickHandle.IsBound())
	//	{
	//		return Data->MouseDoubleClickHandle.Execute(MyGeometry, MouseEvent);
	//	}
	//}
	return FReply();
}

void SWidget::Tick(const FGeometry& InParentGeometry, float DeltaTime)
{
	// 1. 자신의 LayoutTransform을 로컬 위치(LocalPosition)를 바탕으로 갱신합니다.
	LayoutTransform = FSlateLayoutTransform(1.0f, LocalPosition);

	// 2. 부모 Geometry(InParentGeometry)를 사용해 자신(현재 위젯)의 Geometry를 계산합니다.
	//    MakeChild를 사용하더라도, 이 호출 결과는 "현재 위젯(자신)"의 Geometry를 의미합니다.
	FGeometry NewMyGeometry = InParentGeometry.MakeChild(LocalSize, LayoutTransform, FSlateRenderTransform(), FVector2D(0.5f, 0.5f));
    
	// 3. 계산된 자신의 Geometry를 저장합니다.
	MyGeometry = NewMyGeometry;
    
	// 4. 자신의 Geometry(MyGeometry)를 부모 Geometry로 하여 자식 위젯들의 Tick을 호출합니다.
	for(auto& Child : Children)
	{
		Child->Tick(MyGeometry, DeltaTime);
	}
}

void SWidget::SetLocalPosition(const FVector2D& NewPosition)
{
	LocalPosition = NewPosition;
	LayoutTransform = FSlateLayoutTransform(1.0f, LocalPosition);
}

void SWidget::AssignParentWidget(const std::shared_ptr<SWidget>& InParent)
{
	if (InParent)
	{
		ParentWidgetPtr = InParent;
		InParent->AddChild(shared_from_this());
	}
}

float SWidget::GetRelativeLayoutScale(const int32 ChildIndex, float LayoutScaleMultiplier) const
{
	return 1.0f;
}

void SWidget::ArrangeChildren(const FGeometry& AllottedGeometry, TArray<FArrangedWidget>& ArrangedChildren) const
{
	OnArrangeChildren(AllottedGeometry, ArrangedChildren);
}

void SWidget::ArrangeSingleChild(const FGeometry& AllottedGeometry, TArray<FArrangedWidget>& ArrangedChildren, std::shared_ptr<SWidget> InChild,
	const FVector2D& ContentScale)
{
	const FMargin SlotPadding = FMargin(5.f);
	
	// 할당된 전체 영역에서 패딩을 제외한 영역 크기를 계산하고, ContentScale을 적용합니다.
	const FVector2D AllottedSize = AllottedGeometry.GetLocalSize();
	const FVector2D AdjustedSize = FVector2D(
		(AllottedSize.X - SlotPadding.Left - SlotPadding.Right) * ContentScale.X,
		(AllottedSize.Y - SlotPadding.Top - SlotPadding.Bottom) * ContentScale.Y
	);
    
	// 자식 위젯의 오프셋은 패딩의 왼쪽, 위쪽 값으로 설정합니다.
	const FVector2D Offset = FVector2D(SlotPadding.Left, SlotPadding.Top);
    
	// 부모의 누적 Geometry와 로컬 변환(오프셋)을 결합하여 자식의 Geometry를 생성합니다.
	FGeometry ChildGeometry = AllottedGeometry.MakeChild(AdjustedSize, FSlateLayoutTransform(Offset));
    
	// 계산된 Geometry와 가시성을 사용하여 ArrangedChildren 배열에 이 위젯을 추가합니다.
	ArrangedChildren.Add({InChild, ChildGeometry});
}

bool SWidget::IsChildActive(const std::shared_ptr<SWidget>& Child, const FPointer& InPointer,
                            const FGeometry& ChildGeometry)
{
	// 1. 포인터의 스크린 좌표를 Child의 로컬 좌표로 변환합니다.
	const FVector2D LocalPos = ChildGeometry.AbsoluteToLocal(InPointer.ScreenSpacePosition);
	const FVector2D WidgetSize = ChildGeometry.GetLocalSize();
    
	// 2. 포인터가 자식 위젯 영역 내에 있는지 체크합니다.
	bool bIsHovered = (LocalPos.X >= 0 && LocalPos.X <= WidgetSize.X &&
						 LocalPos.Y >= 0 && LocalPos.Y <= WidgetSize.Y);
    
	// 3. 전역 입력 관리자를 통해 현재 마우스 캡처를 받고 있는 위젯을 확인합니다.
	//    (예를 들어, APlayerInput::Get().GetMouseCaptor()가 현재 캡처된 위젯을 반환한다고 가정)
	const std::shared_ptr<SWidget> CapturedWidget = APlayerInput::Get().GetMouseCaptor();
	const bool bIsCaptured = (CapturedWidget == Child);
    
	// 4. 둘 다 만족하면 자식 위젯이 활성(호버 및 캡처) 상태임을 반환합니다.
	return bIsHovered && bIsCaptured;
}

bool SWidget::HasFocusedDescendants(const FPointer& InPointer) const
{
	bool isAnyChildFocus = false;
	for (auto Child : Children)
	{
		isAnyChildFocus = IsChildActive(Child, InPointer, Child->GetGeometry());
		if (isAnyChildFocus) return true;
	}
	return isAnyChildFocus;
}

bool SWidget::IsAccessible() const
{
	std::shared_ptr<SWidget> Parent = GetParentWidget();

	while (Parent)
	{
		if (!Parent->CanChildrenBeAccessible())
		{
			return false;
		}
		Parent = Parent->GetParentWidget();
	}
	return true;
}

bool SWidget::CanChildrenBeAccessible() const
{
	return bCanChildrenBeAccessible;
}

void SWidget::SetCanChildrenBeAccessible(bool bInCanChildrenBeAccessible)
{
	if (bCanChildrenBeAccessible != bInCanChildrenBeAccessible)
	{
		bCanChildrenBeAccessible = bInCanChildrenBeAccessible;
	}
}

int32 SWidget::FindChildUnderMouse(const TArray<FArrangedWidget>& Children, const FPointer& InPointer)
{
	const FVector2D AbsoluteCursorLocation = InPointer.ScreenSpacePosition;
	return SWidget::FindChildUnderPosition( Children, AbsoluteCursorLocation );
}

int32 SWidget::FindChildUnderPosition(const TArray<FArrangedWidget>& Children, const FVector2D& ArrangedSpacePosition)
{
	const int32 NumChildren = Children.Num();
	for( int32 ChildIndex = NumChildren - 1; ChildIndex >= 0; --ChildIndex )
	{
		const FArrangedWidget& Candidate = Children[ChildIndex];
		const bool bCandidateUnderCursor = 
			// Candidate is physically under the cursor
			Candidate.Geometry.IsUnderLocation( ArrangedSpacePosition );

		if (bCandidateUnderCursor)
		{
			return ChildIndex;
		}
	}

	return -1;
}
