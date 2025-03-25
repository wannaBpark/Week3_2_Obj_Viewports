#include "SWidget.h"
#include "ArrangedWidget.h"
#include "Core/Rendering/Widget/Reply.h"

SWidget::SWidget()
	: bCanSupportFocus(true)
	, bCanHaveChildren(true)
	, bNeedsPrepass(true)
	, bHasRegisteredSlateAttribute(false)
	, bEnabledAttributesUpdate(true)
	, bIsHoveredAttributeSet(false)
	, bDesiredSizeSet(false)
	, bCanChildrenBeAccessible(true)
	// Note we are defaulting to tick for backwards compatibility
	, VisibilityAttribute(EVisibility::Visible)
	, HoveredAttribute(false)
	, DesiredSize(FVector2D(0.0f, 0.0f))
	, RenderTransformPivotAttribute(FVector2D::ZeroVector)
	, RenderTransformAttribute()
{
}

SWidget::~SWidget()
{
}

int32 SWidget::Paint(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, int32 LayerId, bool bParentEnabled)
{
	SWidget* MutableThis = const_cast<SWidget*>(this);

	FGeometry DesktopSpaceGeometry = AllottedGeometry;
	DesktopSpaceGeometry.AppendTransform(FSlateLayoutTransform(/*WindowOffset*/));
	
	/*MutableThis->Tick();*/

	PersistentState.LayerId = LayerId;
	PersistentState.bParentEnabled = bParentEnabled;
	PersistentState.bDeferredPainting = true;
	PersistentState.AllottedGeometry = AllottedGeometry;
	PersistentState.DesktopGeometry = DesktopSpaceGeometry;
	PersistentState.CullingBounds = MyCullingRect;
	
	// TODO : 자신 그리고

	// TODO : 자식들 그림
	for (std::shared_ptr<SWidget> Child : MutableThis->GetChildren())
	{
		if (Child->GetVisibility() == EVisibility::Visible)
		{
			Child->Paint(Child->GetCachedGeometry(), MyCullingRect, LayerId, bParentEnabled);
			const bool bIsChildDeferredPaint = Child->GetPersistentState().bDeferredPainting;
			if(!bIsChildDeferredPaint && Child->GetVisibility() == EVisibility::Visible)
			{
				// 일단 신경 안써도 될 듯
				//if (NewLayerId < Child->GetPersistentState().OutgoingLayerId)
				//{
				//	ensureAlwaysMsgf(false, TEXT("The widget '%s' Outgoing Layer Id is bigger than its parent."), *FReflectionMetaData::GetWidgetPath(DebugChild.Get()));
				//	CVarSlateEnsureOutgoingLayerId->Set(false, CVarSlateEnsureOutgoingLayerId->GetFlags());
				//}
			}
		}
	}

	// TODO : 위젯 그리는 거 끝났을 때, Delegate 호출

	if (bCanSupportFocus && SupportsKeyboardFocus())
	{
		// bool bShowUserFocus = FSlateApplicationBase::Get().ShowUserFocus(SharedThis(this));
		// Focus 됬을 때, 하이라이트(?)
	}

	return int32();
}

FReply SWidget::OnKeyDown(const FGeometry& MyGeometry, const EKeyCode& InKeyCode)
{
	if (bCanSupportFocus && SupportsKeyboardFocus())
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

FReply SWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointer&InPointer)
{
	// TODO : 마우스 버튼 눌렀을 때 처리
	//if (TSharedPtr<FSlateMouseEventsMetaData> Data = GetMetaData<FSlateMouseEventsMetaData>())
	//{
	//	if (Data->MouseButtonDownHandle.IsBound())
	//	{
	//		return Data->MouseButtonDownHandle.Execute(MyGeometry, MouseEvent);
	//	}
	//}
	return FReply();
}

FReply SWidget::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointer& InPointer)
{
	// TODO : 마우스 버튼 뗐을 때 처리
	//if (TSharedPtr<FSlateMouseEventsMetaData> Data = GetMetaData<FSlateMouseEventsMetaData>())
	//{
	//	if (Data->MouseButtonUpHandle.IsBound())
	//	{
	//		return Data->MouseButtonUpHandle.Execute(MyGeometry, MouseEvent);
	//	}
	//}
	return FReply();
}

FReply SWidget::OnMouseMove(const FGeometry& MyGeometry, const FPointer& InPointer)
{
	// TODO : 마우스 움직였을 때 처리
	//if (TSharedPtr<FSlateMouseEventsMetaData> Data = GetMetaData<FSlateMouseEventsMetaData>())
	//{
	//	if (Data->MouseMoveHandle.IsBound())
	//	{
	//		return Data->MouseMoveHandle.Execute(MyGeometry, MouseEvent);
	//	}
	//}
	return FReply();
}

void SWidget::OnMouseEnter(const FGeometry& MyGeometry, const FPointer& InPointer)
{
	if (!bIsHoveredAttributeSet)
	{
		HoveredAttribute = true;
	}

	// TODO : 마우스가 위젯에 들어왔을 때 처리
	//if (TSharedPtr<FSlateMouseEventsMetaData> Data = GetMetaData<FSlateMouseEventsMetaData>())
	//{
	//	if (Data->MouseEnterHandler.IsBound())
	//	{
	//		// A valid handler is assigned; let it handle the event.
	//		Data->MouseEnterHandler.Execute(MyGeometry, MouseEvent);
	//	}
	//}
}

