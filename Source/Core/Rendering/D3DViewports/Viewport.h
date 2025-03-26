#pragma once
#include "memory"
#include "optional"
#include "Core/EngineTypes.h"
#include "RenderTarget.h"
#include "Core/Math/IntPoint.h"
#include <Core/Input/PlayerInput.h>

#include "ViewportClient.h"
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
	virtual FCursorReply OnCursorQuery(const FPointer& InPointer) = 0;

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
	virtual FReply OnMouseButtonDown(EMouseButton InMouseButton, const FPointer& InPointer) = 0;

	/**
	 * 마우스 버튼이 뷰포트 내부에서 떼어졌을 때 호출됩니다.
	 *
	 * @param MyGeometry	뷰포트의 위치와 크기에 대한 정보
	 * @param MouseEvent	마우스 이벤트에 대한 정보
	 *
	 * @return 이벤트가 처리되었는지와 시스템이 취할 행동 요청을 포함하여 반환합니다.
	 */
	virtual FReply OnMouseButtonUp(EMouseButton InMouseButton, const FPointer& InPointer) = 0;

	/**
	 * 마우스가 뷰포트 내부로 들어올 때 호출됩니다.
	 *
	 * @param MyGeometry	뷰포트의 위치와 크기에 대한 정보
	 */
	virtual void OnMouseEnter(const FPointer& InPointer) = 0;

	/**
	 * 마우스가 뷰포트에서 나갈 때 호출됩니다.
	 */
	virtual void OnMouseLeave(const FPointer& InPointer) = 0;

	/**
	 * 마우스가 뷰포트 내부에서 움직일 때 호출됩니다.
	 *
	 * @param MyGeometry	뷰포트의 위치와 크기에 대한 정보
	 * @param MouseEvent	마우스 이벤트에 대한 정보
	 *
	 * @return 이벤트가 처리되었는지와 시스템이 취할 행동 요청을 포함하여 반환합니다.
	 */
	virtual FReply OnMouseMove(const FPointer& InPointer) = 0;

	/**
	 * 뷰포트 내부에서 마우스 휠이 사용될 때 호출됩니다.
	 *
	 * @param MyGeometry	뷰포트의 위치와 크기에 대한 정보
	 * @param MouseEvent	마우스 이벤트에 대한 정보
	 *
	 * @return 이벤트가 처리되었는지와 시스템이 취할 행동 요청을 포함하여 반환합니다.
	 */
	virtual FReply OnMouseWheel(const FPointer& InPointer) = 0;

	/**
	 * 뷰포트 내부에서 마우스 버튼 더블클릭이 발생할 때 호출됩니다.
	 *
	 * @param InMyGeometry	뷰포트의 위치와 크기에 대한 정보
	 * @param InMouseEvent	마우스 이벤트에 대한 정보
	 *
	 * @return 이벤트가 처리되었는지와 시스템이 취할 행동 요청을 포함하여 반환합니다.
	 */
	virtual FReply OnMouseButtonDoubleClick(const FPointer& InPointer) = 0;

	/**
	 * 뷰포트 내부에서 키가 눌렸을 때 호출됩니다.
	 *
	 * @param MyGeometry	뷰포트의 위치와 크기에 대한 정보
	 * @param InKeyEvent	키 이벤트에 대한 정보
	 *
	 * @return 이벤트가 처리되었는지와 시스템이 취할 행동 요청을 포함하여 반환합니다.
	 */
	virtual FReply OnKeyDown(const EKeyCode& InKeyCode) = 0;

	/**
	 * 뷰포트 내부에서 키가 떼어졌을 때 호출됩니다.
	 *
	 * @param MyGeometry	뷰포트의 위치와 크기에 대한 정보
	 * @param InKeyEvent	키 이벤트에 대한 정보
	 *
	 * @return 이벤트가 처리되었는지와 시스템이 취할 행동 요청을 포함하여 반환합니다.
	 */
	virtual FReply OnKeyUp(const EKeyCode& InKeyCode) = 0;

	/**
	 * 뷰포트가 포커스를 가진 상태에서 문자 키가 눌렸을 때 호출됩니다.
	 *
	 * @param MyGeometry	뷰포트의 위치와 크기에 대한 정보
	 * @param InCharacterEvent	눌린 문자에 대한 정보
	 *
	 * @return 이벤트가 처리되었는지와 시스템이 취할 행동 요청을 포함하여 반환합니다.
	 */
	virtual FReply OnKeyChar(const EKeyCode& InKeyCode) = 0;

	/**
	 * 뷰포트가 키보드 포커스를 얻었을 때 호출됩니다.
	 *
	 * @param InFocusEvent	포커스를 얻은 원인에 대한 정보
	 */
	virtual FReply OnFocusReceived() = 0;

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
	virtual void OnFocusLost() = 0;

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
	virtual FReply OnViewportActivated() = 0;

	///**
	// * 뷰포트의 최상위 창이 비활성화될 때 호출됩니다.
	// */
	virtual void OnViewportDeactivated() = 0;
};

