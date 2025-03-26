#pragma once
#include "SWidget.h"
#include "Core/Rendering/D3DViewports/Viewport.h"

struct FArrangedWidget;

class SViewport : public SWidget
{
	using Super = SWidget;
public:
	SViewport(FVector2D InViewportSize = FVector2D(0, 0));
	virtual ~SViewport();

	/**
	 * 뷰포트의 크기를 업데이트합니다.
	 * 보통 부모 위젯이나 창의 크기가 변경되었을 때 호출됩니다.
	 *
	 * @param NewSize   새로 적용할 뷰포트 크기.
	 */
	void UpdateViewportSize(const FVector2D& NewSize);

	/**
	 * 뷰포트에 연결된 렌더 타깃에 콘텐츠를 렌더링합니다.
	 */
	void RenderViewport();

	/**
	* Checks to see if this widget is the current mouse captor
	*
	* @return  True if this widget has captured the mouse
	*/
	bool HasMouseCapture() const;

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

	/** @return 이 뷰포트가 백버퍼에 직접 렌더링하는지 여부를 반환합니다. */
	bool ShouldRenderDirectly() const { return bRenderDirectlyToWindow; }

	/**
	* 이 뷰포트가 백버퍼에 직접 렌더링할 수 있는지 설정합니다. (고급 사용용)
	*
	* @param bInRenderDirectlyToWindow 백버퍼에 직접 렌더링할 수 있는지 여부
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
	virtual void Tick(const FGeometry& AllottedGeometry, const float InDeltaTime) override;
	virtual FCursorReply OnCursorQuery(const FPointer& InPointer) const override;
	virtual std::optional<std::shared_ptr<SWidget>> OnMapCursor(const FCursorReply& CursorReply) const override;
	virtual FReply OnMouseButtonDown(EMouseButton InMouseButton, const FPointer& InPointer) override;
	virtual FReply OnMouseButtonUp(EMouseButton InMouseButton, const FPointer& InPointer) override;
	virtual void OnMouseEnter(const FPointer& InPointer) override;
	virtual void OnMouseLeave(const FPointer& InPointer) override;
	virtual FReply OnMouseMove(const FPointer& InPointer) override;
	virtual FReply OnMouseWheel(const FPointer& InPointer) override;
	virtual FReply OnMouseButtonDoubleClick(const FPointer& InPointer) override;
	virtual FReply OnKeyDown(const EKeyCode& InKeyCode) override;
	virtual FReply OnKeyUp(const EKeyCode& InKeyCode) override;
	virtual FReply OnKeyChar(const EKeyCode& InKeyCode) override;
	virtual FReply OnFocusReceived() override;
	virtual void OnFocusLost() override;
	virtual void OnFinishedPointerInput() override;
	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, TArray<FArrangedWidget>& ArrangedChildren) const override;
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
	using Super = SWidget;
public:
	SEditorViewport();
	virtual ~SEditorViewport(); 

	void Construct();

	virtual FReply OnKeyDown(const EKeyCode& InKeyCode) override;
	virtual FReply OnFocusReceived() override;
	virtual void Tick(const FGeometry& AllottedGeometry,  const float InDeltaTime) override;

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
	using Super = SEditorViewport;
public:
	void Tick(const FGeometry& ParentGeometry, float DeltaTime) override;

	FReply OnKeyDown(const EKeyCode& InKeyCode) override;
	FReply OnFocusReceived() override;
	void OnFocusLost() override;
	FReply OnMouseButtonDown(EMouseButton InMouseButton, const FPointer& InPointer) override;
	FReply OnMouseButtonUp(EMouseButton InMouseButton, const FPointer& InPointer) override;
	FReply OnMouseMove(const FPointer& InPointer) override;
	FReply OnMouseWheel(const FPointer& InPointer) override;

	void OnMouseEnter(const FPointer& InPointer) override;
	void OnMouseLeave(const FPointer& InPointer) override;
	
	SLevelViewport();
	~SLevelViewport();
};
