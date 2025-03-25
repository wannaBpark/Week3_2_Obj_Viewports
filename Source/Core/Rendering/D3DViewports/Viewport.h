#pragma once
#include "memory"
#include "optional"
#include "Core/EngineTypes.h"
#include "RenderTarget.h"
#include "Core/Math/IntPoint.h"
#include <Core/Input/PlayerInput.h>
#include "Core/Math/Geometry.h"
#include "Core/Rendering/Widget/Reply.h"

struct FIntPoint;
struct FCursorReply;
struct FSlateRect;
struct FVector2D;
class SWidget;
class SEditorViewport;

//struct FCharacterEvent;
//struct FFocusEvent;
//struct FWindowActivateEvent;
//struct FPointerEvent;
//struct FKeyEvent;

class ISlateViewport
{
public:
	virtual ~ISlateViewport() {}
	/**
	 * 뷰포트 위젯이 그려질 때 Slate에 의해 호출됩니다.
	 * 이 인터페이스를 구현하는 객체는 사용자 정의 드로잉 기능을 수행할 수 있습니다.
	 * 이 메서드는 위젯이 보이는 경우에만 호출됩니다.
	 *
	 * @param AllottedGeometry	뷰포트 위젯의 기하학 정보
	 */
	virtual void OnDrawViewport(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect/*, class FSlateWindowElementList& OutDrawElements*/, int32 LayerId, bool bParentEnabled) {}

	/**
	 * 뷰포트의 크기를 반환합니다.
	 */
	virtual FIntPoint GetSize() const = 0;

	/**
	 * GetViewportRenderTargetTexture가 반환하는 텍스처가 알파 채널만 포함하는지 여부를 반환합니다.
	 */
	virtual bool IsViewportTextureAlphaOnly() const
	{
		return false;
	}

	/**
	 * 뷰포트가 GetViewportRenderTargetTexture 대신 스왑체인으로 렌더링할 것으로 예상되는지 여부를 반환합니다.
	 */
	virtual bool UseSeparateRenderTarget() const
	{
		return false;
	}

	/**
	 * 이 핸들이 수행해야 하는 틱 작업을 처리합니다.
	 */
	virtual void Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float DeltaTime)
	{
	}

	/**
	 * 뷰포트가 Vsync를 요구하는 경우 true를 반환합니다.
	 */
	virtual bool RequiresVsync() const { return true; }

	/**
	 * 뷰포트 내용이 스케일되어야 하는지 여부를 반환합니다. 기본값은 true입니다.
	 */
	virtual bool AllowScaling() const
	{
		return true;
	}

	/**
	 * Slate가 마우스 커서가 어떤 형태여야 하는지 결정하기 위해 호출됩니다.
	 *
	 * @return 이벤트가 처리되지 않으면 FCursorReply::Unhandled()를, 처리되었다면 FCursorReply::Cursor()를 반환합니다.
	 */
	virtual FCursorReply OnCursorQuery(const FGeometry& MyGeometry/*, const FPointerEvent& CursorEvent*/) = 0;

	/**
	 * OnCursorQuery에서 지정한 커서를 위젯에 매핑하기 위해, 시스템이 마우스 아래의 각 위젯에 대해 호출합니다.
	 * 이 이벤트는 버블링됩니다.
	 *
	 * @return 매핑이 없으면 TOptional<TSharedRef<SWidget>>()를, 매핑이 있으면 해당 위젯을 반환합니다.
	 */
	virtual std::optional<std::shared_ptr<SWidget>> OnMapCursor(const FCursorReply& CursorReply)
	{
		return std::optional<std::shared_ptr<SWidget>>();
	}

	/**
	 * 소프트웨어 커서가 현재 보이는지 여부를 반환합니다.
	 */
	virtual bool IsSoftwareCursorVisible() const
	{
		return false;
	}

	/**
	 * 현재 소프트웨어 커서의 위치를 반환합니다.
	 */
	virtual FVector2D GetSoftwareCursorPosition() const = 0;

	/**
	 * 마우스 버튼이 뷰포트 내부에서 눌렸을 때 호출됩니다.
	 *
	 * @param MyGeometry	뷰포트의 위치와 크기에 대한 정보
	 * @param MouseEvent	마우스 이벤트에 대한 정보
	 *
	 * @return 이벤트가 처리되었는지와 시스템이 취할 행동 요청을 포함하여 반환합니다.
	 */
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry/*, const FPointerEvent& MouseEvent*/) = 0;

	/**
	 * 마우스 버튼이 뷰포트 내부에서 떼어졌을 때 호출됩니다.
	 *
	 * @param MyGeometry	뷰포트의 위치와 크기에 대한 정보
	 * @param MouseEvent	마우스 이벤트에 대한 정보
	 *
	 * @return 이벤트가 처리되었는지와 시스템이 취할 행동 요청을 포함하여 반환합니다.
	 */
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry/*, const FPointerEvent& MouseEvent*/) = 0;

	/**
	 * 마우스가 뷰포트 내부로 들어올 때 호출됩니다.
	 *
	 * @param MyGeometry	뷰포트의 위치와 크기에 대한 정보
	 */
	virtual void OnMouseEnter(const FGeometry& MyGeometry/*, const FPointerEvent& MouseEvent*/) = 0;

	/**
	 * 마우스가 뷰포트에서 나갈 때 호출됩니다.
	 */
	virtual void OnMouseLeave(/*const FPointerEvent& MouseEvent*/) = 0;

	/**
	 * 마우스가 뷰포트 내부에서 움직일 때 호출됩니다.
	 *
	 * @param MyGeometry	뷰포트의 위치와 크기에 대한 정보
	 * @param MouseEvent	마우스 이벤트에 대한 정보
	 *
	 * @return 이벤트가 처리되었는지와 시스템이 취할 행동 요청을 포함하여 반환합니다.
	 */
	virtual FReply OnMouseMove(const FGeometry& MyGeometry/*, const FPointerEvent& MouseEvent*/) = 0;

	/**
	 * 뷰포트 내부에서 마우스 휠이 사용될 때 호출됩니다.
	 *
	 * @param MyGeometry	뷰포트의 위치와 크기에 대한 정보
	 * @param MouseEvent	마우스 이벤트에 대한 정보
	 *
	 * @return 이벤트가 처리되었는지와 시스템이 취할 행동 요청을 포함하여 반환합니다.
	 */
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry/*, const FPointerEvent& MouseEvent*/) = 0;

	/**
	 * 뷰포트 내부에서 마우스 버튼 더블클릭이 발생할 때 호출됩니다.
	 *
	 * @param InMyGeometry	뷰포트의 위치와 크기에 대한 정보
	 * @param InMouseEvent	마우스 이벤트에 대한 정보
	 *
	 * @return 이벤트가 처리되었는지와 시스템이 취할 행동 요청을 포함하여 반환합니다.
	 */
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry/*, const FPointerEvent& InMouseEvent*/) = 0;

	/**
	 * 뷰포트 내부에서 키가 눌렸을 때 호출됩니다.
	 *
	 * @param MyGeometry	뷰포트의 위치와 크기에 대한 정보
	 * @param InKeyEvent	키 이벤트에 대한 정보
	 *
	 * @return 이벤트가 처리되었는지와 시스템이 취할 행동 요청을 포함하여 반환합니다.
	 */
	virtual FReply OnKeyDown(const FGeometry& MyGeometry/*, const FKeyEvent& InKeyEvent*/) = 0;

	/**
	 * 뷰포트 내부에서 키가 떼어졌을 때 호출됩니다.
	 *
	 * @param MyGeometry	뷰포트의 위치와 크기에 대한 정보
	 * @param InKeyEvent	키 이벤트에 대한 정보
	 *
	 * @return 이벤트가 처리되었는지와 시스템이 취할 행동 요청을 포함하여 반환합니다.
	 */
	virtual FReply OnKeyUp(const FGeometry& MyGeometry/*, const FKeyEvent& InKeyEvent*/) = 0;

	/**
	 * 뷰포트가 포커스를 가진 상태에서 문자 키가 눌렸을 때 호출됩니다.
	 *
	 * @param MyGeometry	뷰포트의 위치와 크기에 대한 정보
	 * @param InCharacterEvent	눌린 문자에 대한 정보
	 *
	 * @return 이벤트가 처리되었는지와 시스템이 취할 행동 요청을 포함하여 반환합니다.
	 */
	virtual FReply OnKeyChar(const FGeometry& MyGeometry/*, const FCharacterEvent& InCharacterEvent*/) = 0;

	/**
	 * 뷰포트가 키보드 포커스를 얻었을 때 호출됩니다.
	 *
	 * @param InFocusEvent	포커스를 얻은 원인에 대한 정보
	 */
	virtual FReply OnFocusReceived(/*const FFocusEvent& InFocusEvent*/) = 0;

	/**
	 * 이 프레임의 모든 입력 처리가 완료된 후 호출됩니다.
	 */
	virtual void OnFinishedPointerInput()
	{
	}

	/**
	 * 뷰포트가 키보드 포커스를 잃었을 때 호출됩니다.
	 *
	 * @param InFocusEvent	포커스를 잃은 원인에 대한 정보
	 */
	virtual void OnFocusLost(/*const FFocusEvent& InFocusEvent*/) = 0;

	/**
	 * 뷰포트와 연결된 최상위 창이 닫히도록 요청되었을 때 호출됩니다.
	 * 이 시점에서 뷰포트는 아직 닫히지 않았으며, 작업이 취소될 수 있습니다.
	 * 이 메서드는 PIE, 에디터 창, 콘솔 또는 게임 종료 전의 다른 상황에서는 호출되지 않을 수 있습니다.
	 * 이는 플랫폼 별 창이 닫힐 때만 호출됩니다.
	 *
	 * @return 창 닫기 이벤트가 처리되어 창이 열려 있어야 한다면 FReply::Handled()를 반환합니다.
	 */
	virtual FReply OnRequestWindowClose() = 0;

	/**
	 * 뷰포트가 닫히도록 요청되었을 때 호출됩니다.
	 */
	virtual void OnViewportClosed() = 0;

	/**
	 * 이 뷰포트와 연결된 SWidget을 반환합니다.
	 */
	virtual std::weak_ptr<SWidget> GetWidget() = 0;

	// TODO: 이벤트
	///**
	// * 뷰포트의 최상위 창이 활성화될 때 호출됩니다.
	// */
	virtual FReply OnViewportActivated(/*const FWindowActivateEvent& InActivateEvent*/) = 0;

	///**
	// * 뷰포트의 최상위 창이 비활성화될 때 호출됩니다.
	// */
	virtual void OnViewportDeactivated(/*const FWindowActivateEvent& InActivateEvent*/) = 0;
};