void SWidget::OnMouseLeave(const FPointer& InPointer)
{
	if (bIsHoveredAttributeSet)
	{
		HoveredAttribute = false;
	}
	// TODO : 마우스가 위젯에서 나갔을 때 처리
	//if (TSharedPtr<FSlateMouseEventsMetaData> Data = GetMetaData<FSlateMouseEventsMetaData>())
	//{
	//	if (Data->MouseLeaveHandler.IsBound())
	//	{
	//		// A valid handler is assigned; let it handle the event.
	//		Data->MouseLeaveHandler.Execute(MyGeometry, MouseEvent);
	//	}
	//}
}

FCursorReply SWidget::OnCursorQuery(const FGeometry& MyGeometry, const FPointer& InPointer) const
{
	// TODO : 커서 쿼리 처리
	//TOptional<EMouseCursor::Type> TheCursor = GetCursor();
	//return (TheCursor.IsSet())
	//	? FCursorReply::Cursor(TheCursor.GetValue())
	//	: FCursorReply::Unhandled();
	return FCursorReply::Cursor(EMouseCursor::Default);
}

FReply SWidget::OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointer& InPointer)
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

void SWidget::Invalidate(EInvalidateWidgetReason InvalidateReason)
{
	// TODO : 위젯을 무효화 처리
	//if (InvalidateReason == EInvalidateWidgetReason::None)
	//{
	//	return;
	//}
	//if (InvalidateReason & EInvalidateWidgetReason::Paint)
	//{
	//	// TODO : Paint 무효화 처리
	//}
	//if (InvalidateReason & EInvalidateWidgetReason::ChildOrder)
	//{
	//	// TODO : ChildOrder 무효화 처리
	//}
}

FVector2D SWidget::GetDesiredSize() const
{
	return bDesiredSizeSet ? DesiredSize : FVector2D(0.0f, 0.0f);
}

void SWidget::AssignParentWidget(std::shared_ptr<SWidget> InParent)
{
	if (InParent)
	{
		ParentWidgetPtr = InParent;
	}

	// TODO : 부모 위젯이 바뀌었을 때 처리
	if (InParent)
	{
		InParent->Invalidate(EInvalidateWidgetReason::ChildOrder);
	}
}

float SWidget::GetRelativeLayoutScale(const int32 ChildIndex, float LayoutScaleMultiplier) const
{
	return 1.0f;
}

void SWidget::ArrangeChildren(const FGeometry& AllottedGeometry, TArray<FArrangedWidget>& ArrangedChildren, bool bUpdateAttributes) const
{
	if (bUpdateAttributes)
	{
		// TODO : 어트리뷰트 업데이트
		//FSlateAttributeMetaData::UpdateChildrenOnlyVisibilityAttributes(const_cast<SWidget&>(*this), FSlateAttributeMetaData::EInvalidationPermission::DelayInvalidation, false);
	}

	OnArrangeChildren(AllottedGeometry, ArrangedChildren);
}

bool SWidget::SupportsKeyboardFocus() const
{
	return false;
}

bool SWidget::HasKeyboardFocus() const
{
	return false;
}

bool SWidget::HasFocusedDescendants() const
{
	// TODO : 포커스된 자식 위젯이 있는지 판단
	//return FSlateApplicationBase::Get().HasFocusedDescendants(SharedThis(this));
	return false;
}

bool SWidget::HasMouseCapture() const
{
	// TODO : 마우스 캡쳐된 상태인지 판단
	//return FSlateApplicationBase::Get().DoesWidgetHaveMouseCapture(SharedThis(this));
	return false;
}

bool SWidget::IsHovered() const
{
	return HoveredAttribute;
}

bool SWidget::IsDirectlyHovered() const
{
	// TODO : 직접적으로 마우스가 위젯에 올라와 있는지 판단
	//return FSlateApplicationBase::Get().IsWidgetDirectlyHovered(SharedThis(this));
	return false;
}

void SWidget::SetVisibility(EVisibility InVisibility)
{
	VisibilityAttribute = InVisibility;
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
		//FSlateApplicationBase::Get().GetAccessibleMessageHandler()->MarkDirty();
	}
}

bool SWidget::IsChildWidgetCulled(const FSlateRect& MyCullingRect, const TArray<FArrangedWidget>& ArrangedChild) const
{
	return false;
}

FReply& FReply::SetMousePos(const FIntPoint& NewMousePos)
{
	this->bSetUserFocus = true;
	this->bReleaseUserFocus = false;

	return *this;
}

FReply& FReply::SetUserFocus(std::shared_ptr<SWidget> GiveMeFocus)
{
	this->FocusRecipient = GiveMeFocus;

	return *this;
}

FReply& FReply::ClearUserFocus()
{
	bSetUserFocus = false;
	return *this;
}

