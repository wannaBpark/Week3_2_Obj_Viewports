#pragma once
#include "Core/Rendering/Widget/SWidget.h"
#include "Core/Math/Vector.h"
#include "Core/Container/Array.h"
#include "Core/Rendering/D3DViewports/Viewport.h"
#include "Core/Rendering/Widget/Margin.h"

class SWindow : public SWidget
{
    using Super = SWidget;
public:
    SWindow(const FVector2D& InSize);
    virtual ~SWindow();

    /**
     * 창의 기하학(Geometry)을 업데이트합니다.
     * 예를 들어, 창 크기 변경, 위치 이동 등이 발생했을 때 호출됩니다.
     *
     * @param NewTransform   창에 적용할 새로운 레이아웃 변환 (스케일, 위치 등)
     */
    void UpdateWindowGeometry(const FSlateLayoutTransform& NewTransform);

    /**
     * 루트 위젯을 기준으로 자식 위젯들의 배치를 재계산합니다.
     * ArrangeChildren 함수를 호출하여 자식 위젯들의 Geometry를 갱신합니다.
     */
    void ArrangeWindow() const;

    /**
     * 루트 위젯을 설정합니다.
     */
    void SetRootWidget(const std::shared_ptr<SWidget>& InRootWidget);
protected:
    // 창의 루트 위젯 (UI 트리의 최상위 컨테이너)
    std::shared_ptr<SWidget> RootWidget;
public:
    /** @return 슬레이트 창의 초기 원하는 스크린 위치를 반환합니다. */
    FVector2D GetInitialDesiredSizeInScreen() const;

    /** @return 슬레이트 창의 초기 원하는 크기를 반환합니다. */
    FVector2D GetInitialDesiredPositionInScreen() const;

    /** 창을 설명하는 기하학 정보를 반환합니다. Slate의 창은 자신만의 기하학 정보를 알고 있습니다. */
    FGeometry GetWindowGeometryInScreen() const;

    /** @return 윈도우 공간(예: 위치와 AbsolutePosition이 0인 공간)에서의 창 기하학 정보를 반환합니다. */
    FGeometry GetWindowGeometryInWindow() const;

    /** @return 로컬 공간에서 스크린 공간(데스크탑 공간)으로의 변환 행렬을 반환합니다. */
    FSlateLayoutTransform GetLocalToScreenTransform() const;

    /** @return 로컬 공간에서 윈도우 공간으로의 변환 행렬을 반환합니다. 기본적으로 오프셋이 없는 데스크탑 공간이며, DPI 스케일을 포함합니다. */
    FSlateLayoutTransform GetLocalToWindowTransform() const;

    /** @return 스크린 공간에서의 창 위치를 반환합니다. */
    FVector2D GetPositionInScreen() const;

    /** @return 스크린 픽셀 단위의 창 크기를 반환합니다. */
    FVector2D GetSizeInScreen() const;

    /** @return 비최대화 상태에서 창의 사각형을 반환합니다. */
    FSlateRect GetNonMaximizedRectInScreen() const;

    /** @return 스크린 공간에서 창이 차지하는 사각형을 반환합니다. */
    FSlateRect GetRectInScreen() const;

    /** @return 스크린 공간에서 창의 사용 가능한 클라이언트 영역의 사각형을 반환합니다. */
    FSlateRect GetClientRectInScreen() const;

    /** @return 스크린 픽셀 단위의 창 사용 영역 크기를 반환합니다. */
    FVector2D GetClientSizeInScreen() const;

    /** @return 윈도우 공간(항상 0,0에서 시작)에서 이 창을 나타내는 클리핑 사각형을 반환합니다. */
    FSlateRect GetClippingRectangleInWindow() const;

    /** 창 테두리에 사용되는 여백을 반환합니다. 최대화 상태 여부에 따라 달라집니다. */
    FMargin GetWindowBorderSize(bool bIncTitleBar = false) const;

    /** 최대화되지 않은 경우의 창 테두리 여백을 반환합니다. */
    FMargin GetNonMaximizedWindowBorderSize() const;

    /** 새로운 위치(NewPosition)로 스크린 공간 상에서 창을 이동시킵니다. */
    void MoveWindowTo(FVector2D NewPosition);
    /** 새로운 위치(NewPosition)로 창을 이동시키고, 크기를 NewSize로 변경합니다. */
    void ReshapeWindow(FVector2D NewPosition, FVector2D NewSize);
    void ReshapeWindow(const FSlateRect& InNewShape);
    /**
     * DPI 스케일이 적용된 NewClientSize로 즉시 창의 클라이언트 영역 크기를 변경합니다.
     *
     * @param NewClientSize: DPI 스케일이 이미 적용되어 있으며, 테두리나 타이틀바를 포함하지 않는 클라이언트 크기.
     */
    void Resize(FVector2D NewClientSize);

