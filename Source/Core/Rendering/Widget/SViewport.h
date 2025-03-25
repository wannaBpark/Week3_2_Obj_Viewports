#pragma once
#include "SWidget.h"
#include "Core/Rendering/D3DViewports/Viewport.h"

struct FArrangedWidget;

class SViewport : public SWidget
{
public:
	SViewport();
	virtual ~SViewport();

	void Construct();

	/** SViewport은 키보드 포커스를 원합니다. */
	virtual bool SupportsKeyboardFocus() const override { return true; }

	/**
	* 이 위젯을 표시하기 위해 필요한 이상적인 크기를 계산합니다.
	*
	* @return 원하는 너비와 높이를 반환합니다.
	*/
	virtual FVector2D ComputeDesiredSize(float) const override
	{
		return ViewportSize;
	}

	/**
	* 이 뷰포트에서 렌더링 및 입출력을 위해 사용할 인터페이스를 설정합니다.
	*
	* @param InViewportInterface 사용할 인터페이스
	*/
	void SetViewportInterface(std::shared_ptr<ISlateViewport> InViewportInterface);

	/**
	 * 이 뷰포트에서 렌더링 및 입출력을 위해 사용할 인터페이스를 반환합니다.
	 *
	 * @return 설정된 인터페이스(weak pointer)
	 */
	std::weak_ptr<ISlateViewport> GetViewportInterface()
	{
		return ViewportInterface;
	}

	/**
	* 이 위젯의 콘텐츠를 설정합니다.
	*
	* @param InContent 새로운 콘텐츠 (null일 수 있음)
	*/
	void SetContent(std::shared_ptr<SWidget> InContent);

	/**
	* 뷰포트의 최상위 창이 닫힐 때 호출되는 델리게이트입니다.
	*
	* @param InWindowBeingClosed 닫히려는 창
	*/
	void OnWindowClosed(const std::shared_ptr<SWindow>& InWindowBeingClosed);

	// TODO: 델리게이트
	///**
	// * 뷰포트의 최상위 창이 활성화될 때 호출되는 델리게이트
	// */
	//FReply OnViewportActivated(const FWindowActivateEvent& InActivateEvent);

	///**
	// * 뷰포트의 최상위 창이 비활성화될 때 호출되는 델리게이트
	// */
	//void OnViewportDeactivated(const FWindowActivateEvent& InActivateEvent);

	/** @return 이 뷰포트가 백버퍼에 직접 렌더링하는지 여부를 반환합니다. */
	bool ShouldRenderDirectly() const { return bRenderDirectlyToWindow; }

	/**
	* 이 뷰포트가 백버퍼에 직접 렌더링할 수 있는지 설정합니다. (고급 사용용)
	*
	* @param	bInRenderDirectlyToWindow 백버퍼에 직접 렌더링할 수 있는지 여부
	*/
	void SetRenderDirectlyToWindow(const bool bInRenderDirectlyToWindow);

	/**
	* 이 뷰포트를 활성 상태로 설정합니다.
	* 활성 상태일 때는, 지속적인 Active Timer가 등록되며 매 프레임 Slate 틱/페인트가 보장됩니다.
	* @param bActive 뷰포트를 활성 상태로 설정할지 여부
	*/
	void SetActive(bool bActive);

public:

	// SWidget 인터페이스
	// TODO: OnPaint
	//virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, bool bParentEnabled) const override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual FCursorReply OnCursorQuery(const FGeometry& MyGeometry, const FPointer& InPointer/*, const FPointerEvent& CursorEvent*/) const override;
	virtual std::optional<std::shared_ptr<SWidget>> OnMapCursor(const FCursorReply& CursorReply) const override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointer& InPointer/*, const FPointerEvent& MouseEvent*/) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointer& InPointer/*, const FPointerEvent& MouseEvent*/) override;
	virtual void OnMouseEnter(const FGeometry& MyGeometr, const FPointer& InPointery/*, const FPointerEvent& MouseEvent*/) override;
	virtual void OnMouseLeave(const FPointer& InPointer/*const FPointerEvent& MouseEvent*/) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointer& InPointer/*, const FPointerEvent& MouseEvent*/) override;
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointer& InPointer/*, const FPointerEvent& MouseEvent*/) override;
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointer& InPointer/*, const FPointerEvent& MouseEvent*/) override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const EKeyCode& InKeyCode/*, const FKeyEvent& KeyEvent*/) override;
	virtual FReply OnKeyUp(const FGeometry& MyGeometry, const EKeyCode& InKeyCode/*, const FKeyEvent& KeyEvent*/) override;
	virtual FReply OnKeyChar(const FGeometry& MyGeometry, const EKeyCode& InKeyCode/*, const FCharacterEvent& CharacterEvent*/) override;
	virtual FReply OnFocusReceived(const FGeometry& MyGeometry/*, const FFocusEvent& InFocusEvent*/) override;
	virtual void OnFocusLost(/*const FFocusEvent& InFocusEvent*/) override;
	virtual void OnFinishedPointerInput() override;
	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, TArray<FArrangedWidget>& ArrangedChildren) const override;
	//virtual std::optional<FVirtualPointerPosition> TranslateMouseCoordinateForCustomHitTestChild(const SWidget& ChildWidget, const FGeometry& MyGeometry, const FVector2D ScreenSpaceMouseCoordinate, const FVector2D LastScreenSpaceMouseCoordinate) const override;

