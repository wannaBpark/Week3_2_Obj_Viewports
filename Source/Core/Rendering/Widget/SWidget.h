#pragma once
#include "memory"
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

    int32 Paint(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, int32 LayerId, bool bParentEnabled);
    virtual FReply OnFocusReceived() = 0;
    virtual void OnFocusLost() = 0;
    virtual FReply OnKeyChar(const EKeyCode& InKeyCode) = 0;
    virtual FReply OnKeyDown(const EKeyCode& InKeyCode) = 0;
    virtual FReply OnKeyUp(const EKeyCode& InKeyCode) = 0;
    virtual FReply OnMouseButtonDown(EMouseButton InMouseButton, const FPointer& InPointer);
    virtual FReply OnMouseButtonUp(EMouseButton InMouseButton, const FPointer& InPointer);
    virtual FReply OnMouseMove(const FPointer& InPointer);
    virtual void OnMouseEnter(const FPointer& InPointer);
    virtual void OnMouseLeave(const FPointer& InPointer);
    virtual FReply OnMouseWheel(const FPointer& InPointer) = 0;
    virtual FCursorReply OnCursorQuery(const FPointer& InPointer) const;
    virtual std::optional<std::shared_ptr<SWidget>> OnMapCursor(const FCursorReply& CursorReply) const = 0;
    virtual FReply OnMouseButtonDoubleClick(const FPointer& InPointer);
    virtual FReply OnDragDetected(const FPointer& InPointer) = 0;
    virtual void OnDragEnter(const FPointer& InPointer) = 0;
    virtual void OnDragLeave(const FPointer& InPointer) = 0;
    virtual FReply OnDragOver(const FPointer& InPointer) = 0;
    virtual FReply OnDrop(const FPointer& InPointer) = 0;
    virtual void OnFinishedPointerInput() = 0;
    virtual void OnFinishedKeyInput() = 0;

public:
	// 매 틱마다 호출되어 부모의 Geometry를 바탕으로 자신의 Geometry를 갱신합니다.
	virtual void Tick(const FGeometry& ParentGeometry, float DeltaTime);

	// 위젯의 현재 Geometry 정보를 반환합니다.
	[[nodiscard]] const FGeometry& GetGeometry() const { return MyGeometry; }
	void SetGeometry(const FGeometry& NewGeometry) { MyGeometry = NewGeometry; }

	// 위젯의 위치나 크기를 변경할 수 있는 인터페이스 (예: 애니메이션, 사용자 입력 등)
    void SetLocalPosition(const FVector2D& NewPosition);
	
    void SetLocalSize(const FVector2D& NewSize)
    {
		LocalSize = NewSize;
    }
	
	void SWidget::AddChild(const std::shared_ptr<SWidget>& Child)
	{
		Children.push_back(Child);
	}

    static bool IsInsideWidget(const FVector2D& ScreenPos, const FGeometry& MyGeometry)
	{
		FVector2D LocalPos = MyGeometry.AbsoluteToLocal(ScreenPos);
		return (LocalPos.X >= 0 && LocalPos.X <= MyGeometry.GetLocalSize().X &&
				LocalPos.Y >= 0 && LocalPos.Y <= MyGeometry.GetLocalSize().Y);
	}

	/**
	 * 자식 위젯 배치를 위한 비가상(Non-virtual) 진입점 함수입니다.
	 * 공통 작업이 실행된 후 가상 ArrangeChildren 함수를 호출하도록 보장합니다.
	 * 모든 자식 위젯의 Geometry를 계산하고, 그 값들로 ArrangedChildren 리스트를 채웁니다.
	 * 각 레이아웃 패널 유형은 원하는 동작에 따라 자식들을 배치해야 합니다.
	 * 
	 * 선택적으로, 가상 ArrangeChildren 함수를 실행하기 전에 자식의 collapsed 속성(가시성에 영향을 주는 속성)을 업데이트합니다.
	 * 가시성 속성은 매 프레임 한 번 업데이트됩니다 (SlatePrepass 참조).
	 * 일반적인 SWidget의 Paint/Tick 외부에서 ArrangeChildren을 호출하는 경우에 이 옵션을 사용합니다.
	 *
	 * @param AllottedGeometry    부모에 의해 이 위젯에 할당된 Geometry.
	 * @param ArrangedChildren    배치된 자식 위젯의 WidgetGeometry를 추가할 배열.
	 */
	void ArrangeChildren(const FGeometry& AllottedGeometry, TArray<FArrangedWidget>& ArrangedChildren) const;

	/**
	 * 모든 자식 위젯들의 Geometry를 계산하여, 그 결과를 ArrangedChildren 리스트에 추가합니다.
	 * 각 레이아웃 패널 유형은 원하는 동작에 따라 자식 위젯들을 배치해야 합니다.
	 *
	 * @param AllottedGeometry    부모 위젯이 이 위젯에 할당한 Geometry (크기, 위치 등).
	 * @param ArrangedChildren    계산된 자식 위젯들의 WidgetGeometry를 추가할 배열.
	 */
	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, TArray<FArrangedWidget>& ArrangedChildren) const = 0;

    static void ArrangeSingleChild(const FGeometry& AllottedGeometry, TArray<FArrangedWidget>& ArrangedChildren, std::shared_ptr<SWidget> InChild, const
                                   FVector2D& ContentScale);
	
	bool IsMouseCaptured() const { return bIsMouseCaptured; }

	// 자식 위젯이 포인터 아래에 있는지(호버 상태)와 캡처 상태인지를 확인하는 헬퍼 함수
    static bool IsChildActive(const std::shared_ptr<SWidget>& Child, const FPointer& InPointer, const FGeometry& ChildGeometry);