    /** 실제 스크린 위치를 설정합니다. (이 함수는 OS에서만 호출되어야 합니다.) */
    void SetCachedScreenPosition(FVector2D NewPosition);

    /** 실제 창 크기를 설정합니다. (이 함수는 OS에서만 호출되어야 합니다.) */
    void SetCachedSize(FVector2D NewSize);

    /**
     * 이 창에 대한 위젯 콘텐츠를 설정합니다.
     *
     * @param InContent: 이 창의 콘텐츠로 사용할 위젯
     */
    void SetContent(std::shared_ptr<SWidget> InContent);
    
    /** 이 창의 파괴를 요청합니다. 창은 즉시 파괴되지 않고 다음 Tick에서 파괴 대기열에 들어갑니다. */
    void RequestDestroyWindow();

    /** @return 창이 입력을 받으면 true, 비대화형이면 false를 반환합니다. */
    bool AcceptsInput() const;

    /** @return 창의 구성된 최대 예상 너비를 반환합니다. 지정되지 않은 경우 INDEX_NONE 반환.
        창 크기 애니메이션 최적화를 위해 사용될 수 있습니다. */
    int32 GetExpectedMaxWidth() const
    {
        return ExpectedMaxWidth;
    }

    /** @return 창의 구성된 최대 예상 높이를 반환합니다. 지정되지 않은 경우 INDEX_NONE 반환.
        창 크기 애니메이션 최적화를 위해 사용될 수 있습니다. */
    int32 GetExpectedMaxHeight() const
    {
        return ExpectedMaxHeight;
    }

    // TODO
    ///** @return 마우스 좌표가 이 창 내부에 있는지 여부를 반환합니다. */
    bool IsScreenspaceMouseWithin(FVector2D ScreenspaceMouseCoordinate) const;

    ///** @return 이 창이 두꺼운 가장자리를 가진 사용자 크기 창인지 여부를 반환합니다. */
    //bool HasSizingFrame() const;

    ///** @return 이 창의 타이틀바 영역에 닫기 버튼/상자가 있는지 여부를 반환합니다. */
    //bool HasCloseBox() const;

    ///** @return 이 창의 타이틀바 영역에 최대화 버튼/상자가 있는지 여부를 반환합니다. */
    //bool HasMaximizeBox() const;

    ///** @return 이 창의 타이틀바 영역에 최소화 버튼/상자가 있는지 여부를 반환합니다. */
    //bool HasMinimizeBox() const;

    virtual FCursorReply OnCursorQuery(const FPointer& InPointer/*, const FPointerEvent& CursorEvent*/) const override;

private:
    virtual FReply OnFocusReceived() override;
    virtual FReply OnMouseButtonDown(EMouseButton InMouseButton,const FPointer& InPointer/*, const FPointerEvent& MouseEvent*/) override;
    virtual FReply OnMouseButtonUp(EMouseButton InMouseButton,const FPointer& InPointer/*, const FPointerEvent& MouseEvent*/) override;
    virtual FReply OnMouseMove(const FPointer& InPointer/*, const FPointerEvent& MouseEvent*/) override;
    virtual int32 OnPaint(/*const FPaintArgs& Args, */const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect/*, FSlateWindowElementList& OutDrawElements*/, int32 LayerId/*, const FWidgetStyle& InWidgetStyle*/, bool bParentEnabled) const;

    /** DPI 스케일이 적용된 테두리/타이틀바를 포함한 창 크기를 사용하여 창 크기를 조정합니다. */
    void ResizeWindowSize(FVector2D NewWindowSize);

public:
    /**
     * 주어진 클라이언트 크기에 대해, OS 이외의 테두리와 타이틀바를 수용하기 위한 창 크기를 계산합니다.
     *
     * @param InClientSize: DPI 스케일이 이미 적용된 클라이언트 크기
     */
    FVector2D GetWindowSizeFromClientSize(FVector2D InClientSize/*, std::optional<float> DPIScale = TOptional<float>()*/);

    /** @return 창이 처음 표시될 때 포커스를 받을지 여부를 반환합니다. */
    inline bool IsFocusedInitially() const
    {
        return bFocusWhenFirstShown;
    }
    /** @return 이 창의 자식 창 목록을 반환합니다. */
    const TArray<std::shared_ptr<SWindow>>& GetChildWindows() const;