struct FViewportCameraTransform
{
public:
	FViewportCameraTransform();

	/** Sets the transform's location */
	void SetLocation(const FVector& Position);

	/** Sets the transform's rotation */
	void SetRotation(const FQuat& Rotation)
	{
		ViewRotation = Rotation;
	}

	/** Sets the location to look at during orbit */
	void SetLookAt(const FVector& InLookAt)
	{
		LookAt = InLookAt;
	}

	/** Set the ortho zoom amount */
	void SetOrthoZoom(float InOrthoZoom)
	{
		OrthoZoom = InOrthoZoom;
	}

	/** Check if transition curve is playing. */
	bool IsPlaying();

	/** @return The transform's location */
	FORCEINLINE const FVector& GetLocation() const { return ViewLocation; }

	/** @return The transform's rotation */
	FORCEINLINE const FQuat& GetRotation() const { return ViewRotation; }

	/** @return The look at point for orbiting */
	FORCEINLINE const FVector& GetLookAt() const { return LookAt; }

	/** @return The ortho zoom amount */
	FORCEINLINE float GetOrthoZoom() const { return OrthoZoom; }

	/**
	 * Animates from the current location to the desired location
	 *
	 * @param InDesiredLocation	The location to transition to
	 * @param bInstant			If the desired location should be set instantly rather than transitioned to over time
	 */
	void TransitionToLocation(const FVector& InDesiredLocation, std::weak_ptr<SWidget> EditorViewportWidget, bool bInstant);

	/**
	 * Updates any current location transitions
	 *
	 * @return true if there is currently a transition
	 */
	bool UpdateTransition();

	/**
	 * Computes a matrix to use for viewport location and rotation when orbiting
	 */
	//FMatrix ComputeOrbitMatrix() const;

private:
	/** The time when a transition to the desired location began */
	//double TransitionStartTime;

	/** Current viewport Position. */
	FVector	ViewLocation;
	/** Current Viewport orientation; valid only for perspective projections. */
	FQuat ViewRotation;
	/** Desired viewport location when animating between two locations */
	FVector	DesiredLocation;
	/** When orbiting, the point we are looking at */
	FVector LookAt;
	/** Viewport start location when animating to another location */
	FVector StartLocation;
	/** Ortho zoom amount */
	float OrthoZoom;
};

class FViewportFrame
{
public:

	virtual class FViewport* GetViewport() = 0;
	virtual void ResizeFrame(uint32 NewSizeX, uint32 NewSizeY /*,EWindowMode::Type NewWindowMode*/) = 0;
};


class FViewportClient
{
public:
	virtual ~FViewportClient() {}
	// 미리 데이터를 로드합니다.
	virtual void Precache() {}
	// 뷰포트의 리드로 요청 시 호출됩니다.
	virtual void RedrawRequested(FViewport* Viewport);
	// 히트 프록시 무효화를 요청합니다.
	virtual void RequestInvalidateHitProxy(FViewport* Viewport);
	// 뷰포트를 그립니다. (Canvas는 주석 처리되어 있음)
	virtual void Draw(FViewport* Viewport/*, FCanvas* Canvas*/) {}
	// 스크린샷 처리를 수행합니다. 사용하지 않으면 false를 반환합니다.
	virtual bool ProcessScreenShots(FViewport* Viewport) { return false; }
	// 이 뷰포트 클라이언트와 연관된 월드를 반환합니다.
	virtual class UWorld* GetWorld() const { return nullptr; }
	// 엔진의 Show Flags를 반환합니다.
	virtual struct FEngineShowFlags* GetEngineShowFlags() { return nullptr; }

	/**
	 * 뷰포트에서 수신한 키 이벤트를 확인합니다.
	 * 만약 뷰포트 클라이언트가 이벤트를 사용한다면, 이벤트를 소모하기 위해 true를 반환해야 합니다.
	 * @param	EventArgs - 입력 이벤트 인자.
	 * @return	키 이벤트를 소모하면 true, 그렇지 않으면 false를 반환합니다.
	 */
	virtual bool InputKey(/*const FInputKeyEventArgs& EventArgs*/);

	/**
	 * 뷰포트에서 수신한 축(axis) 움직임을 확인합니다.
	 * 만약 뷰포트 클라이언트가 해당 움직임을 사용한다면, 이벤트를 소모하기 위해 true를 반환해야 합니다.
	 * @param	Viewport - 축 움직임이 발생한 뷰포트.
	 * @param	InputDevice - 이 축 움직임을 유발한 입력 장치.
	 * @param	Key - 움직인 축의 이름.
	 * @param	Delta - 축 움직임의 변화량.
	 * @param	DeltaTime - 마지막 축 업데이트 이후 경과 시간.
	 * @param	NumSamples - 이 Delta에 기여한 장치 샘플 수 (스무딩 등에 유용).
	 * @param	bGamepad - 게임패드(예: Xbox 컨트롤러)에서 입력이 발생했는지 여부.
	 * @return	축 움직임을 소모하면 true, 그렇지 않으면 false를 반환합니다.
	 */
	virtual bool InputAxis(FViewport* Viewport/*, FKey Key*/, float Delta, float DeltaTime, int32 NumSamples = 1, bool bGamepad = false) { return false; }

	/**
	 * 뷰포트에서 수신한 문자 입력을 확인합니다.
	 * 만약 뷰포트 클라이언트가 해당 문자를 사용한다면, 이벤트를 소모하기 위해 true를 반환해야 합니다.
	 * @param	Viewport - 문자 입력이 발생한 뷰포트.
	 * @param	ControllerId - 입력이 발생한 컨트롤러 ID.
	 * @param	Character - 입력된 문자.
	 * @return	문자 입력을 소모하면 true, 그렇지 않으면 false를 반환합니다.
	 */
	virtual bool InputChar(FViewport* Viewport, int32 ControllerId, TCHAR Character) { return false; }

	// 에디터 뷰포트 시뮬레이션 여부를 설정합니다.
	virtual void SetIsSimulateInEditorViewport(bool bInIsSimulateInEditorViewport) = 0;

	// 마우스 움직임 폴링을 원하면 true를 반환합니다.
	virtual bool WantsPollingMouseMovement(void) const { return true; }

	// 뷰포트 내에서 마우스가 들어올 때 호출됩니다.
	virtual void MouseEnter(FViewport* Viewport, int32 x, int32 y) = 0;

	// 뷰포트 내에서 마우스가 나갈 때 호출됩니다.
	virtual void MouseLeave(FViewport* Viewport) = 0;

	// 뷰포트 내에서 마우스가 움직일 때 호출됩니다.
	virtual void MouseMove(FViewport* Viewport, int32 X, int32 Y) = 0;

	/**
	 * 창 입력 캡처가 적용된 상태에서 마우스가 움직일 때 호출됩니다.
	 *
	 * @param	InViewport	마우스 입력을 캡처한 뷰포트
	 * @param	InMouseX	새로운 마우스 커서 X 좌표
	 * @param	InMouseY	새로운 마우스 커서 Y 좌표
	 */
	virtual void CapturedMouseMove(FViewport* InViewport, int32 InMouseX, int32 InMouseY) = 0;

	/**
	 * Slate에서 이 프레임의 입력이 완료되었을 때, 대기 중인 마우스 움직임을 처리하도록 호출됩니다.
	 */
	virtual void ProcessAccumulatedPointerInput(FViewport* InViewport) = 0;

