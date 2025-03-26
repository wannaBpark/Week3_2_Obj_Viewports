#pragma once
#include "Viewport.h"
#include "Core/Rendering/Widget/SViewport.h"


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

class FViewportClient
{
public:
	virtual ~FViewportClient() {}
	// 미리 데이터를 로드합니다.
	virtual void Precache() {}
	// 뷰포트를 그립니다. (Canvas는 주석 처리되어 있음)
	virtual void Draw(FViewport* Viewport/*, FCanvas* Canvas*/) {}
	// 이 뷰포트 클라이언트와 연관된 월드를 반환합니다.
	virtual class UWorld* GetWorld() const { return UEngine::Get().GetWorld(); }

	/**
	 * 뷰포트에서 수신한 키 이벤트를 확인합니다.
	 * 만약 뷰포트 클라이언트가 이벤트를 사용한다면, 이벤트를 소모하기 위해 true를 반환해야 합니다.
	 * @return	키 이벤트를 소모하면 true, 그렇지 않으면 false를 반환합니다.
	 */
	virtual bool InputKey(EKeyCode Key);

	/**
	 * 뷰포트에서 수신한 축(axis) 움직임을 확인합니다.
	 * 만약 뷰포트 클라이언트가 해당 움직임을 사용한다면, 이벤트를 소모하기 위해 true를 반환해야 합니다.
	 * @param	Viewport - 축 움직임이 발생한 뷰포트.
	 * @param	Delta - 축 움직임의 변화량.
	 * @param	DeltaTime - 마지막 축 업데이트 이후 경과 시간.
	 * @param	NumSamples - 이 Delta에 기여한 장치 샘플 수 (스무딩 등에 유용).
	 * @param	bGamepad - 게임패드(예: Xbox 컨트롤러)에서 입력이 발생했는지 여부.
	 * @return	축 움직임을 소모하면 true, 그렇지 않으면 false를 반환합니다.
	 */
	virtual bool InputAxis(FViewport* Viewport, const EKeyCode& InKeyCode, float Delta, float DeltaTime, int32 NumSamples = 1, bool bGamepad = false) { return false; }

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

class FEditorViewportClient : public FViewportClient
{
public:
	FEditorViewportClient(const std::weak_ptr<SEditorViewport>& InEditorViewportWidget, bool bInCameraPerspective);

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
	virtual bool InputAxis(FViewport* Viewport, const EKeyCode& InKeyCode, float Delta, float DeltaTime,
	                       int32 NumSamples = 1, bool bGamepad = false) override;

	virtual void ReceivedFocus(FViewport* Viewport) override;
	virtual void MouseEnter(FViewport* Viewport, int32 x, int32 y) override;
	virtual void MouseMove(FViewport* Viewport, int32 x, int32 y) override;
	virtual void MouseLeave(FViewport* Viewport) override;
	virtual EMouseCursor GetCursor(FViewport* Viewport, int32 X, int32 Y) override;
	virtual void CapturedMouseMove(FViewport* InViewport, int32 InMouseX, int32 InMouseY) override;
	virtual void ProcessAccumulatedPointerInput(FViewport* InViewport) override;
	virtual bool IsOrtho() const override;
	virtual void LostFocus(FViewport* Viewport) override;

protected:
	// TODO: InternalInput
	virtual bool Internal_InputKey(const EKeyCode& InKeyCode);
	virtual bool Internal_InputAxis(FViewport* Viewport, const EKeyCode& InKeyCode, float Delta, float DeltaTime, int32 NumSamples = 1, bool bGamepad = false);
	
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

	virtual void Tick(float DeltaSeconds);

	/**
	 * 매 프레임 호출되어 마우스 이동 델타를 기반으로 뷰포트를 업데이트합니다.
	 */
	virtual void UpdateMouseDelta();

	/** 이 뷰포트가 레벨 에디터 뷰포트인 경우 true를 반환합니다. */
	virtual bool IsLevelEditorClient() const { return false; }

	void SetCameraPerspective(const bool bPerspective) { bCameraPerspective = bPerspective; }
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
	

	/** 카메라가 이동하기 직전 또는 멈추기 직전에 호출됩니다. */
	virtual void BeginCameraMovement(bool bHasMovement) {}
	virtual void EndCameraMovement() {}
public:
	static const uint32 MaxCameraSpeeds;

	FViewport* Viewport;
	bool bShowWidget;

	bool bCameraPerspective;
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

	TArray<FIntPoint> CapturedMouseMoves;
};