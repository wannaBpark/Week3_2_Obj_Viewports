#pragma once
#include "memory"
#include "Core/Container/Set.h"
#include "Core/Container/Map.h"
#include "Core/HAL/PlatformType.h"

#include "Core/Input/PlayerInput.h"

#include "Core/Math/Geometry.h"
#include "optional"

struct FReply;
struct FCursorReply;

enum class EVisibility
{
	Visible,
	Collapsed,
	Hidden,
	All,
};

enum class EInvalidateWidgetReason
{
	None = 0,
	Paint = 1 << 1,
	/**
	* Use if just the volatility of the widget has been adjusted.
	*/
	Volatility = 1 << 2,
	ChildOrder = 1 << 3,
	/**
	 * Changing visibility (this implies layout)
	*/
	Visibility = 1 << 5,
	PaintAndVolatility = Paint | Volatility,
};

struct FSlateWidgetPersistentState
{
public:
	FSlateWidgetPersistentState()
		: LayerId(0)
		, OutgoingLayerId(0)
		, IncomingUserIndex(-1)
		, bParentEnabled(true)
		, bDeferredPainting(false)
	{
	}

	std::weak_ptr<SWidget> PaintParent;
	FGeometry AllottedGeometry;
	FGeometry DesktopGeometry;
	FSlateRect CullingBounds;

	int32 LayerId;
	int32 OutgoingLayerId;
	int8 IncomingUserIndex;
	bool bParentEnabled;
	bool bDeferredPainting;

};

class SWidget : public std::enable_shared_from_this<SWidget>
{
    friend class SWindow;
public:
    enum class EState
    {
        Disable,
        Active,
        Destroy,
    };

    SWidget();
    virtual ~SWidget();