protected:
	// 위젯의 로컬 크기 (자기 자신의 크기)
	FVector2D LocalSize;
	// 위젯의 로컬 위치 (부모 위젯 내 오프셋)
	FVector2D LocalPosition;
	// 로컬 변환 정보 (주로 이동, 스케일 정보를 포함)
	FSlateLayoutTransform LayoutTransform;

	// 매 틱마다 계산된 절대 좌표, 스케일 등 정보를 담은 기하학 정보
	FGeometry MyGeometry;

	// 자식 위젯들
	std::vector<std::shared_ptr<SWidget>> Children;

	// 마우스 캡처 상태를 나타내는 플래그 (예: true면 이 위젯이 마우스를 캡처한 상태)
	bool bIsMouseCaptured = false;
public:
    void AssignParentWidget(const std::shared_ptr<SWidget>& InParent);

    bool IsParentValid() const { return ParentWidgetPtr.lock() != nullptr; }
    std::shared_ptr<SWidget> GetParentWidget() const { return ParentWidgetPtr.lock(); }
	
    virtual float GetRelativeLayoutScale(const int32 ChildIndex, float LayoutScaleMultiplier) const;

    virtual TArray<std::shared_ptr<SWidget>> GetChildren() { return ChildrenWidgetPtr; }

    bool HasFocusedDescendants(const FPointer& InPointer) const;

    inline EVisibility GetVisibility() const { return VisibilityAttribute; }

	inline virtual void SetVisibility(EVisibility InVisibility) const {	VisibilityAttribute = InVisibility; }

    bool IsAccessible() const;

    bool CanChildrenBeAccessible() const;

    void SetCanChildrenBeAccessible(bool bInCanChildrenBeAccessible);

    /** @return 현재 마우스가 가리키고 있는 자식 위젯의 인덱스 */
    static int32 FindChildUnderMouse(const TArray<FArrangedWidget>& Children, const FPointer& InPointer);

    /** @return 지정된 위치 아래에 있는 자식 위젯의 인덱스 */
    static int32 FindChildUnderPosition(const TArray<FArrangedWidget>& Children, const FVector2D& ArrangedSpacePosition);

    inline bool ShouldBeEnabled(const bool InParentEnabled) const
    {
        // 부모가 활성화되어 있고, 이 위젯도 활성화되어 있으면 true 반환
        return InParentEnabled && IsEnabled();
    }

    static EVisibility AccessWidgetVisibility(const std::shared_ptr<SWidget>& Widget)
    {
        return Widget->VisibilityAttribute;
    }
	
public:
    inline void SetEnabled(const bool InEnabled) { EnabledAttribute = InEnabled; }

    inline bool IsEnabled() const { return EnabledAttribute; }

    inline virtual bool IsInteractable() const { return false; }

    inline bool GetEnabledAttribute() const { return &EnabledAttribute; }

    inline bool GetHoveredAttribute() const { return &HoveredAttribute; }

    inline bool IsHovered() const { return HoveredAttribute; }

    inline void SetHover(const bool InHovered) {  bIsHoveredAttributeSet = InHovered; }

    inline EVisibility* GetVisibilityAttribute() const { return &VisibilityAttribute; }
private:
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

    mutable EVisibility VisibilityAttribute;
    bool EnabledAttribute;
    bool HoveredAttribute;

    /** 이 위젯의 부모 위젯에 대한 포인터. null이면 루트 위젯이거나 위젯 트리에 포함되어 있지 않음을 의미 */
    std::weak_ptr<SWidget> ParentWidgetPtr;

    TArray<std::shared_ptr<SWidget>> ChildrenWidgetPtr;
};