	/**
	 * OS가 표시해야 하는 커서를 반환합니다.
	 *
	 * @param	Viewport	커서를 포함하는 뷰포트
	 * @param	X			커서의 X 위치
	 * @param	Y			커서의 Y 위치
	 *
	 * @return	OS가 표시해야 하는 커서를 반환합니다.
	 */
	virtual EMouseCursor GetCursor(FViewport* Viewport, int32 X, int32 Y) { return EMouseCursor::Default; }

	/**
	 * 커서 응답을 실제로 렌더링할 위젯으로 매핑하기 위해 호출됩니다.
	 *
	 * @return 매핑된 위젯이 없으면 TOptional<TSharedRef<SWidget>>()를, 있으면 해당 위젯을 반환합니다.
	 */
	virtual std::optional<std::shared_ptr<SWidget>> MapCursor(FViewport* Viewport, const FCursorReply& CursorReply);

	// 뷰포트가 포커스를 잃었을 때 호출됩니다.
	virtual void LostFocus(FViewport* Viewport) = 0;
	// 뷰포트가 포커스를 받았을 때 호출됩니다.
	virtual void ReceivedFocus(FViewport* Viewport) = 0;
	// 뷰포트가 포커스를 가지고 있는지 여부를 반환합니다.
	virtual bool IsFocused(FViewport* Viewport) { return true; }

	// 뷰포트가 활성화되었을 때 호출됩니다.
	virtual void Activated(FViewport* Viewport/*, const FWindowActivateEvent& InActivateEvent*/) = 0;
	// 뷰포트가 비활성화되었을 때 호출됩니다.
	virtual void Deactivated(FViewport* Viewport/*, const FWindowActivateEvent& InActivateEvent*/) = 0;

	/**
	 * 뷰포트와 연관된 최상위 창이 닫히도록 요청되었을 때 호출됩니다.
	 * 이 시점에서 뷰포트는 아직 닫히지 않았으며, 작업이 취소될 수 있습니다.
	 * 이 메서드는 PIE, 에디터 창, 콘솔 또는 게임 종료 전의 다른 상황에서는 호출되지 않을 수 있습니다.
	 * 이는 플랫폼별 창이 닫힐 때만 호출됩니다.
	 *
	 * @return 뷰포트를 닫을 수 있다면 true, 그렇지 않으면 false를 반환합니다.
	 */
	virtual bool WindowCloseRequested() { return true; }

	// 뷰포트 닫기 요청 시 호출됩니다.
	virtual void CloseRequested(FViewport* Viewport) = 0;

	// 히트 프록시 저장이 필요한지 여부를 반환합니다.
	virtual bool RequiresHitProxyStorage() { return true; }

	/**
	 * 게임 창이 현재 마우스를 캡처하고 있지 않을 때에도 InputAxis() 호출을 받을지 여부를 결정합니다.
	 * 이는 뷰포트의 마우스 캡처가 비활성화되어 있을 때 UI 시스템이 InputAxis 호출을 쉽게 받을 수 있도록 사용됩니다.
	 */
	virtual bool RequiresUncapturedAxisInput() const { return false; }

	/**
	 * 뷰포트 클라이언트가 키보드 입력을 필요로 하는지 여부를 결정합니다.
	 * @return 키보드 입력이 필요하면 true를 반환합니다.
	 */
	virtual bool RequiresKeyboardInput() const { return true; }

	/**
	 * 이 뷰포트가 직교(orthogonal)인지 여부를 반환합니다.
	 * 게임에서 히트 프록시를 사용하는 경우, GameViewportClient에서 올바르게 재정의되어야 합니다.
	 */
	virtual bool IsOrtho() const { return false; }

	/**
	 * 이 뷰포트가 비게임 요소들을 표시에서 제외하는지 여부를 반환합니다.
	 */
	virtual bool IsInGameView() const { return false; }

	/**
	 * 이 클라이언트에 대해 적절한 월드로 GWorld를 설정합니다.
	 *
	 * @return 이전 GWorld를 반환합니다.
	 */
	virtual class UWorld* ConditionalSetWorld() { return nullptr; }

	/**
	 * GWorld를 InWorld로 복원합니다.
	 *
	 * @param InWorld	복원할 월드
	 */
	virtual void ConditionalRestoreWorld(class UWorld* InWorld) {}

	/**
	 * 뷰포트 클라이언트가 현재 캡처 영역을 재정의할 수 있도록 허용합니다.
	 *
	 * @param OutCaptureRegion	재정의된 영역을 기록할 사각형에 대한 참조
	 * @return 캡처 영역이 재정의되었다면 true, 그렇지 않으면 false를 반환합니다.
	 */
	virtual bool OverrideHighResScreenshotCaptureRegion(FIntRect& OutCaptureRegion) { return false; }

	/**
	 * 뷰포트에 대해 통계(stat)를 표시할지 여부를 설정합니다.
	 */
	virtual void SetShowStats(bool bWantStats) = 0;

	/**
	 * 입력을 무시해야 하는지 확인합니다.
	 */
	virtual bool IgnoreInput() { return false; }
	/**
	 * 애플리케이션 실행 시 뷰포트가 마우스를 캡처하는지 여부를 반환합니다.
	 * 기술적으로 이는 첫 번째 창 활성화 시 캡처를 제어하므로, 애플리케이션이 실행되었으나 활성화되지 않은 경우에는 효과가 활성화될 때까지 지연됩니다.
	 */
	virtual bool CaptureMouseOnLaunch() { return true; }

	/**
	 * 뷰포트가 마우스를 캡처할 때 커서가 뷰포트에 고정되는지 여부를 반환합니다.
	 */
	virtual bool LockDuringCapture() { return true; }

	/**
	 * 뷰포트가 항상 마우스에 고정되어야 하는지 여부를 반환합니다.
	 */
	virtual bool ShouldAlwaysLockMouse() { return false; }

	/**
	 * 뷰포트가 마우스를 캡처할 때 커서를 숨길지 여부를 반환합니다.
	 */
	virtual bool HideCursorDuringCapture() const { return false; }
};

class FViewport : public FRenderTarget /*, protected FRenderResource*/
{
	// TODO: 델리게이트
	///** 뷰포트 리사이즈 이벤트에 대한 델리게이트 타입 (매개변수: FViewport* Viewport, uint32) */
	///** 뷰포트가 리사이즈될 때 전송됩니다. */

public:
	FViewport(FViewportClient* InViewportClient);

	virtual ~FViewport();

	// 창 핸들을 반환합니다.
	virtual void* GetWindow() { return nullptr; }
	// 창의 위치와 크기를 이동 및 변경합니다.
	virtual void MoveWindow(int32 NewPosX, int32 NewPosY, int32 NewSizeX, int32 NewSizeY) {};

	// 뷰포트를 파괴합니다.
	virtual void Destroy() {};

	// 마우스 캡처 여부를 반환합니다.
	virtual bool HasMouseCapture() const { return false; }
	// 포커스 여부를 반환합니다.
	virtual bool HasFocus() const { return true; }
	// 이 창이 포그라운드 창인지 여부를 반환합니다.
	virtual bool IsForegroundWindow() const { return true; }
	// 마우스 캡처를 설정합니다.
	virtual void CaptureMouse(bool bCapture) {}
	// 뷰포트에 마우스를 고정합니다.
	virtual void LockMouseToViewport(bool bLock) {}
	// 커서를 표시하거나 숨깁니다.
	virtual void ShowCursor(bool bVisible) {}
	// 마우스 커서를 업데이트합니다.
	virtual bool UpdateMouseCursor(bool bSetCursor) { return true; }

	// 소프트웨어 커서를 표시합니다.
	virtual void ShowSoftwareCursor(bool bVisible) {}
	// 소프트웨어 커서의 위치를 설정합니다.
	virtual void SetSoftwareCursorPosition(FVector2D Position) {}
	// 소프트웨어 커서의 표시 여부를 반환합니다.
	virtual bool IsSoftwareCursorVisible() const { return false; }

	// 커서가 현재 표시되고 있는지 여부를 반환합니다.
	virtual bool IsCursorVisible() const { return true; }

	// 사용자 포커스를 설정합니다.
	virtual bool SetUserFocus(bool bFocus) { return true; }
	//virtual bool KeyState(FKey Key) const = 0;
	// 마우스 X 좌표를 반환합니다.
	virtual int32 GetMouseX() const { return 0; }
	// 마우스 Y 좌표를 반환합니다.
	virtual int32 GetMouseY() const { return 0; };
	// 마우스 위치를 가져옵니다.
	virtual void GetMousePos(FIntPoint& MousePosition, const bool bLocalPosition = true) {};
	// 태블릿 압력을 반환합니다.
	virtual float GetTabletPressure() { return 0.f; }
	// 펜 입력 활성화 여부를 반환합니다.
	virtual bool IsPenActive() { return false; }
	// 마우스 좌표를 설정합니다.
	virtual void SetMouse(int32 x, int32 y) {};
	//virtual bool IsFullscreen()	const { return WindowMode == EWindowMode::Fullscreen || WindowMode == EWindowMode::WindowedFullscreen; }
	//virtual bool IsExclusiveFullscreen() const { return WindowMode == EWindowMode::Fullscreen; }
	//virtual EWindowMode::Type GetWindowMode()	const { return WindowMode; }
	// 입력을 처리합니다.
	virtual void ProcessInput(float DeltaTime) {};