    int32 Paint(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, int32 LayerId /*, 색상(?)*/, bool bParentEnabled);
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) = 0;
    virtual FReply OnFocusReceived(const FGeometry& MyGeometry /*, const FFocusEvent& InFocusEvent*/) = 0;
    virtual void OnFocusLost(/*const FFocusEvent& InFocusEvent*/) = 0;
    virtual FReply OnKeyChar(const FGeometry& MyGeometry, const EKeyCode& InKeyCode /*, const FKeyEvent& InKeyEvent*/) = 0;
    virtual FReply OnKeyDown(const FGeometry& MyGeometry, const EKeyCode& InKeyCode /*, const FKeyEvent& InKeyEvent*/) = 0;
    virtual FReply OnKeyUp(const FGeometry& MyGeometry, const EKeyCode& InKeyCode /*, const FKeyEvent& InKeyEvent*/) = 0;
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointer& InPointer /*, const FPointerEvent& MouseEvent*/);
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointer& InPointer /*, const FPointerEvent& MouseEvent*/);
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointer& InPointer /*, const FPointerEvent& MouseEvent*/);
    virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointer& InPointer /*, const FPointerEvent& MouseEvent*/);
    virtual void OnMouseLeave(const FPointer& InPointer/*, const FPointerEvent& MouseEvent*/);
    virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointer& InPointer /*, const FPointerEvent& MouseEvent*/) = 0;
    virtual FCursorReply OnCursorQuery(const FGeometry& MyGeometry, const FPointer& InPointer /*, const FPointerEvent& CursorEvent*/) const;
    virtual std::optional<std::shared_ptr<SWidget>> OnMapCursor(const FCursorReply& CursorReply) const = 0;
    virtual FReply OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointer& InPointer /*, const FPointerEvent& InMouseEvent*/);
    virtual FReply OnDragDetected(const FGeometry& MyGeometry, const FPointer& InPointer /*, const FPointerEvent& MouseEvent*/) = 0;
    virtual void OnDragEnter(const FGeometry& MyGeometry, const FPointer& InPointer /*, const FDragDropEvent& DragDropEvent*/) = 0;
    virtual void OnDragLeave(const FPointer& InPointer/*const FDragDropEvent& DragDropEvent*/) = 0;
    virtual FReply OnDragOver(const FGeometry& MyGeometry, const FPointer& InPointer /*, const FDragDropEvent& DragDropEvent*/) = 0;
    virtual FReply OnDrop(const FGeometry& MyGeometry, const FPointer& InPointer /*, const FDragDropEvent& DragDropEvent*/) = 0;
    virtual void OnFinishedPointerInput() = 0;
    virtual void OnFinishedKeyInput() = 0;

    void SetCanTick(bool bInCanTick) { bCanTick = bInCanTick; }
    bool GetCanTick() { return bCanTick; }

    /** @return 위젯이 어떤 Slate 속성에 바인딩되어 있으면 true를 반환 */
    bool HasRegisteredSlateAttribute() const { return bHasRegisteredSlateAttribute; }
    /** @return 위젯이 등록된 Slate 속성을 자동으로 업데이트할지, 수동으로 업데이트해야 하는지를 나타냄 */
    bool IsAttributesUpdatesEnabled() const { return bEnabledAttributesUpdate; }

    const FSlateWidgetPersistentState& GetPersistentState() const { return PersistentState; }

    void Invalidate(EInvalidateWidgetReason InvalidateReason);

    FVector2D GetDesiredSize() const;
    void AssignParentWidget(std::shared_ptr<SWidget> InParent);

    bool IsParentValid() const { return ParentWidgetPtr.lock() != nullptr; }
    std::shared_ptr<SWidget> GetParentWidget() const { return ParentWidgetPtr.lock(); }
    std::shared_ptr<SWidget> Advanced_GetPaintParentWidget() const { return PersistentState.PaintParent.lock(); }

    virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const { return FVector2D(0, 0); }

    void SetDesiredSize(const FVector2D& InDesiredSize)
    {
        DesiredSize = InDesiredSize;
        bDesiredSizeSet = true;
    }

    virtual float GetRelativeLayoutScale(const int32 ChildIndex, float LayoutScaleMultiplier) const;

    void ArrangeChildren(const FGeometry& AllottedGeometry, TArray<FArrangedWidget>& ArrangedChildren, bool bUpdateAttributes = false) const;

    virtual TArray<std::shared_ptr<SWidget>> GetChildren() = 0;

    virtual bool SupportsKeyboardFocus() const;

    virtual bool HasKeyboardFocus() const;

    bool HasFocusedDescendants() const;

    bool HasMouseCapture() const;

    virtual void OnMouseCaptureLost() = 0;

    void SetEnabled(bool InEnabled)
    {
        EnabledAttribute = InEnabled;
    }

    inline bool IsEnabled() const
    {
        return EnabledAttribute;
    }

    virtual bool IsInteractable() const
    {
        return false;
    }

    bool IsHovered() const;

    bool IsDirectlyHovered() const;

    void SetHover(bool InHovered)
    {
        bIsHoveredAttributeSet = InHovered;
    }

    inline EVisibility GetVisibility() const { return VisibilityAttribute; }

    virtual void SetVisibility(EVisibility InVisibility);

    bool IsAccessible() const;

    bool CanChildrenBeAccessible() const;

    void SetCanChildrenBeAccessible(bool bInCanChildrenBeAccessible);

    bool IsChildWidgetCulled(const FSlateRect& MyCullingRect, const TArray<FArrangedWidget>& ArrangedChild) const;

    std::optional<FSlateRenderTransform> GetRenderTransform() const
    {
        return RenderTransformAttribute;
    }

    std::optional<FSlateRenderTransform> GetRenderTransformWithRespectToFlowDirection() const;

    FVector2D GetRenderTransformPivotWithRespectToFlowDirection() const;

    inline void SetRenderTransform(std::optional<FSlateRenderTransform> InRenderTransform)
    {
        RenderTransformAttribute = InRenderTransform;
    }

    inline FVector2D GetRenderTransformPivot() const { return RenderTransformPivotAttribute; }

    inline void SetRenderTransformPivot(FVector2D InRenderTransformPivot)
    {
        RenderTransformPivotAttribute = InRenderTransformPivot;
    }

    void SetCursor(const std::optional<EMouseCursor>& InCursor);

    virtual std::optional<EMouseCursor> GetCursor() const;

    ///** OnMouseButtonDown 이벤트 참조 */
    //void SetOnMouseButtonDown(FPointerEventHandler EventHandler);

    ///** OnMouseButtonUp 이벤트 참조 */
    //void SetOnMouseButtonUp(FPointerEventHandler EventHandler);

    ///** OnMouseMove 이벤트 참조 */
    //void SetOnMouseMove(FPointerEventHandler EventHandler);

    ///** OnMouseDoubleClick 이벤트 참조 */
    //void SetOnMouseDoubleClick(FPointerEventHandler EventHandler);

    ///** OnMouseEnter 이벤트 참조 */
    //void SetOnMouseEnter(FNoReplyPointerEventHandler EventHandler);

    ///** OnMouseLeave 이벤트 참조 */
    //void SetOnMouseLeave(FSimpleNoReplyPointerEventHandler EventHandler);

    const FGeometry& GetCachedGeometry() const;

    const FGeometry& GetPaintSpaceGeometry() const;

    void SWidgetConstruct(bool InbEnabled, EVisibility InVisibility, std::optional<FSlateRenderTransform> InRenderTransform, FVector2D InRenderTransformPivot, bool InbEnabledAttributesUpdate, std::optional<EMouseCursor> InCursor, bool InbCanChildrenBeAccessible);

    bool FindChildGeometries(const FGeometry& MyGeometry, const TSet<std::shared_ptr<SWidget> >& WidgetsToFind, TMap<std::shared_ptr<SWidget>, FArrangedWidget>& OutResult) const;

    void FindChildGeometries_Helper(const FGeometry& MyGeometry, const TSet<std::shared_ptr<SWidget> >& WidgetsToFind, TMap<std::shared_ptr<SWidget>, FArrangedWidget>& OutResult) const;

    FGeometry FindChildGeometry(const FGeometry& MyGeometry, std::shared_ptr<SWidget> WidgetToFind) const;

    /** @return 현재 마우스가 가리키고 있는 자식 위젯의 인덱스 */
    static int32 FindChildUnderMouse(const TArray<FArrangedWidget>& Children);

    /** @return 지정된 위치 아래에 있는 자식 위젯의 인덱스 */
    static int32 FindChildUnderPosition(const TArray<FArrangedWidget>& Children, const FVector2D& ArrangedSpacePosition);

    bool ShouldBeEnabled(bool InParentEnabled) const
    {
        // 부모가 활성화되어 있고, 이 위젯도 활성화되어 있으면 true 반환
        return InParentEnabled && IsEnabled();
    }

    virtual bool ComputeVolatility() const { return false; }

    static EVisibility AccessWidgetVisibility(const std::shared_ptr<SWidget>& Widget)
    {
        return Widget->VisibilityAttribute;
    }