class FViewportFrame
{
public:

	virtual class FViewport* GetViewport() = 0;
	virtual void ResizeFrame(uint32 NewSizeX, uint32 NewSizeY /*,EWindowMode::Type NewWindowMode*/) = 0;
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

	/**
	 * 뷰포트가 프레임 인터페이스를 가지고 있다면 해당 인터페이스를 반환합니다.
	 * @return 뷰포트 프레임 인터페이스
	 */
	virtual FViewportFrame* GetViewportFrame() = 0;

	/**
	 * 화면 비율이 고정될 때 뷰포트 내부의 뷰 영역을 계산합니다.
	 * 시네마틱 바를 만들 때 사용됩니다.
	 * @param AspectRatio [입력] 고정할 비율
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
	
	// 틱(Tick) 함수를 호출하여 뷰포트를 업데이트합니다.
	virtual void Tick(const FGeometry& AllottedGeometry, float InDeltaTime) override;

	/**
	 * 이 뷰포트가 위치한 실제 창을 리사이즈할 때 호출됩니다.
	 *
	 * @param NewSizeX		뷰포트의 새로운 너비
	 * @param NewSizeY		뷰포트의 새로운 높이
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
	virtual void OnDrawViewport(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, int32 LayerId, bool bParentEnabled) override;
	// 커서 모양을 결정하기 위해 호출됩니다.
	virtual FCursorReply OnCursorQuery(const FPointer& InPointer) override;
	// 커서 매핑을 처리합니다.
	virtual std::optional<std::shared_ptr<SWidget>> OnMapCursor(const FCursorReply& CursorReply) override;
	// 마우스 버튼이 눌렸을 때 호출됩니다.
	virtual FReply OnMouseButtonDown(EMouseButton InMouseButton, const FPointer& InPointer) override;
	// 마우스 버튼이 떼어졌을 때 호출됩니다.
	virtual FReply OnMouseButtonUp(EMouseButton InMouseButton, const FPointer& InPointer) override;
	// 마우스가 뷰포트에 들어올 때 호출됩니다.
	virtual void OnMouseEnter(const FPointer& InPointer) override;
	// 마우스가 뷰포트에서 나갈 때 호출됩니다.
	virtual void OnMouseLeave(const FPointer& InPointer) override;
	// 마우스가 움직일 때 호출됩니다.
	virtual FReply OnMouseMove(const FPointer& InPointer) override;
	// 마우스 휠이 사용될 때 호출됩니다.
	virtual FReply OnMouseWheel(const FPointer& InPointer) override;
	// 마우스 버튼 더블클릭이 발생할 때 호출됩니다.
	virtual FReply OnMouseButtonDoubleClick(const FPointer& InPointer) override;
	// 키가 눌렸을 때 호출됩니다.
	virtual FReply OnKeyDown(const EKeyCode& InKeyCode) override;
	// 키가 떼어졌을 때 호출됩니다.
	virtual FReply OnKeyUp(const EKeyCode& InKeyCode) override;
	// 문자 입력 이벤트가 발생했을 때 호출됩니다.
	virtual FReply OnKeyChar(const EKeyCode& InKeyCode) override;
	// 포커스를 받았을 때 호출됩니다.
	virtual FReply OnFocusReceived() override;
	// 포커스를 잃었을 때 호출됩니다.
	virtual void OnFocusLost() override;
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
	 */
	virtual void ResizeViewport(uint32 NewSizeX, uint32 NewSizeY);

	/**
	 * 마우스 이벤트로부터 캐시된 커서 위치를 업데이트합니다.
	 *
	 * @param InGeometry	로컬 공간으로 변환할 뷰포트의 기하학 정보
	 */
	void UpdateCachedCursorPos(const FGeometry& InGeometry, const FIntPoint& InPosition);

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