	/**
	 * 가상 데스크탑 픽셀(기본 화면의 좌상단을 원점으로 함)을 이 뷰포트의 로컬 공간으로 변환합니다.
	 *
	 * @param VirtualDesktopPointPx 픽셀 단위의 가상 데스크탑 좌표. 여러 모니터가 지원되므로 데스크탑은 가상으로 간주됩니다.
	 *
	 * @return 변환된 픽셀. 값은 [0, 1] 범위로 정규화됩니다.
	 */
	virtual FVector2D VirtualDesktopPixelToViewport(FIntPoint VirtualDesktopPointPx) const { return FVector2D(0, 0); }

	/**
	 * 이 뷰포트의 로컬 공간 내의 좌표를 가상 데스크탑 픽셀로 변환합니다.
	 *
	 * @param ViewportCoordinate [0, 1] 범위로 정규화된 좌표; (0,0)은 좌상단, (1,1)은 우하단입니다.
	 *
	 * @return 변환된 좌표. 가상 데스크탑 픽셀 단위입니다.
	 */
	virtual FIntPoint ViewportToVirtualDesktopPixel(FVector2D ViewportCoordinate) const { return FIntPoint(0, 0); }

	// 슬레이트 커서에서 프리캡처 마우스 위치를 설정합니다.
	virtual void SetPreCaptureMousePosFromSlateCursor() {}

	// 렌더 프레임 시작을 큐에 등록합니다.
	virtual void EnqueueBeginRenderFrame(const bool bShouldPresent);
	/**
	 * 렌더링 프레임을 종료합니다. 게임 스레드에서 호출됩니다.
	 * @param bLockToVsync GPU가 VSYNC까지 대기할지 여부
	 * @param bShouldPresent 프레임이 화면에 표시되어야 하는지 여부
	 */
	virtual void EnqueueEndRenderFrame(const bool bLockToVsync, const bool bShouldPresent);

	/**
	 * 새 렌더링 프레임을 시작합니다. 렌더링 스레드에서 호출됩니다.
	 */
	virtual void BeginRenderFrame(/*FRHICommandListImmediate& RHICmdList*/);

	/**
	 * 렌더링 프레임을 종료합니다. 렌더링 스레드에서 호출됩니다.
	 * @param bPresent 프레임이 화면에 표시되어야 하는지 여부
	 * @param bLockToVsync GPU가 VSYNC까지 대기할지 여부
	 */
	virtual void EndRenderFrame(/*FRHICommandListImmediate& RHICmdList,*/ bool bPresent, bool bLockToVsync);

	// 뷰포트의 원하는 종횡비를 반환합니다.
	virtual float GetDesiredAspectRatio() const
	{
		FIntPoint Size = GetSizeXY();
		return static_cast<float>(Size.X) / static_cast<float>(Size.Y);
	}

	/**
	 * 뷰포트에 표시된 픽셀들을 무효화합니다.
	 */
	virtual void InvalidateDisplay() {}

	/**
	 * ViewportClient->Draw 호출 결과로 뷰포트에 표시된 픽셀들을 업데이트합니다.
	 *
	 * @param bShouldPresent 해당 프레임을 화면에 표시할지 여부
	 */
	void Draw(bool bShouldPresent = true);

	/**
	 * 캐시된 히트 프록시와 디스플레이를 무효화합니다.
	 */
	void Invalidate();

	///**
	// * 화면의 특정 영역에서 히트 프록시를 버퍼로 복사합니다.
	// * InRect는 뷰포트의 클라이언트 영역 내에 완전히 포함되어야 합니다.
	// * 히트 프록시가 캐시되지 않은 경우, 히트 테스트 캔버스를 사용하여 ViewportClient->Draw를 호출합니다.
	// */
	//void GetHitProxyMap(FIntRect InRect, TArray<HHitProxy*>& OutMap);

	///**
	// * 주어진 좌표에서 우세한(주요) 히트 프록시를 반환합니다.
	// * X, Y가 뷰포트 클라이언트 영역 밖이면 NULL을 반환합니다.
	// * 주의: 이 후 Invalidate를 호출하면 반환된 HHitProxy가 해제됩니다.
	// */
	//HHitProxy* GetHitProxy(int32 X, int32 Y);

	///**
	// * 지정된 영역 내의 모든 액터와 모델을 반환합니다.
	// * InRect는 뷰포트의 클라이언트 영역 내에 완전히 포함되어야 합니다.
	// * 히트 프록시가 캐시되지 않은 경우, 히트 테스트 캔버스를 사용하여 ViewportClient->Draw를 호출합니다.
	// */
	//void GetActorsAndModelsInHitProxy(FIntRect InRect, TSet<AActor*>& OutActors, TSet<UModel*>& OutModels);

	/**
	 * 뷰포트가 프레임 인터페이스를 가지고 있다면 해당 인터페이스를 반환합니다.
	 * @return 뷰포트 프레임 인터페이스
	 */
	virtual FViewportFrame* GetViewportFrame() = 0;

	/**
	 * 화면 비율이 고정될 때 뷰포트 내부의 뷰 영역을 계산합니다.
	 * 시네마틱 바를 만들 때 사용됩니다.
	 * @param Aspect [입력] 고정할 비율
	 * @param ViewRect [입력] 제한되지 않은 뷰 사각형
	 * @return 제한된 뷰 사각형
	 */
	FIntRect CalculateViewExtents(float AspectRatio, const FIntRect& ViewRect);

	/**
	 * 화면 비율이 고정될 때 뷰포트 내부의 뷰 영역을 계산합니다.
	 * 시네마틱 바를 만들 때 사용됩니다.
	 * @param AspectRatio [입력] 고정할 비율
	 * @param DesiredAspectRatio [입력] 뷰포트의 비율
	 * @param ViewRect [입력] 제한되지 않은 뷰 사각형
	 * @param DestSize [입력] 뷰포트의 크기
	 * @return 제한된 뷰 사각형
	 */
	static FIntRect CalculateViewExtents(float AspectRatio, float DesiredAspectRatio, const FIntRect& ViewRect, const FIntPoint& DestSize);

	/**
	 * 생성 시 제공되지 않은 경우 뷰포트 클라이언트를 설정합니다.
	 * @param InViewportClient 설정할 뷰포트 클라이언트
	 */
	virtual void SetViewportClient(FViewportClient* InViewportClient)
	{
		ViewportClient = InViewportClient;
	}

	//~ Begin FRenderTarget 인터페이스.
	// 뷰포트의 크기를 반환합니다.
	virtual FIntPoint GetSizeXY() const { return FIntPoint(SizeX, SizeY); }
	// 뷰포트의 초기 위치를 반환합니다.
	FIntPoint GetInitialPositionXY() const { return FIntPoint(InitialPositionX, InitialPositionY); }
	// 접근자: 뷰포트 클라이언트를 반환합니다.
	FViewportClient* GetClient() const { return ViewportClient; }

	/**
	 * 현재 백 버퍼로 렌더 타겟 표면 RHI를 업데이트합니다.
	 */
	void UpdateRenderTargetSurfaceRHIToCurrentBackBuffer();

	// 뷰포트 RHI를 반환합니다.
	const FRHIViewport* GetViewportRHI() const { return ViewportRHI; }

	/**
	 * 뷰포트의 초기 크기를 설정합니다. 뷰포트가 이미 크기 조정되었다면 아무 작업도 수행하지 않습니다.
	 *
	 * @param InitialSizeXY 뷰포트의 초기 크기
	 */
	void SetInitialSize(FIntPoint InitialSizeXY);

	/** 플레이 인 에디터용 뷰포트라면 true를 반환합니다. */
	bool IsPlayInEditorViewport() const
	{
		return bIsPlayInEditorViewport;
	}

	/** 이 뷰포트를 플레이 인 에디터 뷰포트로 설정합니다. */
	void SetPlayInEditorViewport(bool bInPlayInEditorViewport)
	{
		bIsPlayInEditorViewport = bInPlayInEditorViewport;
	}

	/** 이 뷰포트가 FSlateSceneViewport라면 true를 반환합니다. */
	bool IsSlateViewport() const { return bIsSlateViewport; }

	// 렌더 타겟 텍스처의 크기를 반환합니다.
	virtual FIntPoint GetRenderTargetTextureSizeXY() const { return GetSizeXY(); }

protected:
	// 뷰포트의 클라이언트입니다.
	FViewportClient* ViewportClient;

	// virtual void UpdateViewportRHI(bool bDestroyed, uint32 NewSizeX, uint32 NewSizeY, EWindowMode::Type NewWindowMode, EPixelFormat PreferredPixelFormat);

	// 뷰포트의 RHI 객체입니다.
	FRHIViewport* ViewportRHI;

	// 뷰포트의 초기 X 위치입니다.
	uint32 InitialPositionX;
	/** 뷰포트의 초기 위치입니다. */
	uint32 InitialPositionY;