    /** @return 이 창의 자식 창 목록을 반환합니다. */
    TArray<std::shared_ptr<SWindow>>& GetChildWindows();

    void AddChildWindow(const std::shared_ptr<SWindow>& ChildWindow);

    /** @return 이 창의 부모 창을 반환합니다. 자식이 아니면 유효하지 않은(shared pointer) 값을 반환합니다. */
    std::shared_ptr<SWindow> GetParentWindow() const;

    /** 소유하는 최상위 창(상위 체인을 따라 탐색하여 최상위 창)을 반환합니다. */
    std::shared_ptr<SWindow> GetTopmostAncestor();

    /** 이 창의 자식이나 그 자식들 중에서 DescendantToRemove를 제거합니다. */
    bool RemoveDescendantWindow(const std::shared_ptr<SWindow>& DescendantToRemove);

    /** 이 창이 활성화되어 있는지 여부를 반환합니다. */
    bool IsActive() const;

    /** 자식 창 중 활성화된 창이 있는지 여부를 반환합니다. */
    bool HasActiveChildren() const;

    /** 상위 창 중 활성화된 창이 있는지 여부를 반환합니다. */
    bool HasActiveParent() const;

    /**
     * 뷰포트 크기가 창의 크기에 의해 결정되어야 하는지 설정합니다.
     * true이면 두 값은 동일합니다. false인 경우, SetIndependentViewportSize를 통해 독립적으로 지정할 수 있습니다.
     */
    inline void SetViewportSizeDrivenByWindow(const bool bDrivenByWindow)
    {
        if (bDrivenByWindow)
        {
            ViewportSize = FVector2D::ZeroVector;
        }
        else
        {
            ViewportSize = Size;
        }
    }

    /**
     * 뷰포트와 창의 크기가 연동되어야 하는지 여부를 반환합니다.
     * false이면 두 값을 독립적으로 사용할 수 있습니다. (예: 미러 모드 창 그리기)
     */
    inline bool IsViewportSizeDrivenByWindow() const
    {
        return (ViewportSize.X == 0);
    }

    /**
     * 창의 크기가 뷰포트 크기에 영향을 미치는 경우를 고려하여 뷰포트 크기를 반환합니다.
     */
    inline FVector2D GetViewportSize() const
    {
        return (ViewportSize.X != 0) ? ViewportSize : Size;
    }

    /**
     * 뷰포트 크기를 창의 크기와 독립적으로 설정합니다. (0이 아니면 적용)
     */
    inline void SetIndependentViewportSize(const FVector2D& VP)
    {
        ViewportSize = VP;
    }

    void SetViewport(const std::shared_ptr<ISlateViewport>& ViewportRef)
    {
        Viewport = ViewportRef;
    }

    void UnsetViewport(const std::shared_ptr<ISlateViewport>& ViewportRef)
    {
        if (Viewport.lock() == ViewportRef)
        {
            Viewport.reset();
        }
    }

    std::shared_ptr<ISlateViewport> GetViewport() const
    {
        return Viewport.lock();
    }

public:
    virtual void Tick(const FGeometry& ParentGeometry, float DeltaTime) override;
protected:
    virtual void Render() = 0; // 스플리터 창을 렌더합니다

protected:
    std::optional<float> MinWidth;

    std::optional<float> MinHeight;

    std::optional<float> MaxWidth;

    std::optional<float> MaxHeight;

    std::weak_ptr<SWindow> ParentWindowPtr;

    TArray<std::shared_ptr<SWindow>> ChildWindowPtrs;

    /** 이 창에 타이틀바가 있으면 true입니다. */
    bool bCreateTitleBar = false;

    /** 팝업 창이면 true입니다. */
    bool bIsPopupWindow = false;

    /** 최상위 창이면 true입니다. */
    bool bIsTopmostWindow = false;

    /** 이 창의 크기가 자주 변경될 것으로 예상되면(true, 예: 애니메이션, 툴팁 재사용 등),
        GPU 버퍼 재할당 비용을 줄이기 위해 사용됩니다.
        이 경우 메모리 오버헤드나 플랫폼 별 부작용이 발생할 수 있습니다. */
    bool bSizeWillChangeOften = false;

    /** 창이 생성될 때 최대화된 상태이면 true입니다. */
    bool bInitiallyMaximized = false;

    /** 창이 생성될 때 최소화된 상태이면 true입니다. */
    bool bInitiallyMinimized = false;

    /** 창이 한 번이라도 표시되었으면 true입니다. */
    bool bHasEverBeenShown = false;

    /** 창이 표시될 때 즉시 포커스를 받을지 여부 */
    bool bFocusWhenFirstShown = false;