FReply& FReply::CancelFocusRequest()
{
	this->bSetUserFocus = false;
	this->FocusRecipient.reset();
	this->bReleaseUserFocus = false;

	return *this;
}

std::optional<FSlateRenderTransform> SWidget::GetRenderTransformWithRespectToFlowDirection() const
{
	return std::optional<FSlateRenderTransform>();
}

FVector2D SWidget::GetRenderTransformPivotWithRespectToFlowDirection() const
{
	return FVector2D(0.5f, 0.5f);
}

void SWidget::SetCursor(const std::optional<EMouseCursor>& InCursor)
{
}

std::optional<EMouseCursor> SWidget::GetCursor() const
{
	return std::optional<EMouseCursor>();
}

const FGeometry& SWidget::GetCachedGeometry() const
{
	return PersistentState.DesktopGeometry;
}

const FGeometry& SWidget::GetPaintSpaceGeometry() const
{
	return PersistentState.AllottedGeometry;
}

void SWidget::SWidgetConstruct(bool InbEnabled, EVisibility InVisibility, std::optional<FSlateRenderTransform> InRenderTransform, FVector2D InRenderTransformPivot, bool InbEnabledAttributesUpdate, std::optional<EMouseCursor> InCursor, bool InbCanChildrenBeAccessible)
{
	SetEnabled(InbEnabled);
	VisibilityAttribute = InVisibility; // SetVisibility is virtual, assign directly to stay backward compatible
	SetRenderTransform(InRenderTransform);
	SetRenderTransformPivot(InRenderTransformPivot);
	bEnabledAttributesUpdate = InbEnabledAttributesUpdate;

	if (InCursor.has_value())
	{
		SetCursor(InCursor);
	}

	SetCanChildrenBeAccessible(InbCanChildrenBeAccessible);

}

bool SWidget::FindChildGeometries(const FGeometry& MyGeometry, const TSet<std::shared_ptr<SWidget>>& WidgetsToFind, TMap<std::shared_ptr<SWidget>, FArrangedWidget>& OutResult) const
{
	FindChildGeometries_Helper(MyGeometry, WidgetsToFind, OutResult);
	return OutResult.Num() == WidgetsToFind.Num();
}

void SWidget::FindChildGeometries_Helper(const FGeometry& MyGeometry, const TSet<std::shared_ptr<SWidget>>& WidgetsToFind, TMap<std::shared_ptr<SWidget>, FArrangedWidget>& OutResult) const
{
	// Perform a breadth first search!

	TArray<FArrangedWidget> ArrangedChildren;
	this->ArrangeChildren(MyGeometry, ArrangedChildren);
	const int32 NumChildren = ArrangedChildren.Num();

	// See if we found any of the widgets on this level.
	for (int32 ChildIndex = 0; ChildIndex < NumChildren; ++ChildIndex)
	{
		const FArrangedWidget& CurChild = ArrangedChildren[ChildIndex];

		if (WidgetsToFind.Find(CurChild.Widget) != WidgetsToFind.end())
		{
			// We found one of the widgets for which we need geometry!
			OutResult.Add(CurChild.Widget, CurChild);
		}
	}

	// If we have not found all the widgets that we were looking for, descend.
	if (OutResult.Num() != WidgetsToFind.Num())
	{
		// Look for widgets among the children.
		for (int32 ChildIndex = 0; ChildIndex < NumChildren; ++ChildIndex)
		{
			const FArrangedWidget& CurChild = ArrangedChildren[ChildIndex];
			CurChild.Widget->FindChildGeometries_Helper(CurChild.Geometry, WidgetsToFind, OutResult);
		}
	}
}

FGeometry SWidget::FindChildGeometry(const FGeometry& MyGeometry, std::shared_ptr<SWidget> WidgetToFind) const
{
	// We just need to find the one WidgetToFind among our descendants.
	TSet<std::shared_ptr<SWidget>> WidgetsToFind;
	{
		WidgetsToFind.Add(WidgetToFind);
	}

	TMap<std::shared_ptr<SWidget>, FArrangedWidget> Result;

	FindChildGeometries(MyGeometry, WidgetsToFind, Result);

	return Result.Find(WidgetToFind)->Geometry;
}

int32 SWidget::FindChildUnderMouse(const TArray<FArrangedWidget>& Children)
{
	// TODO : MouseEvent
	//FVector2D AbsoluteCursorLocation = MouseEvent.GetScreenSpacePosition();
	//return SWidget::FindChildUnderPosition(Children, AbsoluteCursorLocation);

	return 0;
}

int32 SWidget::FindChildUnderPosition(const TArray<FArrangedWidget>& Children, const FVector2D& ArrangedSpacePosition)
{
	const int32 NumChildren = Children.Num();
	for (int32 ChildIndex = NumChildren - 1; ChildIndex >= 0; --ChildIndex)
	{
		const FArrangedWidget& Candidate = Children[ChildIndex];
		const bool bCandidateUnderCursor =
			// Candidate is physically under the cursor
			Candidate.Geometry.IsUnderLocation(ArrangedSpacePosition);

		if (bCandidateUnderCursor)
		{
			return ChildIndex;
		}
	}

	return -1;
}