	/** 뷰포트의 너비입니다. */
	uint32 SizeX;

	/** 뷰포트의 높이입니다. */
	uint32 SizeY;

	// 플레이 인 에디터 뷰포트 여부.
	bool bIsPlayInEditorViewport;

	/** 이 뷰포트가 FSlateSceneViewport라면 true입니다. */
	bool bIsSlateViewport;

	//~ Begin FRenderResource 인터페이스.
	virtual void ReleaseRHI();
	virtual void InitRHI(/*FRHICommandListBase& RHICmdList*/);
};


class SViewport;

class FSceneViewport : public FViewportFrame, public FViewport, public ISlateViewport
{
public:
	// 생성자: 뷰포트 클라이언트와 뷰포트 위젯을 전달받아 FSceneViewport를 생성합니다.
	FSceneViewport(FViewportClient* InViewportClient, std::shared_ptr<SViewport> InViewportWidget);
	// 소멸자
	~FSceneViewport();

	// 창 핸들을 반환합니다.
	virtual void* GetWindow() override { return nullptr; }

	/** FViewport 인터페이스 */
	// 창의 위치 및 크기를 변경합니다.
	virtual void MoveWindow(int32 NewPosX, int32 NewPosY, int32 NewSizeX, int32 NewSizeY) override {};
	// 마우스 캡처 상태를 반환합니다.
	virtual bool HasMouseCapture() const override;
	// 포커스 여부를 반환합니다.
	virtual bool HasFocus() const override;
	// 이 창이 포그라운드 창인지 여부를 반환합니다.
	virtual bool IsForegroundWindow() const override;
	// 마우스 캡처를 설정합니다.
	virtual void CaptureMouse(bool bCapture) override;
	// 뷰포트에 마우스를 고정합니다.
	virtual void LockMouseToViewport(bool bLock) override;
	// 커서를 표시하거나 숨깁니다.
	virtual void ShowCursor(bool bVisible) override;
	// Slate 커서의 위치에서 캡처 전 마우스 위치를 설정합니다.
	virtual void SetPreCaptureMousePosFromSlateCursor() override;
	// 커서가 보이는지 여부를 반환합니다.
	virtual bool IsCursorVisible() const override { return bIsCursorVisible; }
	// 소프트웨어 커서를 표시합니다.
	virtual void ShowSoftwareCursor(bool bVisible) override { bIsSoftwareCursorVisible = bVisible; }
	// 소프트웨어 커서의 위치를 설정합니다.
	virtual void SetSoftwareCursorPosition(FVector2D Position) override { SoftwareCursorPosition = Position; }
	// 소프트웨어 커서의 표시 여부를 반환합니다.
	virtual bool IsSoftwareCursorVisible() const override { return bIsSoftwareCursorVisible; }
	// 소프트웨어 커서의 현재 위치를 반환합니다.
	virtual FVector2D GetSoftwareCursorPosition() const override { return SoftwareCursorPosition; }
	// 렌더 프레임 종료를 큐에 등록합니다.
	virtual void EnqueueEndRenderFrame(const bool bLockToVsync, const bool bShouldPresent) override;

	/** 현재 스레드에 따라 적절한 렌더 타겟을 반환합니다. */
	// TODO: 렌더 타겟 텍스처 관련 구현
	//virtual const FTextureRHIRef& GetRenderTargetTexture() const;
	//virtual void SetRenderTargetTextureRenderThread(FTextureRHIRef& RT);

	//virtual bool KeyState(FKey Key) const override;

	// 마우스 X 좌표를 반환합니다.
	virtual int32 GetMouseX() const override;
	// 마우스 Y 좌표를 반환합니다.
	virtual int32 GetMouseY() const override;
	// 마우스 위치를 가져옵니다.
	virtual void GetMousePos(FIntPoint& MousePosition, const bool bLocalPosition = true) override;
	// 마우스 좌표를 설정합니다.
	virtual void SetMouse(int32 X, int32 Y) override;
	// 입력을 처리합니다.
	virtual void ProcessInput(float DeltaTime) override;

	/** FViewportFrame 인터페이스 */
	// 현재 뷰포트(자기 자신)를 반환합니다.
	virtual FViewport* GetViewport() override { return this; }
	// 현재 뷰포트 프레임(자기 자신)을 반환합니다.
	virtual FViewportFrame* GetViewportFrame() override { return this; }

	/** @return 사용 중인 뷰포트 위젯을 반환합니다. */
	std::weak_ptr<SViewport> GetViewportWidget() const { return ViewportWidget; }

	/** BeginRenderFrame이 큐에 등록되기 전에 호출됩니다. */
	virtual void EnqueueBeginRenderFrame(const bool bShouldPresent) override;

	/** 프레임 렌더링 시작 시 호출됩니다. */
	//virtual void BeginRenderFrame(FRHICommandListImmediate& RHICmdList) override;

	//virtual void EndRenderFrame(FRHICommandListImmediate& RHICmdList, bool bPresent, bool bLockToVsync) override;

	// 틱(Tick) 함수를 호출하여 뷰포트를 업데이트합니다.
	virtual void Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float InDeltaTime) override;

	/**
	 * 이 뷰포트가 위치한 실제 창을 리사이즈할 때 호출됩니다.
	 *
	 * @param NewSizeX		뷰포트의 새로운 너비
	 * @param NewSizeY		뷰포트의 새로운 높이
	 * @param NewWindowMode	뷰포트를 변경할 창 모드(예: 창 모드, 전체 화면 등)
	 */
	virtual void ResizeFrame(uint32 NewSizeX, uint32 NewSizeY/*, EWindowMode::Type NewWindowMode*/) override;

	// TODO: 델리게이트
	/**
	 * 뷰포트 리사이즈 델리게이트를 설정합니다.
	 */
	 //void SetOnSceneViewportResizeDel(FOnSceneViewportResize InOnSceneViewportResize)
	 //{
	 //	OnSceneViewportResizeDel = InOnSceneViewportResize;
	 //}

	 //virtual void UpdateViewportRHI(bool bDestroyed, uint32 NewSizeX, uint32 NewSizeY, EWindowMode::Type NewWindowMode, EPixelFormat PreferredPixelFormat) override;

	 /** ISlateViewport 인터페이스 */
	 // 뷰포트를 그릴 때 호출됩니다.
	virtual void OnDrawViewport(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect/*, FSlateWindowElementList& OutDrawElements*/, int32 LayerId, bool bParentEnabled) override;
	// 커서 모양을 결정하기 위해 호출됩니다.
	virtual FCursorReply OnCursorQuery(const FGeometry& MyGeometry/*, const FPointerEvent& CursorEvent*/) override;
	// 커서 매핑을 처리합니다.
	virtual std::optional<std::shared_ptr<SWidget>> OnMapCursor(const FCursorReply& CursorReply) override;
	// 마우스 버튼이 눌렸을 때 호출됩니다.
	virtual FReply OnMouseButtonDown(const FGeometry& InGeometry/*, const FPointerEvent& MouseEvent*/) override;
	// 마우스 버튼이 떼어졌을 때 호출됩니다.
	virtual FReply OnMouseButtonUp(const FGeometry& InGeometry/*, const FPointerEvent& MouseEvent*/) override;
	// 마우스가 뷰포트에 들어올 때 호출됩니다.
	virtual void OnMouseEnter(const FGeometry& MyGeometry/*, const FPointerEvent& MouseEvent*/) override;
	// 마우스가 뷰포트에서 나갈 때 호출됩니다.
	virtual void OnMouseLeave(/*const FPointerEvent& MouseEvent*/) override;
	// 마우스가 움직일 때 호출됩니다.
	virtual FReply OnMouseMove(const FGeometry& InGeometry/*, const FPointerEvent& MouseEvent*/) override;
	// 마우스 휠이 사용될 때 호출됩니다.
	virtual FReply OnMouseWheel(const FGeometry& InGeometry/*, const FPointerEvent& MouseEvent*/) override;
	// 마우스 버튼 더블클릭이 발생할 때 호출됩니다.
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InGeometry/*, const FPointerEvent& InMouseEvent*/) override;
	// 키가 눌렸을 때 호출됩니다.
	virtual FReply OnKeyDown(const FGeometry& InGeometry/*, const FKeyEvent& InKeyEvent*/) override;
	// 키가 떼어졌을 때 호출됩니다.
	virtual FReply OnKeyUp(const FGeometry& InGeometry/*, const FKeyEvent& InKeyEvent*/) override;
	// 문자 입력 이벤트가 발생했을 때 호출됩니다.
	virtual FReply OnKeyChar(const FGeometry& InGeometry/*, const FCharacterEvent& InCharacterEvent*/) override;
	// 포커스를 받았을 때 호출됩니다.
	virtual FReply OnFocusReceived(/*const FFocusEvent& InFocusEvent*/) override;
	// 포커스를 잃었을 때 호출됩니다.
	virtual void OnFocusLost(/*const FFocusEvent& InFocusEvent*/) override;
	// 뷰포트가 닫힐 때 호출됩니다.
	virtual void OnViewportClosed() override;
	// 창 닫기 요청이 있을 때 호출되며, 처리 결과를 반환합니다.
	virtual FReply OnRequestWindowClose() override;
	// 이 뷰포트와 연결된 위젯을 반환합니다.
	virtual std::weak_ptr<SWidget> GetWidget() override;
	// 뷰포트가 활성화될 때 호출됩니다.
	virtual FReply OnViewportActivated(/*const FWindowActivateEvent& InActivateEvent*/) override;
	// 뷰포트가 비활성화될 때 호출됩니다.
	virtual void OnViewportDeactivated(/*const FWindowActivateEvent& InActivateEvent*/) override;
	// 뷰포트의 크기를 반환합니다.
	virtual FIntPoint GetSize() const override { return GetSizeXY(); }

	// 뷰포트 크기를 설정합니다.
	void SetViewportSize(uint32 NewSizeX, uint32 NewSizeY);
	// 고정된 뷰포트 크기를 설정합니다.
	void SetFixedViewportSize(uint32 NewSizeX, uint32 NewSizeY);

	/** 뷰포트가 고정된 크기를 가지는지 여부를 반환합니다. */
	bool HasFixedSize() const;

	// 이 뷰포트가 속한 SWindow를 찾습니다.
	std::shared_ptr<class SWindow> FindSWindow();