    /** 이 창이 슬레이트 대신 OS 창 테두리를 표시하면 true입니다. */
    bool bHasOSWindowBorder = false;

    /** 이 창이 가상 창(슬레이트 애플리케이션 또는 OS에서 직접 렌더링되지 않음)이면 true입니다. */
    bool bVirtualWindow = false;

    /** 툴바 영역에 닫기 버튼이 활성화되어 있으면 true입니다. */
    bool bHasCloseButton = false;

    /** 툴바 영역에 최소화 버튼이 활성화되어 있으면 true입니다. */
    bool bHasMinimizeButton = false;

    /** 툴바 영역에 최대화 버튼이 활성화되어 있으면 true입니다. */
    bool bHasMaximizeButton = false;

    /** 사용자가 창 크기를 조절할 수 있도록 두꺼운 가장자리가 있으면 true입니다. */
    bool bHasSizingFrame = true;

    /** 창이 모달 창이면 true입니다. */
    bool bIsModalWindow = false;

    /** 창이 HMD 콘텐츠를 위한 미러 창이면 true입니다. */
    bool bIsMirrorWindow = false;

    /** 사용자가 창 크기를 조절할 때 가로세로 비율을 유지해야 하면 true입니다. */
    bool bShouldPreserveAspectRatio = false;

    FVector2D InitialDesiredScreenPosition;

    /** 스크린 공간에서 창 콘텐츠의 초기 원하는 크기 */
    FVector2D InitialDesiredSize;

    /** 스크린 공간에서 창 콘텐츠의 위치 */
    FVector2D ScreenPosition;

    /** 전체 화면 모드로 전환되기 전의 창 위치 */
    FVector2D PreFullscreenPosition;

    /** 스크린 공간에서 창 콘텐츠 영역의 크기 */
    FVector2D Size;

    /** 뷰포트 크기. (0,0)일 경우 Size와 동일 */
    FVector2D ViewportSize;

    /** 이 창과 연동된 뷰포트에 대한 포인터 */
    std::weak_ptr<ISlateViewport> Viewport;

    /**
     * 종횡비, DPI, UI 확대/축소의 혼합 값.
     * 쉐이더/머티리얼에서 사용됩니다.
     * 자식 위젯에 의해 재정의될 수 있으며, 매 프레임 그린 후 초기화됩니다.
     * 음수 값이면 사용되지 않음을 의미하며, 이 경우 애플리케이션 수준 UI 스케일이 사용됩니다.
     */
    float ViewportScaleUIOverride = 0;

    /** 이 창의 타이틀바 크기. 0일 수 있으며, 생성 시 설정되고 이후 변경되지 않아야 합니다. */
    float TitleBarSize = 0;

    /** 이 창이 활성화될 때 키보드 포커스를 이전하기 위한 위젯(있을 경우). 팝업이 해제된 후 포커스를 복원하는 데 사용됩니다. */
    std::weak_ptr<SWidget> WidgetToFocusOnActivate;

    /** 창이 마지막으로 비활성화되었을 때 포커스를 가지고 있던 위젯(있을 경우). 창이 다시 활성화될 때 포커스를 복원하는 데 사용됩니다. */
    std::weak_ptr<SWidget> WidgetFocusedOnDeactivate;

    /** 전체 창 오버레이 위젯 */
    std::shared_ptr<SWidget> FullWindowOverlayWidget;

    /**
     * 전체 화면 오버레이로 인해 가려질 수 있는 창 콘텐츠를 표시할지 여부.
     * 전체 화면 오버레이가 있으면 콘텐츠를 숨기기 위해 사용됩니다.
     */
    bool bShouldShowWindowContentDuringOverlay = false;

    /** 창의 예상 최대 너비. bSizeWillChangeOften이 설정된 경우 성능 최적화를 위해 사용됩니다. */
    int32 ExpectedMaxWidth = 0;

    /** 창의 예상 최대 높이. bSizeWillChangeOften이 설정된 경우 성능 최적화를 위해 사용됩니다. */
    int32 ExpectedMaxHeight = 0;

    // 창의 가장자리와 콘텐츠 사이의 패딩
    FMargin LayoutBorder = FMargin(5, 5, 5, 5);

    // 사용자가 창 크기를 조절할 수 있도록 창 가장자리 주변의 여백
    FMargin UserResizeBorder = FMargin(3, 3, 3, 3);

    // 이 창에 대해 그리기가 활성화되어 있는지 여부
    bool bIsDrawingEnabled = true;

    FVector2D MoveResizeStart;

    FSlateRect MoveResizeRect;
};