protected:
	/** 뷰포트의 렌더링 및 입출력 구현 인터페이스. */
	std::weak_ptr<ISlateViewport> ViewportInterface;

private:
	std::weak_ptr<SWindow> CachedParentWindow;

	FVector2D ViewportSize;

	/** 이 뷰포트가 창의 백버퍼에 직접 렌더링하는지 여부. */
	bool bRenderDirectlyToWindow;
};


class SEditorViewport : public SWidget
{
public:
	SEditorViewport();
	virtual ~SEditorViewport(); 

	void Construct();

	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const EKeyCode& InKeyCode/*, const FKeyEvent& InKeyEvent*/) override;
	virtual bool SupportsKeyboardFocus() const override;
	virtual FReply OnFocusReceived(const FGeometry& MyGeometry/*, const FFocusEvent& InFocusEvent*/) override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	/**
	* @return 뷰포트가 실시간으로 업데이트되고 있다면 true를 반환합니다.
	*/
	bool IsRealtime() const;

	/** @return 뷰포트가 현재 보이고 있다면 true를 반환합니다. */
	virtual bool IsVisible() const;

	/**
	 * 이 뷰포트가 백버퍼에 직접 렌더링할 수 있는지 여부를 설정합니다. (고급 사용용)
	 *
	 * @param	bInRenderDirectlyToWindow	백버퍼에 직접 렌더링할 수 있는지 여부
	 */
	void SetRenderDirectlyToWindow(const bool bInRenderDirectlyToWindow);

	/** @return 뷰포트 커맨드 리스트를 반환합니다. */
	//const std::shared_ptr<FUICommandList> GetCommandList() const { return CommandList; }

	std::shared_ptr<FEditorViewportClient> GetViewportClient() const { return Client; }

	/**
	* @return 현재의 FSceneViewport shared pointer를 반환합니다.
	*/
	std::shared_ptr<FSceneViewport> GetSceneViewport() { return SceneViewport; }

	/**
	* @return translate/rotate 모드 위젯이 보이면 true를 반환합니다.
	*/
	virtual bool IsTranslateRotateModeVisible() const;

	/** 이 뷰포트가 속한 월드를 반환합니다. */
	virtual UWorld* GetWorld() const;

protected:
	virtual std::shared_ptr<FEditorViewportClient> MakeEditorViewportClient() = 0;

	virtual void OnIncrementPositionGridSize() {};
	virtual void OnDecrementPositionGridSize() {};
	virtual void OnIncrementRotationGridSize() {};
	virtual void OnDecrementRotationGridSize() {};

	// TODO: 여기서 Command를 바인드해주는 듯?
	//virtual void BindCommands();
protected:
	/** 뷰포트 클라이언트가 제공하는 씬을 렌더링하는 뷰포트 */
	std::shared_ptr<FSceneViewport> SceneViewport;

	/** 씬 뷰포트가 그려지는 위젯 */
	std::shared_ptr<SViewport> ViewportWidget;

	/** 씬 설정을 담당하는 클라이언트 */
	std::shared_ptr<FEditorViewportClient> Client;

	// TODO: COMMAND
	/** 뷰포트에서 사용되는 커맨드 리스트 (뷰포트 전용 동작과 커맨드를 매핑) */
	//std::shared_ptr<FUICommandList> CommandList;
};

class SLevelViewport : public SEditorViewport
{
public:
	SLevelViewport();
	~SLevelViewport();
}; 