private:
	// 뷰포트를 파괴합니다.
	void Destroy() override;

	// FRenderResource 인터페이스.
	//virtual void InitRHI(FRHICommandListBase& RHICmdList) override;
	//virtual void ReleaseRHI() override;\

	/**
	 * Slate에서 뷰포트를 리사이즈해야 할 때 호출됩니다.
	 *
	 * @param NewSizeX		 뷰포트의 새로운 너비
	 * @param NewSizeY		 뷰포트의 새로운 높이
	 * @param NewWindowMode	 뷰포트를 변경할 창 모드(예: 창 모드, 전체 화면 등)
	 */
	virtual void ResizeViewport(uint32 NewSizeX, uint32 NewSizeY/*, EWindowMode::Type NewWindowMode*/);

	/**
	 * 마우스 이벤트로부터 캐시된 커서 위치를 업데이트합니다.
	 *
	 * @param InGeometry	로컬 공간으로 변환할 뷰포트의 기하학 정보
	 * @param InMouseEvent	절대 좌표의 마우스 이벤트 정보
	 */
	void UpdateCachedCursorPos(const FGeometry& InGeometry/*, const FPointerEvent& InMouseEvent*/);

	/**
	 * 캐시된 뷰포트 기하학 정보를 업데이트합니다.
	 *
	 * @param InGeometry	로컬 공간으로 변환할 뷰포트의 기하학 정보
	 */
	void UpdateCachedGeometry(const FGeometry& InGeometry);

	/** 현재 게임 뷰포트인지 여부를 판단하는 유틸리티 함수 */
	bool IsCurrentlyGameViewport();

	/**
	 * Slate 창이 파괴되기 직전에 호출되어, 창을 지원하는 백버퍼 리소스를 해제할 수 있도록 합니다.
	 */
	void OnWindowBackBufferResourceDestroyed(void* Backbuffer);

	/**
	 * 백버퍼 리사이즈 전 호출됩니다. 이 뷰포트가 해당 백버퍼를 사용 중이면 리소스를 해제합니다.
	 */
	void OnPreResizeWindowBackbuffer(void* Backbuffer);

	/**
	 * 백버퍼 리사이즈 후 호출됩니다. 필요시 이 뷰포트가 백버퍼 핸들을 재획득합니다.
	 */
	void OnPostResizeWindowBackbuffer(void* Backbuffer);

private:
	/** 입력 이벤트가 발생할 때마다 초기화되는 중간 응답 상태입니다. */
	FReply CurrentReplyState;
	/** 키 이름과 누른 상태의 매핑 (주석 처리됨) */
	//TMap<FKey, bool> KeyStateMap;
	/** 마지막으로 알려진 로컬 공간의 마우스 위치 (-1, -1이면 알 수 없음) */
	FIntPoint CachedCursorPos;
	/** 마지막으로 알려진 기하학 정보 */
	FGeometry CachedGeometry;
	/** 최신 캡처 이전의 마우스 위치 */
	FIntPoint PreCaptureCursorPos;
	/** 현재 소프트웨어 커서의 위치 */
	FVector2D SoftwareCursorPosition;
	/** 뷰포트에 소프트웨어 커서를 그릴지 여부 */
	bool bIsSoftwareCursorVisible;
	/** 이 뷰포트가 그려지는 Slate 뷰포트 위젯 */
	std::weak_ptr<SViewport> ViewportWidget;
	/** 마지막 입력 처리 이후 X축 입력 샘플 수 */
	int32 NumMouseSamplesX;
	/** 마지막 입력 처리 이후 Y축 입력 샘플 수 */
	int32 NumMouseSamplesY;
	/** 현재 마우스 델타 값 */
	FIntPoint MouseDelta;
	/** 현재 커서가 보이는지 여부 */
	bool bIsCursorVisible;
	/** 비활성화 시 캡처가 있었는지 여부 */
	bool bShouldCaptureMouseOnActivate;
	/** 이 뷰포트가 별도의 렌더 타겟에 렌더링되는 경우 true, 창의 백버퍼에 직접 렌더링하면 false */
	bool bUseSeparateRenderTarget;
	/** HMD(헤드 마운트 디스플레이) 때문에 별도의 렌더 타겟 사용을 강제해야 하는 경우 true */
	bool bForceSeparateRenderTarget;
	/** 현재 리사이즈 중인지 여부 */
	bool bIsResizing;
	/** 리사이즈 시 RenderTarget 크기를 변경하지 않음 */
	bool bForceViewportSize;
	// TODO: 델리게이트
	/** ResizeFrame에서 ResizeViewport 후에 발생하는 델리게이트 (주석 처리됨) */
	//FOnSceneViewportResize OnSceneViewportResizeDel;
	/** 캡처로 인해 커서를 숨길 때의 커서 위치 (나중에 복원하기 위해 저장) */
	FIntPoint MousePosBeforeHiddenDueToCapture;

	/** 일부 HMD RHI의 재투영을 위해 뷰포트 타겟을 버퍼링해야 합니다. */
	/** Slate가 뷰포트를 그리기 위해 사용하는 렌더 타겟. 이 뷰포트가 창의 백버퍼에 직접 렌더링하면 null일 수 있습니다. */
	TArray<class FSlateRenderTargetRHI*> BufferedSlateHandles;
	//TArray<FTextureRHIRef> BufferedRenderTargetsRHI;
	//TArray<FTextureRHIRef> BufferedShaderResourceTexturesRHI;

	//FTextureRHIRef RenderTargetTextureRenderThreadRHI;
	// 렌더 스레드에서 사용되는 Slate 텍스처 (렌더 타겟)
	class FSlateRenderTargetRHI* RenderThreadSlateTexture;
};

class FEditorViewportClient : public FViewportClient
{
public:
	friend class FMouseDeltaTracker;
	FEditorViewportClient(const std::weak_ptr<SEditorViewport>& InEditorViewportWidget);

	/** 복사 불가능 */
	FEditorViewportClient(const FEditorViewportClient&) = delete;
	FEditorViewportClient& operator=(const FEditorViewportClient&) = delete;

	// 카메라 설정을 지정합니다.
	void SetCameraSetup(const FVector& LocationForOrbiting, const FQuat& InOrbitRotation, const FVector& InOrbitZoom, const FVector& InOrbitLookAt, const FVector& InViewLocation, const FQuat& InViewRotation);

	/** 카메라 잠금 플래그를 전환하기 위한 콜백입니다. */
	virtual void SetCameraLock();

	/** 카메라 잠금 플래그를 확인하는 콜백입니다. */
	bool IsCameraLocked() const;

	/** 그리드 표시 플래그를 전환하기 위한 콜백입니다. */
	void SetShowGrid();

	/** 경계선 표시 플래그를 설정합니다. */
	void SetShowBounds(bool bShow);

	/** 충돌 기하구조 표시 플래그를 전환하기 위한 콜백입니다. */
	void SetShowCollision();

	virtual UWorld* GetWorld() const override;

	// TODO: CameraTransform
	/** 현재 뷰포트 유형에 대한 ViewportCameraTransform 객체를 반환합니다. */
	FViewportCameraTransform& GetViewTransform()
	{
		return IsPerspective() ? ViewTransformPerspective : ViewTransformOrthographic;
	}

	const FViewportCameraTransform& GetViewTransform() const
	{
		return IsPerspective() ? ViewTransformPerspective : ViewTransformOrthographic;
	}
	// TODO: Camera
	///** 뷰포트의 카메라 위치를 설정합니다. */
	void SetViewLocation(const FVector& NewLocation)
	{
		FViewportCameraTransform& ViewTransform = GetViewTransform();
		ViewTransform.SetLocation(NewLocation);
	}