private:

    virtual int32 OnPaint(/*const FPaintArgs& Args, */const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect /*, FSlateWindowElementList& OutDrawElements*/, int32 LayerId /*, const FWidgetStyle& InWidgetStyle*/, bool bParentEnabled) const { return -1; }

    virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, TArray<FArrangedWidget>& ArrangedChildren) const = 0;
public:
    bool GetEnabledAttribute() { return &EnabledAttribute; }
    bool GetHoveredAttribute() { return &HoveredAttribute; }
    EVisibility* GetVisibilityAttribute() { return &VisibilityAttribute; }
    std::optional<FSlateRenderTransform> GetRenderTransformAttribute() { return RenderTransformAttribute; }
    FVector2D GetRenderTransformPivotAttribute() { return RenderTransformPivotAttribute; }

private:
    bool bNeedsPrepass;

    bool bHasRegisteredSlateAttribute;

    bool bEnabledAttributesUpdate;
    /** IsHovered 속성이 설정되었는지 여부 */
    bool bIsHoveredAttributeSet;
    /** DesiredSize가 설정되었는지 여부 */
    bool bDesiredSizeSet;
    /** 위젯이 키보드 포커스를 지원할 수 있는지 여부 */
    bool bCanSupportFocus;

    bool bCanHaveChildren;

    /** 이 위젯이 플랫폼의 접근성 API에 노출되는 방식과 관련된 모든 변수들 */
    bool bCanChildrenBeAccessible;

    bool bCanTick;

    EVisibility VisibilityAttribute;
    bool EnabledAttribute;
    bool HoveredAttribute;

    /** 이 위젯이 갖고자 하는 이상적인 크기를 저장합니다. (bDesiredSizeSet 참조) */
    FVector2D DesiredSize;

    FSlateWidgetPersistentState PersistentState;

    /** 이 위젯의 렌더 트랜스폼 피벗 (정규화된 로컬 좌표) */
    FVector2D RenderTransformPivotAttribute;

    /** 이 위젯의 렌더 트랜스폼. 값이 없으면 비용이 많이 드는 처리를 건너뛸 수 있도록 TOptional 사용 */
    std::optional<FSlateRenderTransform> RenderTransformAttribute;

    /** 이 위젯의 부모 위젯에 대한 포인터. null이면 루트 위젯이거나 위젯 트리에 포함되어 있지 않음을 의미 */
    std::weak_ptr<SWidget> ParentWidgetPtr;

    TArray<std::shared_ptr<SWidget>> ChildrenWidgetPtr;

    uint64 UniqueIdentifier;
};