	///** 뷰포트의 카메라 회전을 설정합니다. */
	void SetViewRotation(const FQuat& NewRotation)
	{
		FViewportCameraTransform& ViewTransform = GetViewTransform();
		ViewTransform.SetRotation(NewRotation);
	}

	///**
	// * 오비트(Orbit) 카메라의 경우 뷰포트 카메라의 LookAt 위치를 설정합니다.
	// *
	// * @param LookAt 새 LookAt 위치
	// * @param bRecalculateView 즉시 새로운 LookAt 위치로 뷰 위치와 회전을 재계산할지 여부
	// */
	void SetLookAtLocation(const FVector& LookAt, bool bRecalculateView = false)
	{
		FViewportCameraTransform& ViewTransform = GetViewTransform();
		ViewTransform.SetLookAt(LookAt);

		if (bRecalculateView)
		{
			/*FMatrix OrbitMatrix = ViewTransform.ComputeOrbitMatrix();
			OrbitMatrix = OrbitMatrix.InverseFast();

			ViewTransform.SetRotation(OrbitMatrix.Rotator());
			ViewTransform.SetLocation(OrbitMatrix.GetOrigin());*/
		}
	}

	// TODO: Camera
	//	/** 직교(Ortho) 줌(Zoom) 값을 설정합니다. */
	void SetOrthoZoom(float InOrthoZoom)
	{
		FViewportCameraTransform& ViewTransform = GetViewTransform();

		// 0의 직교 줌은 지원되지 않으며 NaN 또는 0으로 나누기 오류를 발생시킵니다.
		ViewTransform.SetOrthoZoom(InOrthoZoom);
	}

	///** 현재 뷰포트 카메라의 위치를 반환합니다. */
	const FVector& GetViewLocation() const
	{
		const FViewportCameraTransform& ViewTransform = GetViewTransform();
		return ViewTransform.GetLocation();
	}

	///** 현재 뷰포트 카메라의 회전을 반환합니다. */
	const FQuat& GetViewRotation() const
	{
		const FViewportCameraTransform& ViewTransform = GetViewTransform();
		return ViewTransform.GetRotation();
	}

	///** 현재 LookAt 위치를 반환합니다. */
	const FVector& GetLookAtLocation() const
	{
		const FViewportCameraTransform& ViewTransform = GetViewTransform();
		return ViewTransform.GetLookAt();
	}

	///** 현재 직교 줌 값을 반환합니다. */
	float GetOrthoZoom() const
	{
		const FViewportCameraTransform& ViewTransform = GetViewTransform();
		return ViewTransform.GetOrthoZoom();
	}

	///** FViewElementDrawer 인터페이스 (주석 처리됨) */
	//virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	//virtual void Draw(FViewport* Viewport, FCanvas* Canvas) override;

public:
	/** FViewportClient 인터페이스 */
	virtual bool InputAxis(FViewport* Viewport/*, FKey Key*/, float Delta, float DeltaTime, int32 NumSamples = 1, bool bGamepad = false) override;
	virtual void ReceivedFocus(FViewport* Viewport) override;
	virtual void MouseEnter(FViewport* Viewport, int32 x, int32 y) override;
	virtual void MouseMove(FViewport* Viewport, int32 x, int32 y) override;
	virtual void MouseLeave(FViewport* Viewport) override;
	virtual EMouseCursor GetCursor(FViewport* Viewport, int32 X, int32 Y) override;
	virtual void CapturedMouseMove(FViewport* InViewport, int32 InMouseX, int32 InMouseY) override;
	virtual void ProcessAccumulatedPointerInput(FViewport* InViewport) override;
	virtual bool IsOrtho() const override;
	virtual void LostFocus(FViewport* Viewport) override;

	//virtual bool BeginTransform(const FGizmoState& InState) { return false; }
	//virtual bool EndTransform(const FGizmoState& InState) { return false; }

protected:
	// TODO: InternalInput
	virtual bool Internal_InputKey(/*const FInputKeyEventArgs& EventArgs*/);
	virtual bool Internal_InputAxis(FViewport* Viewport/*, FKey Key*/, float Delta, float DeltaTime, int32 NumSamples = 1, bool bGamepad = false);

	// TODO: ProcessClick
	///**
	// * 사용자가 뷰포트 내에서 클릭할 때 호출됩니다.
	// *
	// * @param View			뷰포트의 씬 뷰
	// * @param HitProxy		클릭된 히트 프록시
	// * @param Key			클릭 이벤트를 유발한 키
	// * @param Event			눌린 키의 상태
	// * @param HitX			마우스의 X 위치
	// * @param HitY			마우스의 Y 위치
	// */
	// virtual void ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY);

	// TODO: TrackingMouse
	/**
	 * 마우스 이동 추적이 시작될 때 호출됩니다.
	 *
	 * @param InInputState	현재 마우스 및 키보드 입력 상태
	 * @param bIsDraggingWidget 위젯이 드래그되고 있다면 true
	 * @param bNudge			화살표 키로 이동한 경우라면 true
	 */
	 //virtual
	 //void TrackingStarted(const struct FInputEventState& InInputState, bool bIsDraggingWidget, bool bNudge) {}

	 // TODO: InputWidgetDelta
	 /**
	  * 위젯이 이동될 때 뷰포트 클라이언트에 이를 처리할 기회를 제공합니다.
	  *
	  * @param InViewport		렌더링 중인 뷰포트
	  * @param CurrentAxis		현재 이동 중인 위젯 축
	  * @param Drag				위젯이 이동한 양 (위젯의 좌표계에 따라 값이 달라집니다. GetWidgetCoordSystem 참고)
	  * @param Rot				위젯이 회전한 양 (위젯의 좌표계에 따라 값이 달라집니다. GetWidgetCoordSystem 참고)
	  * @param Scale			위젯이 스케일된 양 (위젯의 좌표계에 따라 값이 달라집니다. GetWidgetCoordSystem 참고)
	  */
	  //virtual bool InputWidgetDelta(FViewport* InViewport, EAxisList::Type CurrentAxis, FVector& Drag, FQuat& Rot, FVector& Scale);

	 /**
	  * @return 변환 위젯의 월드 공간 위치를 반환합니다.
	  */
	virtual FVector GetWidgetLocation() const;

	/**
	 * @return 변환 위젯의 현재 좌표계를 반환합니다.
	 * 월드 좌표계를 사용하면 항등 행렬(identity matrix)을 반환합니다.
	 */
	virtual FMatrix GetWidgetCoordSystem() const;

	/**
	 * @return 변환 위젯의 로컬 좌표계를 반환합니다.
	 * 월드 좌표계를 사용하면 항등 행렬(identity matrix)을 반환합니다.
	 */
	virtual FMatrix GetLocalCoordinateSystem() const;

	/**
	 * 현재 변환 위젯의 크기를 주어진 델타 값만큼 조정합니다.
	 */
	void AdjustTransformWidgetSize(const int32 SizeDelta);

	/**
	 * 렌더링을 위한 뷰 설정에 추가적인 설정을 수행하기 위해 호출됩니다.
	 *
	 * @param ViewFamily	렌더링 중인 뷰 패밀리
	 * @param View			렌더링 중인 뷰
	 */
	 //virtual void SetupViewForRendering(FSceneViewFamily& ViewFamily, FSceneView& View);

	 // TODO: Draw
	 /**
	  * 뷰포트의 2D 캔버스에 그리기 위해 호출됩니다.
	  *
	  * @param InViewport	렌더링 중인 뷰포트
	  * @param View			렌더링할 씬 뷰
	  * @param Canvas		그릴 캔버스
	  */
	  //virtual void DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& Canvas);

	virtual void Tick(float DeltaSeconds);

	/**
	 * 매 프레임 호출되어 마우스 이동 델타를 기반으로 뷰포트를 업데이트합니다.
	 */
	virtual void UpdateMouseDelta();

	/** 이 뷰포트가 레벨 에디터 뷰포트인 경우 true를 반환합니다. */
	virtual bool IsLevelEditorClient() const { return false; }

	/** 이 뷰포트가 원근(Perspective) 뷰포트이면 true를 반환합니다. */
	bool IsPerspective() const;

	/**
	 * 주어진 LookAt 포인트가 뷰포트의 중앙에 오도록 뷰포트를 평행 이동합니다.
	 * 현재 Location/LookAt 간의 거리는 유지됩니다.
	 *
	 * @param NewLookAt		집중할 새로운 LookAt 포인트
	 * @param bInstant		즉시 포커스할지 여부
	 */
	void CenterViewportAtPoint(const FVector& NewLookAt, bool bInstant = false);

	/**
	 * 지정된 드래그 및 회전 값에 따라 뷰포트 카메라를 이동합니다.
	 */
	void MoveViewportCamera(const FVector& InDrag, const FQuat& InRot, bool bDollyCamera = false);

	/** @return 마우스 버튼이 눌려 있고, 그 움직임이 뷰포트 내 작업을 위해 추적되고 있다면 true를 반환합니다. */
	bool IsTracking() const { return bIsTracking; }

	/** 이 뷰포트의 카메라 속도를 반환합니다. */
	float GetCameraSpeed() const;

	/** 지정된 속도 설정에 따라 이 뷰포트의 카메라 속도를 반환합니다. */
	float GetCameraSpeed(int32 SpeedSetting) const;

	/** 이 뷰포트의 카메라 속도 설정을 변경합니다. */
	virtual void SetCameraSpeedSetting(int32 SpeedSetting);

	/** 이 뷰포트의 카메라 속도 설정을 반환합니다. */
	virtual int32 GetCameraSpeedSetting() const;

	/** 이 뷰포트의 카메라 속도 스칼라 값을 반환합니다. */
	virtual float GetCameraSpeedScalar() const;

	/** 이 뷰포트의 카메라 속도 스칼라 값을 설정합니다. */
	virtual void SetCameraSpeedScalar(float SpeedScalar);

	/** 에디터 뷰포트 위젯을 반환합니다. */
	std::shared_ptr<SEditorViewport> GetEditorViewportWidget() const { return EditorViewportWidget.lock(); }

	/**
	 * 뷰포트 위치와 회전에 사용할 행렬을 계산합니다.
	 */
	virtual FMatrix CalcViewRotationMatrix(const FQuat& InViewRotation) const;
protected:
	/** 오비트 카메라가 현재 사용 중이면 true입니다. */
	bool bUsingOrbitCamera;

	/** 이 창이 시네마틱 도구에 의해 소유될 수 있도록 허용하는 경우 true입니다. */
	bool bAllowCinematicControl;

	/** 카메라 속도 설정 */
	int32 CameraSpeedSetting;

	/** 카메라 속도 스칼라 */
	float CameraSpeedScalar;

public:
	/** 이 뷰포트의 Near(근) 클리핑 평면 값을 반환합니다. */
	float GetNearClipPlane() const;

	/** Near 클리핑 평면을 재정의합니다. 재정의를 비활성화하려면 음수 값을 설정하세요. */
	void OverrideNearClipPlane(float InNearPlane);

	/** 이 뷰포트의 Far(원) 클리핑 평면 재정의 값을 반환합니다. */
	float GetFarClipPlaneOverride() const;

	/** Far 클리핑 평면을 재정의합니다. 재정의를 비활성화하려면 음수 값을 설정하세요. */
	void OverrideFarClipPlane(const float InFarPlane);

	/** 위젯을 표시하거나 숨깁니다. */
	void ShowWidget(const bool bShow);
	bool GetShowWidget() const { return bShowWidget; }

	/** 마우스 정보를 변환하기 위해 사용됩니다. (FEdMode에서 위젯이 움직이는 것을 처리하기 전에 마우스 델타를 얻기 위해 사용) */
	//FMouseDeltaTracker* GetMouseDeltaTracker() const { return  MouseDeltaTracker; }

	virtual uint32 GetCachedMouseX() const { return CachedMouseX; }
	virtual uint32 GetCachedMouseY() const { return CachedMouseY; }

protected:
	/**
	 * 원근 뷰포트 카메라가 이동할 때 호출됩니다.
	 */
	virtual void PerspectiveCameraMoved() {}

	/** 전달된 회전 델타로 회전 위젯을 업데이트합니다. */
	void ApplyDeltaToRotateWidget(const FQuat& InRot);

	/**
	 * 사용자가 직교(Ortho) 뷰포트를 줌할 때 호출됩니다.
	 *
	 * @param Scale 기본 줌 양을 수정할 수 있습니다.
	 */
	void OnOrthoZoom(/*const struct FInputEventState& InputState,*/ float Scale = 1.0f);

	/**
	 * 사용자가 카메라 속도를 변경할 때 호출됩니다.
	 *
	 * @param InputState 현재 마우스 및 키보드 입력 상태
	 */
	void OnChangeCameraSpeed(/*const struct FInputEventState& InputState*/);

	/** 카메라 잠금을 활성화 또는 비활성화합니다. */
	void EnableCameraLock(bool bEnable);

	/** DrawSafeFrames에서 현재 안전 영역(aspect ratio)을 가져오기 위한 헬퍼 함수입니다. */
	virtual bool GetActiveSafeFrame(float& OutAspectRatio) const { return false; }

	/** 현재 뷰포트에서 안전 프레임 사각형을 계산하는 헬퍼 함수입니다. */
	bool CalculateEditorConstrainedViewRect(FSlateRect& OutSafeFrameRect, FViewport* InViewport);

	/** 원근 카메라를 이동합니다. */
	void MoveViewportPerspectiveCamera(const FVector& InDrag, const FQuat& InRot, bool bDollyCamera = false);

	/**
	 * 실시간 카메라 이동을 업데이트합니다. (매 뷰포트 틱마다 호출되어야 합니다.)
	 *
	 * @param DeltaTime 마지막 업데이트 이후 경과 시간(초)
	 */
	void UpdateCameraMovement(float DeltaTime);

	// TODO: Render
	/** 안전 프레임 선들을 렌더링합니다. */
	//void DrawSafeFrames(FViewport& Viewport, FSceneView& View, FCanvas& Canvas);

	/** 카메라가 이동하기 직전 또는 멈추기 직전에 호출됩니다. */
	virtual void BeginCameraMovement(bool bHasMovement) {}
	virtual void EndCameraMovement() {}
public:
	static const uint32 MaxCameraSpeeds;

	// TODO: 델리게이트
	///** 이 클라이언트가 몰입형 뷰포트에 있는지 여부를 가져오는 델리게이트 */
	//FViewportStateGetter ImmersiveDelegate;

	///** Slate 뷰포트 레이아웃 및 탭 구성에서 이 클라이언트의 가시성을 가져오는 델리게이트 */
	//FViewportStateGetter VisibilityDelegate;

	FViewport* Viewport;
	bool bShowWidget;

	/** 원근 뷰포트용 카메라 변환 데이터 */
	FViewportCameraTransform ViewTransformPerspective;

	/** 직교 뷰포트용 카메라 변환 데이터 */
	FViewportCameraTransform ViewTransformOrthographic;

	/** 현재 뷰포트의 수평 시야각 (잠긴 카메라 등으로 수정될 수 있음) */
	float ViewFOV;
	/** 저장된 뷰포트 수평 시야각 (ini 파일에 저장됨) */
	float FOVAngle;

	float AspectRatio;

	/**
	 * 이 뷰포트에서 모든 입력이 거부된다면 true입니다.
	 */
	bool bDisableInput;

	/** true이면, 대형 정점 Show Flags가 설정된 경우 선택된 BSP 브러시와 정적 메시의 정점이 그려집니다. */
	bool bDrawVertices;
protected:
	/** 플라이트 카메라 속도 스케일 */
	float FlightCameraSpeedScale;

	// TODO: CameraController
	/** 카메라를 조종하는 에디터 카메라 컨트롤러 객체 (주석 처리됨) */
	//FEditorCameraController* CameraController;

	/* 각 마우스 드래그 시작 시 업데이트됨 */
	uint32 LastMouseX;
	uint32 LastMouseY;

	/** 마지막으로 캐시된 마우스 X 좌표 */
	uint32 CachedMouseX;
	/** 마지막으로 캐시된 마우스 Y 좌표 */
	uint32 CachedMouseY;

	/** 마지막 마우스 위치 (현재 위치로도 사용될 수 있음) */
	int32 CachedLastMouseX = 0;
	int32 CachedLastMouseY = 0;

	// -1, -1이면 설정되지 않음
	FIntPoint CurrentMousePos;

	/**
	 * FMouseDeltaTracker::StartTracking/EndTracking 블록 내에 있을 때 true입니다.
	 */
	bool bIsTracking;

	/**
	 * 사용자가 위젯 핸들을 드래그 중이면 true입니다.
	 */
	bool bDraggingByHandle;

	/** 카메라 잠금 여부 */
	bool bCameraLock;

	/** 카메라가 이동 중인지 여부 */
	bool bIsCameraMoving;

	/** 틱 시작 시 카메라 이동 여부 */
	bool bIsCameraMovingOnTick;

	/** 이 클라이언트가 연결된 에디터 뷰포트 위젯 */
	std::weak_ptr<SEditorViewport> EditorViewportWidget;

	/** 각 에디터 뷰에 대해 조절 가능한 Near 클리핑 평면 (0보다 작으면 GNearClippingPlane 사용) */
	float NearPlane;

	/** 0보다 크면, Far 클리핑 평면을 지정한 거리로 재정의합니다. */
	float FarPlane;

	/** 기본 오비트 회전 값 */
	FQuat DefaultOrbitRotation;
	FVector DefaultOrbitLocation;
	FVector DefaultOrbitZoom;
	FVector DefaultOrbitLookAt;
	/*
	* 사용자가 위젯을 드래그할 때, 카메라 이동 없이 절대 위치 변화에 따른 델타를 계산하기 위해 시작 시점의 뷰를 사용합니다.
	* 같은 뷰를 사용하면 카메라 이동으로 인한 이동 연속성이 깨지지 않습니다.
	*/
	//FSceneView* DragStartView;

	TArray<FIntPoint> CapturedMouseMoves;
};

class FLevelEditorViewportClient : public FEditorViewportClient
{
public:

};