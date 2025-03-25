#include "Viewport.h"
#include "Core/Engine.h"
#include "Object/World/World.h"
#include "Core/Math/IntPoint.h"
#include "Core/Rendering/Widget/SWindow.h"
#include "Core/Rendering/Widget/SViewport.h"

void FViewport::EnqueueBeginRenderFrame(const bool bShouldPresent)
{
	FViewport* viewport = this;

	// TODO : 렌더 시작
	//ENQUEUE_RENDER_COMMAND(BeginDrawingCommand)(
	//	[Viewport](FRHICommandListImmediate& RHICmdList)
	//	{
	//		Viewport->BeginRenderFrame(RHICmdList);
	//	});
	BeginRenderFrame();
}

void FViewport::EnqueueEndRenderFrame(const bool bLockToVsync, const bool bShouldPresent)
{
	// TODO : 렌더 끝
	//FEndDrawingCommandParams Params = { this, (uint32)bLockToVsync, (uint32)GInputLatencyTimer.GameThreadTrigger, (uint32)(PresentAndStopMovieDelay > 0 ? 0 : bShouldPresent) };
	//ENQUEUE_RENDER_COMMAND(EndDrawingCommand)(
	//	[Params](FRHICommandListImmediate& RHICmdList)
	//	{
	//		ViewportEndDrawing(RHICmdList, Params);
	//	});
}

void FViewport::BeginRenderFrame()
{
	//TODO : 렌더링 시작
	//RHICmdList.BeginDrawingViewport(GetViewportRHI(), FTextureRHIRef());
	UpdateRenderTargetSurfaceRHIToCurrentBackBuffer();
}

void FViewport::EndRenderFrame(bool bPresent, bool bLockToVsync)
{
	// TODO : 렌더링 끝
	//RHICmdList.EnqueueLambda([CurrentFrameCounter = GFrameCounterRenderThread](FRHICommandListImmediate& InRHICmdList)
	//	{
	//		UEngine::SetPresentLatencyMarkerStart(CurrentFrameCounter);
	//	});

	//RHICmdList.EndDrawingViewport(GetViewportRHI(), bPresent, bLockToVsync);

	//RHICmdList.EnqueueLambda([CurrentFrameCounter = GFrameCounterRenderThread](FRHICommandListImmediate& InRHICmdList)
	//	{
	//		UEngine::SetPresentLatencyMarkerEnd(CurrentFrameCounter);
	//	});
}

void FViewport::Draw(bool bShouldPresent)
{
	UWorld* World = UEngine::Get().GetWorld();

	// Ignore reentrant draw calls, since we can only redraw one viewport at a time.
	static bool bReentrant = false;
	if (!bReentrant)
	{
		// if this is a game viewport, and game rendering is disabled, then we don't want to actually draw anything
		//if (World && World->IsGameWorld() && !bIsGameRenderingEnabled)
		//{
		//	// since we aren't drawing the viewport, we still need to update streaming
		//	World->UpdateLevelStreaming();
		//}
	}
	else
	{
		if (SizeX > 0 && SizeY > 0)
		{
			EnqueueBeginRenderFrame(bShouldPresent);

			UWorld* ViewportWorld = ViewportClient->GetWorld();

			// TODO : 렌더링
			//FCanvas Canvas(this, nullptr, ViewportWorld, ViewportWorld ? ViewportWorld->GetFeatureLevel() : GMaxRHIFeatureLevel, FCanvas::CDM_DeferDrawing, ViewportClient->ShouldDPIScaleSceneCanvas() ? ViewportClient->GetDPIScale() : 1.0f);
			//Canvas.SetRenderTargetRect(FIntRect(0, 0, SizeX, SizeY));
			//{
			//	ViewportClient->Draw(this, &Canvas);
			//}
			//Canvas.Flush_GameThread();

			//UGameViewportClient::OnViewportRendered().Broadcast(this);

			//ViewportClient->ProcessScreenShots(this);

			//// Slate doesn't present immediately. Tag the viewport as requiring vsync so that it happens.
			//SetRequiresVsync(bLockToVsync);
			//EnqueueEndRenderFrame(bLockToVsync, bShouldPresent);
		}
	}
}

void FViewport::Invalidate()
{
	//TODO : 예외 걸렸을 때, 취소 로직
}

FIntRect FViewport::CalculateViewExtents(float AspectRatio, const FIntRect& ViewRect)
{
	return CalculateViewExtents(AspectRatio, GetDesiredAspectRatio(), ViewRect, GetSizeXY());
}

FIntRect FViewport::CalculateViewExtents(float AspectRatio, float DesiredAspectRatio, const FIntRect& ViewRect, const FIntPoint& DestSize)
{
	FIntRect Result = ViewRect;

	const float CurrentSizeX = ViewRect.Width();
	const float CurrentSizeY = ViewRect.Height();

	// the viewport's SizeX/SizeY may not always match the DesiredAspectRatio, so adjust the requested AspectRatio to compensate
	const float AdjustedAspectRatio = AspectRatio / (DesiredAspectRatio / ((float)DestSize.X / (float)DestSize.Y));

	// If desired, enforce a particular aspect ratio for the render of the scene. 
	// Results in black bars at top/bottom etc.
	const float AspectRatioDifference = AdjustedAspectRatio - (CurrentSizeX / CurrentSizeY);

	if (FMath::Abs(AspectRatioDifference) > 0.01f)
	{
		// If desired aspect ratio is bigger than current - we need black bars on top and bottom.
		if (AspectRatioDifference > 0.0f)
		{
			// Calculate desired Y size.
			const int32 NewSizeY = FMath::Max(1, static_cast<int32>(std::round(2.0f * CurrentSizeX / AdjustedAspectRatio + 0.5f))/2);
			Result.Min.Y = static_cast<int32_t>(std::round(2.0f * CurrentSizeY - NewSizeY + 0.5f)/2);
			Result.Max.Y = Result.Min.Y + NewSizeY;
			Result.Min.Y += ViewRect.Min.Y;
			Result.Max.Y += ViewRect.Min.Y;
		}
		// Otherwise - will place bars on the sides.
		else
		{
			const int32 NewSizeX = FMath::Max(1, static_cast<int32>(std::round(2.0f * CurrentSizeY / AdjustedAspectRatio + 0.5f)) / 2);
			Result.Min.X = static_cast<int32_t>(std::round(2.0f * CurrentSizeX - NewSizeX + 0.5f) / 2);
			Result.Max.X = Result.Min.X + NewSizeX;
			Result.Min.X += ViewRect.Min.X;
			Result.Max.X += ViewRect.Min.X;
		}
	}

	return Result;
}

void FViewport::UpdateRenderTargetSurfaceRHIToCurrentBackBuffer()
{
	//if (IsValidRef(ViewportRHI))
	//{
	//	RenderTargetTextureRHI = RHIGetViewportBackBuffer(ViewportRHI);
	//}
}

void FViewport::SetInitialSize(FIntPoint InitialSizeXY)
{
	// Initial size only works if the viewport has not yet been resized
	if (GetSizeXY() == FIntPoint::ZeroValue)
	{
		//UpdateViewportRHI(false, InitialSizeXY.X, InitialSizeXY.Y, EWindowMode::Windowed, PF_Unknown);
	}
}

void FViewport::ReleaseRHI()
{
	//HitProxyMap.Release();
	//RenderTargetTextureRHI.SafeRelease();
	//ViewportRHI.SafeRelease();
}

void FViewport::InitRHI()
{
	//if (bRequiresHitProxyStorage)
	//{
	//	// Initialize the hit proxy map.
	//	HitProxyMap.Init(SizeX, SizeY);
	//}
}

void FViewportClient::RedrawRequested(FViewport* Viewport)
{
	Viewport->Draw();
}

void FViewportClient::RequestInvalidateHitProxy(FViewport* Viewport)
{
	//Viewport->InvalidateHitProxy();
}

bool FViewportClient::InputKey()
{
	return true;
	//return InputKey(EventArgs.Viewport, EventArgs.ControllerId, EventArgs.Key, EventArgs.Event, EventArgs.AmountDepressed, EventArgs.Key.IsGamepadKey());
}

FViewport::FViewport(FViewportClient* InViewportClient) :
	ViewportClient(InViewportClient),
	InitialPositionX(0),
	InitialPositionY(0),
	SizeX(0),
	SizeY(0),
	bIsSlateViewport(false)
{
	bIsPlayInEditorViewport = false;
}

FViewport::~FViewport()
{
}

std::optional<std::shared_ptr<SWidget>> FViewportClient::MapCursor(FViewport* Viewport, const FCursorReply& CursorReply)
{
	return std::optional<std::shared_ptr<SWidget>>();
}

FSceneViewport::FSceneViewport(FViewportClient* InViewportClient, std::shared_ptr<SViewport> InViewportWidget)
	: FViewport(InViewportClient)
	, CurrentReplyState(FReply())
	, CachedCursorPos(-1, -1)
	, PreCaptureCursorPos(-1, -1)
	, SoftwareCursorPosition(FVector2D(0, 0))
	, bIsSoftwareCursorVisible(false)
	, ViewportWidget(InViewportWidget)
	, NumMouseSamplesX(0)
	, NumMouseSamplesY(0)
	, MouseDelta(0, 0)
	, bIsCursorVisible(true)
	, bShouldCaptureMouseOnActivate(true)
	, bUseSeparateRenderTarget(InViewportWidget != nullptr ? !InViewportWidget->ShouldRenderDirectly() : true)
	, bForceSeparateRenderTarget(false)
	, bIsResizing(false)
	, bForceViewportSize(false)
	, MousePosBeforeHiddenDueToCapture(-1, -1)
{
	bIsSlateViewport = true;
	//RenderThreadSlateTexture = new FSlateRenderTargetRHI(nullptr, 0, 0);

	if (InViewportClient)
	{
		bShouldCaptureMouseOnActivate = InViewportClient->CaptureMouseOnLaunch();
	}

	//if (FSlateApplication::IsInitialized())
	//{
	//	FSlateApplication::Get().GetRenderer()->OnSlateWindowDestroyed().AddRaw(this, &FSceneViewport::OnWindowBackBufferResourceDestroyed);
	//	FSlateApplication::Get().GetRenderer()->OnPreResizeWindowBackBuffer().AddRaw(this, &FSceneViewport::OnPreResizeWindowBackbuffer);
	//	FSlateApplication::Get().GetRenderer()->OnPostResizeWindowBackBuffer().AddRaw(this, &FSceneViewport::OnPostResizeWindowBackbuffer);
	//}
}

FSceneViewport::~FSceneViewport()
{
	Destroy();
}

bool FSceneViewport::HasMouseCapture() const
{
	std::shared_ptr<SViewport> viewportWidget = ViewportWidget.lock();
	if (viewportWidget == nullptr)
	{
		return false;
	}

	return viewportWidget->HasMouseCapture();
}

bool FSceneViewport::HasFocus() const
{
	return true;
	//return FSlateApplication::Get().GetUserFocusedWidget(0) == ViewportWidget.Pin();
}

bool FSceneViewport::IsForegroundWindow() const
{
	return false;
}

void FSceneViewport::CaptureMouse(bool bCapture)
{
	//if (bCapture)
	//{
	//	CurrentReplyState.UseHighPrecisionMouseMovement(ViewportWidget.lock());
	//}
	//else
	//{
	//	CurrentReplyState.ReleaseMouseCapture();
	//}
}

void FSceneViewport::LockMouseToViewport(bool bLock)
{
	if (bLock)
	{
		CurrentReplyState.LockMouseToWidget(static_pointer_cast<SWidget>(ViewportWidget.lock()));
	}
	else
	{
		CurrentReplyState.ReleaseMouseLock();
	}
}

void FSceneViewport::ShowCursor(bool bVisible)
{
	if (bVisible && !bIsCursorVisible)
	{
		if (bIsSoftwareCursorVisible)
		{
			const int32 ClampedMouseX = FMath::Clamp<int32>(SoftwareCursorPosition.X / CachedGeometry.GetAbsoluteScale(), 0, SizeX);
			const int32 ClampedMouseY = FMath::Clamp<int32>(SoftwareCursorPosition.Y / CachedGeometry.GetAbsoluteScale(), 0, SizeY);
			
			const FVector2D localToAbsolute = CachedGeometry.LocalToAbsolute(FVector2D(ClampedMouseX, ClampedMouseY));
			FIntPoint mousePos = FIntPoint(localToAbsolute.X, localToAbsolute.Y);
			CurrentReplyState.SetMousePos(mousePos);
		}
		else
		{
			// Restore the old mouse position when we show the cursor.
			CurrentReplyState.SetMousePos(PreCaptureCursorPos);
		}

		SetPreCaptureMousePosFromSlateCursor();
		bIsCursorVisible = true;
	}
	else if (!bVisible && bIsCursorVisible)
	{
		// Remember the current mouse position when we hide the cursor.
		SetPreCaptureMousePosFromSlateCursor();
		bIsCursorVisible = false;
	}
}

void FSceneViewport::SetPreCaptureMousePosFromSlateCursor()
{
	// TODO : 마우스 위치 읽어오기
	//PreCaptureCursorPos = FSlateApplication::Get().GetCursorPos().IntPoint();
}

void FSceneViewport::EnqueueEndRenderFrame(const bool bLockToVsync, const bool bShouldPresent)
{
	FViewport::EnqueueEndRenderFrame(bLockToVsync, bShouldPresent);

	//if (DebugCanvasDrawer->GetGameThreadDebugCanvas() && DebugCanvasDrawer->GetGameThreadDebugCanvas()->HasBatchesToRender() && DebugCanvas.IsValid())
	//{
	//	DebugCanvas.Pin()->Invalidate(EInvalidateWidget::Paint);
	//}
}

//bool FSceneViewport::KeyState(FKey Key) const
//{
//	//return KeyStateMap.FindRef(Key);
//}

int32 FSceneViewport::GetMouseX() const
{
	return CachedCursorPos.X;
}

int32 FSceneViewport::GetMouseY() const
{
	return CachedCursorPos.Y;
}

void FSceneViewport::GetMousePos(FIntPoint& MousePosition, const bool bLocalPosition)
{
	if (bLocalPosition)
	{
		MousePosition = CachedCursorPos;
	}
	else
	{
		const FVector2D AbsoluteMousePos = CachedGeometry.LocalToAbsolute(FVector2D(CachedCursorPos.X / CachedGeometry.GetAbsoluteScale(), CachedCursorPos.Y / CachedGeometry.GetAbsoluteScale()));
		MousePosition.X = AbsoluteMousePos.X;
		MousePosition.Y = AbsoluteMousePos.Y;
	}
}

void FSceneViewport::SetMouse(int32 X, int32 Y)
{
	const FVector2D NormalizedLocalMousePosition = FVector2D(X / GetSizeXY().X, Y / GetSizeXY().Y) ;
	FVector2D AbsolutePos = CachedGeometry.LocalToAbsolute(FVector2D(NormalizedLocalMousePosition.X * CachedGeometry.GetLocalSize().X, NormalizedLocalMousePosition.Y * CachedGeometry.GetLocalSize().Y));
	//FSlateApplication::Get().SetCursorPos(AbsolutePos.RoundToVector());
	CachedCursorPos = FIntPoint(X, Y);
}

void FSceneViewport::ProcessInput(float DeltaTime)
{
	// Required 
}

void FSceneViewport::EnqueueBeginRenderFrame(const bool bShouldPresent)
{
	// Rendering
	//if (BufferedRenderTargetsRHI[CurrentBufferedTargetIndex])
	//{
	//	RenderTargetTextureRHI = BufferedRenderTargetsRHI[CurrentBufferedTargetIndex];
	//}

	//DebugCanvasDrawer->InitDebugCanvas(GetClient(), GetClient()->GetWorld());

	//if (!IsValidRef(ViewportRHI))
	//{
	//	// Get the viewport for this window from the renderer so we can render directly to the backbuffer
	//	FSlateRenderer* Renderer = FSlateApplication::Get().GetRenderer();
	//	if (ViewportWidget.IsValid())
	//	{
	//		auto WidgetWindow = FSlateApplication::Get().FindWidgetWindow(ViewportWidget.Pin().ToSharedRef());
	//		if (WidgetWindow.IsValid())
	//		{
	//			void* ViewportResource = Renderer->GetViewportResource(*WidgetWindow);
	//			if (ViewportResource)
	//			{
	//				ViewportRHI = *((FViewportRHIRef*)ViewportResource);
	//			}
	//		}
	//	}
	//}

	//ENQUEUE_RENDER_COMMAND(SetRenderThreadViewportTarget)(
	//	[Viewport = this, RT = RenderTargetTextureRHI](FRHICommandListImmediate& RHICmdList) mutable
	//	{
	//		Viewport->SetRenderTargetTextureRenderThread(RT);
	//	});

	//FViewport::EnqueueBeginRenderFrame(bShouldPresent);
}

void FSceneViewport::Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float InDeltaTime)
{
	UpdateCachedGeometry(AllottedGeometry);
	ProcessInput(InDeltaTime);

	// TODO : Tick Logic
	//if (IsValidRef(ViewportRHI))
	//{
	//	ViewportRHI->Tick(DeltaTime);
	//}

	//if (ViewportClient && ViewportClient->GetWorld() && ViewportClient->GetWorld()->Scene)
	//{
	//	FSlateApplication::Get().GetRenderer()->RegisterCurrentScene(ViewportClient->GetWorld()->Scene);
	//}
	//else
	//{
	//	FSlateApplication::Get().GetRenderer()->RegisterCurrentScene(nullptr);
	//}
}

void FSceneViewport::ResizeFrame(uint32 NewSizeX, uint32 NewSizeY)
{
	// Resizing the window directly is only supported in the game
	if (/*FApp::IsGame() && FApp::CanEverRender() &&*/ NewSizeX > 0 && NewSizeY > 0)
	{
		// TODO : 
		//std::shared_ptr<SWindow> WindowToResize = FSlateApplication::Get().FindWidgetWindow(ViewportWidget.lock());

		//if (WindowToResize != nullptr)
		//{
		//	//NewWindowMode = GetWindowModeType(NewWindowMode);

		//	const FVector2D OldWindowPos = WindowToResize->GetPositionInScreen();
		//	const FVector2D OldWindowSize = WindowToResize->GetClientSizeInScreen();
		//	//const EWindowMode::Type OldWindowMode = WindowToResize->GetWindowMode();

		//	// Set the new window mode first to ensure that the work area size is correct (fullscreen windows can affect this)
		//	//if (NewWindowMode != OldWindowMode)
		//	//{
		//	//	WindowToResize->SetWindowMode(NewWindowMode);
		//	//	WindowMode = NewWindowMode;
		//	//}

		//	std::optional<FVector2D> NewWindowPos;
		//	FVector2D NewWindowSize(NewSizeX, NewSizeY);

		//	// Only adjust window size if not in off-screen rendering mode, because off-screen rendering skips rendering to screen and uses custom size.
		//	if (/*!FSlateApplication::Get().IsRenderingOffScreen()*/true)
		//	{
		//		// TODO : 
		//		//const FSlateRect BestWorkArea = FSlateApplication::Get().GetWorkArea(FSlateRect::FromPointAndExtent(OldWindowPos, OldWindowSize));

		//		// A switch to window mode should position the window to be in the center of the work-area (we don't do this if we were already in window mode to allow the user to move the window)
		//		// Fullscreen modes should position the window to the top-left of the monitor.
		//		// If we're going into windowed fullscreen mode, we always want the window to fill the entire screen.
		//		// When we calculate the scene view, we'll check the fullscreen mode and configure the screen percentage
		//		// scaling so we actual render to the resolution we've been asked for.
		//		//if (/*OldWindowMode == EWindowMode::Windowed &&*/ NewWindowSize == OldWindowSize)
		//		//{
		//		//	// Leave the window position alone!
		//		//	NewWindowPos.reset();
		//		//}
		//		//else
		//		//{
		//		//	const FVector2D BestWorkAreaTopLeft = BestWorkArea.GetTopLeft();
		//		//	const FVector2D BestWorkAreaSize = BestWorkArea.GetSize();

		//		//	FVector2D CenteredWindowPos = BestWorkAreaTopLeft;

		//		//	if (NewWindowSize.X < BestWorkAreaSize.X)
		//		//	{
		//		//		CenteredWindowPos.X += FMath::Max(0.0f, (BestWorkAreaSize.X - NewWindowSize.X) * 0.5f);
		//		//	}

		//		//	if (NewWindowSize.Y < BestWorkAreaSize.Y)
		//		//	{
		//		//		CenteredWindowPos.Y += FMath::Max(0.0f, (BestWorkAreaSize.Y - NewWindowSize.Y) * 0.5f);
		//		//	}

		//		//	NewWindowPos = CenteredWindowPos;
		//		//}
		//	}
		//	else
		//	{
		//		NewWindowPos = FVector2D(0.0f, 0.0f);
		//	}

		//	// Resize window
		//	const bool bSizeChanged = NewWindowSize != OldWindowSize;
		//	const bool bPositionChanged = NewWindowPos.has_value() && NewWindowPos != OldWindowPos;
		//	//const bool bModeChanged = NewWindowMode != OldWindowMode;
		//	if (bSizeChanged || bPositionChanged /*|| bModeChanged*/)
		//	{
		//		if (CurrentReplyState.ShouldReleaseMouseLock())
		//		{
		//			LockMouseToViewport(false);
		//		}

		//		if (/*bModeChanged || */(bSizeChanged && bPositionChanged))
		//		{
		//			WindowToResize->ReshapeWindow(NewWindowPos.value(), NewWindowSize);
		//		}
		//		else if (bSizeChanged)
		//		{
		//			WindowToResize->Resize(NewWindowSize);
		//		}
		//		else
		//		{
		//			WindowToResize->MoveWindowTo(NewWindowPos.value());
		//		}
		//	}

		//	// Resize viewport
		//	FVector2D ViewportSize = WindowToResize->GetWindowSizeFromClientSize(FVector2D(SizeX, SizeY));
		//	FVector2D NewViewportSize = WindowToResize->GetViewportSize();

		//	// Resize backbuffer
		//	FVector2D BackBufferSize = ViewportSize;
		//	FVector2D NewBackbufferSize = NewViewportSize;

		//	if (NewViewportSize != ViewportSize /*|| NewWindowMode != OldWindowMode*/)
		//	{
		//		// TODO : 
		//		//FSlateApplicationBase::Get().GetRenderer()->UpdateFullscreenState(WindowToResize.ToSharedRef(), NewBackbufferSize.X, NewBackbufferSize.Y);
		//		ResizeViewport(NewViewportSize.X, NewViewportSize.Y/*, NewWindowMode*/);
		//	}


		//	if (NewBackbufferSize != BackBufferSize)
		//	{
		//		// TODO : 
		//		//FSlateApplicationBase::Get().GetRenderer()->UpdateFullscreenState(WindowToResize.ToSharedRef(), NewBackbufferSize.X, NewBackbufferSize.Y);
		//	}

		//	// TODO : 
		//	//UCanvas::UpdateAllCanvasSafeZoneData();
		//}
	}
}

void FSceneViewport::OnDrawViewport(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect/*, FSlateWindowElementList& OutDrawElements*/, int32 LayerId, bool bParentEnabled)
{
	if (!bForceViewportSize)
	{
		FVector2D TopLeft = AllottedGeometry.GetAbsolutePosition();
		FVector2D BottomRight = TopLeft + AllottedGeometry.GetDrawSize();
		FIntPoint DrawSize = FIntPoint(FMath::RoundToInt32(BottomRight.X) - FMath::RoundToInt32(TopLeft.X), FMath::RoundToInt32(BottomRight.Y) - FMath::RoundToInt32(TopLeft.Y));
		
		if (GetSizeXY() != DrawSize)
		{
			//TODO :: Resize?
			//TSharedPtr<SWindow> Window = FSlateApplication::Get().FindWidgetWindow(ViewportWidget.Pin().ToSharedRef());
			//ResizeViewport(FMath::Max(0, DrawSize.X), FMath::Max(0, DrawSize.Y), Window->GetWindowMode());
		}
	}
}

FCursorReply FSceneViewport::OnCursorQuery(const FGeometry& MyGeometry/*, const FPointerEvent& CursorEvent*/)
{
	EMouseCursor MouseCursorToUse = EMouseCursor::Default;
	if (ViewportClient && GetSizeXY() != FIntPoint::ZeroValue)
	{
		const int32 MouseX = GetMouseX();
		const int32 MouseY = GetMouseY();
		MouseCursorToUse = ViewportClient->GetCursor(this, MouseX, MouseY);
	}

	return FCursorReply::Cursor(MouseCursorToUse);
}

std::optional<std::shared_ptr<SWidget>> FSceneViewport::OnMapCursor(const FCursorReply& CursorReply)
{
	if (ViewportClient && GetSizeXY() != FIntPoint::ZeroValue)
	{
		return ViewportClient->MapCursor(this, CursorReply);
	}
	return ISlateViewport::OnMapCursor(CursorReply);
}

FReply FSceneViewport::OnMouseButtonDown(const FGeometry& InGeometry/*, const FPointerEvent& MouseEvent*/)
{
	UpdateCachedGeometry(InGeometry);
	UpdateCachedCursorPos(InGeometry);

	if (ViewportClient && GetSizeXY() != FIntPoint::ZeroValue)
	{
		if (!HasFocus())
		{
			// TODO : 
			/*FModifierKeysState KeysState = FSlateApplication::Get().GetModifierKeys();
			ApplyModifierKeys(KeysState);*/
		}

		
	}

	return FReply();
}

FReply FSceneViewport::OnMouseButtonUp(const FGeometry& InGeometry/*, const FPointerEvent& MouseEvent*/)
{
	UpdateCachedGeometry(InGeometry);
	UpdateCachedCursorPos(InGeometry);

	if (ViewportClient && GetSizeXY() != FIntPoint::ZeroValue)
	{
		// TODO : 
		//bCursorVisible = ViewportClient->GetCursor(this, GetMouseX(), GetMouseY()) != EMouseCursor::None;
		
	
	}
	return FReply();
}

void FSceneViewport::OnMouseEnter(const FGeometry& MyGeometry/*, const FPointerEvent& MouseEvent*/)
{
	UpdateCachedCursorPos(MyGeometry);
	ViewportClient->MouseEnter(this, GetMouseX(), GetMouseY());
}

void FSceneViewport::OnMouseLeave(/*const FPointerEvent& MouseEvent*/)
{
	if (ViewportClient)
	{
		ViewportClient->MouseLeave(this);

		if (IsCurrentlyGameViewport())
		{
			CachedCursorPos = FIntPoint(-1, -1);
		}
	}
}

FReply FSceneViewport::OnMouseMove(const FGeometry& InGeometry/*, const FPointerEvent& MouseEvent*/)
{
	UpdateCachedGeometry(InGeometry);
	UpdateCachedCursorPos(InGeometry);

	const bool bViewportHasCapture = ViewportWidget.lock() != nullptr && ViewportWidget.lock()->HasMouseCapture();
	if (ViewportClient && GetSizeXY() != FIntPoint::ZeroValue)
	{
		if (bViewportHasCapture)
		{
			ViewportClient->CapturedMouseMove(this, GetMouseX(), GetMouseY());
		}
		else
		{
			ViewportClient->MouseMove(this, GetMouseX(), GetMouseY());
		}

		if (bViewportHasCapture)
		{
			// TODO : 
			// Accumulate delta changes to mouse movement.  Depending on the sample frequency of a mouse we may get many per frame.
			//@todo Slate: In directinput, number of samples in x/y could be different...
			//const FVector2D CursorDelta = InMouseEvent.GetCursorDelta();
			//MouseDelta.X += CursorDelta.X;
			++NumMouseSamplesX;

			//MouseDelta.Y -= CursorDelta.Y;
			++NumMouseSamplesY;
		}
	}

	return FReply();
}

FReply FSceneViewport::OnMouseWheel(const FGeometry& InGeometry/*, const FPointerEvent& MouseEvent*/)
{
	UpdateCachedGeometry(InGeometry);
	UpdateCachedCursorPos(InGeometry);

	if (ViewportClient && GetSizeXY() != FIntPoint::ZeroValue)
	{
		// TODO : 
		// Switch to the viewport clients world before processing input
		//FScopedConditionalWorldSwitcher WorldSwitcher(ViewportClient);

		// The viewport client accepts two different keys depending on the direction of scroll.  
		//FKey const ViewportClientKey = InMouseEvent.GetWheelDelta() < 0 ? EKeys::MouseScrollDown : EKeys::MouseScrollUp;

		// Pressed and released should be sent
		//ViewportClient->InputKey(FInputKeyEventArgs(this, InMouseEvent.GetUserIndex(), ViewportClientKey, IE_Pressed, 1.0f, InMouseEvent.IsTouchEvent()));
		//ViewportClient->InputKey(FInputKeyEventArgs(this, InMouseEvent.GetUserIndex(), ViewportClientKey, IE_Released, 1.0f, InMouseEvent.IsTouchEvent()));
		//ViewportClient->InputAxis(this, InMouseEvent.GetInputDeviceId(), EKeys::MouseWheelAxis, InMouseEvent.GetWheelDelta(), FApp::GetDeltaTime());
	}
	return CurrentReplyState;
}

FReply FSceneViewport::OnMouseButtonDoubleClick(const FGeometry& InGeometry/*, const FPointerEvent& InMouseEvent*/)
{
	// TODO : 
	// Start a new reply state
	//CurrentReplyState = FReply::Handled();

	// Note: When double-clicking, the following message sequence is sent:
	//	WM_*BUTTONDOWN
	//	WM_*BUTTONUP
	//	WM_*BUTTONDBLCLK	(Needs to set the KeyStates[*] to true)
	//	WM_*BUTTONUP
	//KeyStateMap.Add(InMouseEvent.GetEffectingButton(), true);

	UpdateCachedGeometry(InGeometry);
	UpdateCachedCursorPos(InGeometry);

	if (ViewportClient && GetSizeXY() != FIntPoint::ZeroValue)
	{
		// TODO : 
		// Switch to the viewport clients world before processing input
		//FScopedConditionalWorldSwitcher WorldSwitcher(ViewportClient);

		/*if (!ViewportClient->InputKey(FInputKeyEventArgs(this, InMouseEvent.GetUserIndex(), InMouseEvent.GetEffectingButton(), IE_DoubleClick, 1.0f, InMouseEvent.IsTouchEvent())))
		{
			CurrentReplyState = FReply::Unhandled();
		}*/
	}
	return CurrentReplyState;
}

FReply FSceneViewport::OnKeyDown(const FGeometry& InGeometry/*, const FKeyEvent& InKeyEvent*/)
{
	// TODO : 
	//// Start a new reply state
	//CurrentReplyState = FReply::Handled();

	//FKey Key = InKeyEvent.GetKey();
	//if (Key.IsValid())
	//{
	//	KeyStateMap.Add(Key, true);

	//	//@todo Slate Viewports: FWindowsViewport checks for Alt+Enter or F11 and toggles fullscreen.  Unknown if fullscreen via this method will be needed for slate viewports. 
	//	if (ViewportClient && GetSizeXY() != FIntPoint::ZeroValue)
	//	{
	//		// Switch to the viewport clients world before processing input
	//		FScopedConditionalWorldSwitcher WorldSwitcher(ViewportClient);

	//		if (!ViewportClient->InputKey(FInputKeyEventArgs(this, InKeyEvent.GetInputDeviceId(), Key, InKeyEvent.IsRepeat() ? IE_Repeat : IE_Pressed, 1.0f, false)))
	//		{
	//			CurrentReplyState = FReply::Unhandled();
	//		}
	//	}
	//}
	//else
	//{
	//	CurrentReplyState = FReply::Unhandled();
	//}
	//return CurrentReplyState;
	return FReply();
}

FReply FSceneViewport::OnKeyUp(const FGeometry& InGeometry/*, const FKeyEvent& InKeyEvent*/)
{
	// TODO : 
	//// Start a new reply state
	//CurrentReplyState = FReply::Handled();

	//FKey Key = InKeyEvent.GetKey();
	//if (Key.IsValid())
	//{
	//	KeyStateMap.Add(Key, false);

	//	if (ViewportClient && GetSizeXY() != FIntPoint::ZeroValue)
	//	{
	//		// Switch to the viewport clients world before processing input
	//		FScopedConditionalWorldSwitcher WorldSwitcher(ViewportClient);

	//		if (!ViewportClient->InputKey(FInputKeyEventArgs(this, InKeyEvent.GetInputDeviceId(), Key, IE_Released, 1.0f, false)))
	//		{
	//			CurrentReplyState = FReply::Unhandled();
	//		}
	//	}
	//}
	//else
	//{
	//	CurrentReplyState = FReply::Unhandled();
	//}

	//return CurrentReplyState;

	return FReply();
}

FReply FSceneViewport::OnKeyChar(const FGeometry& InGeometry/*, const FCharacterEvent& InCharacterEvent*/)
{
	// TODO : 
	//// Start a new reply state
	//CurrentReplyState = FReply::Handled();

	//if (ViewportClient && GetSizeXY() != FIntPoint::ZeroValue)
	//{
	//	// Switch to the viewport clients world before processing input
	//	FScopedConditionalWorldSwitcher WorldSwitcher(ViewportClient);

	//	if (!ViewportClient->InputChar(this, InCharacterEvent.GetUserIndex(), InCharacterEvent.GetCharacter()))
	//	{
	//		CurrentReplyState = FReply::Unhandled();
	//	}
	//}
	//return CurrentReplyState;
	return FReply();
}

FReply FSceneViewport::OnFocusReceived(/*const FFocusEvent& InFocusEvent*/)
{
	// TODO : 
	//CurrentReplyState = FReply::Handled();

	//if (InFocusEvent.GetUser() == FSlateApplication::Get().GetUserIndexForKeyboard())
	//{
	//	if (ViewportClient != nullptr)
	//	{
	//		FScopedConditionalWorldSwitcher WorldSwitcher(ViewportClient);
	//		ViewportClient->ReceivedFocus(this);
	//	}

	//	// Update key state mappings so that the the viewport modifier states are valid upon focus.
	//	const FModifierKeysState KeysState = FSlateApplication::Get().GetModifierKeys();
	//	KeyStateMap.Add(EKeys::LeftAlt, KeysState.IsLeftAltDown());
	//	KeyStateMap.Add(EKeys::RightAlt, KeysState.IsRightAltDown());
	//	KeyStateMap.Add(EKeys::LeftControl, KeysState.IsLeftControlDown());
	//	KeyStateMap.Add(EKeys::RightControl, KeysState.IsRightControlDown());
	//	KeyStateMap.Add(EKeys::LeftShift, KeysState.IsLeftShiftDown());
	//	KeyStateMap.Add(EKeys::RightShift, KeysState.IsRightShiftDown());
	//	KeyStateMap.Add(EKeys::LeftCommand, KeysState.IsLeftCommandDown());
	//	KeyStateMap.Add(EKeys::RightCommand, KeysState.IsRightCommandDown());


	//	if (IsCurrentlyGameViewport())
	//	{
	//		FSlateApplication& SlateApp = FSlateApplication::Get();

	//		const bool bPermanentCapture = (!GIsEditor || InFocusEvent.GetCause() == EFocusCause::Mouse) &&
	//			(ViewportClient != nullptr) &&
	//			((ViewportClient->GetMouseCaptureMode() == EMouseCaptureMode::CapturePermanently) ||
	//				(ViewportClient->GetMouseCaptureMode() == EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown)
	//				);

	//		// if bPermanentCapture is true, then ViewportClient != nullptr, so it's ok to dereference it.  But the permanent capture must be tested first.
	//		if (SlateApp.IsActive() && bPermanentCapture && !ViewportClient->IgnoreInput())
	//		{
	//			TSharedRef<SViewport> ViewportWidgetRef = ViewportWidget.Pin().ToSharedRef();

	//			FWidgetPath PathToWidget;
	//			SlateApp.GeneratePathToWidgetUnchecked(ViewportWidgetRef, PathToWidget);

	//			return AcquireFocusAndCapture(GetSizeXY() / 2, EFocusCause::Mouse);
	//		}
	//	}
	//}

	//return CurrentReplyState;
	return FReply();
}

void FSceneViewport::OnFocusLost()
{
	// TODO:
	//// If the focus loss event isn't the for the primary 'keyboard' user, don't worry about it.
	//if (InFocusEvent.GetUser() != FSlateApplication::Get().GetUserIndexForKeyboard())
	//{
	//	return;
	//}

	//bShouldCaptureMouseOnActivate = false;
	//bCursorHiddenDueToCapture = false;
	//KeyStateMap.Empty();
	//if (ViewportClient != nullptr)
	//{
	//	FScopedConditionalWorldSwitcher WorldSwitcher(ViewportClient);
	//	ViewportClient->LostFocus(this);
	//}
}

void FSceneViewport::OnViewportClosed()
{
	if (ViewportClient)
	{
		//TODO :
		//FScopedConditionalWorldSwitcher WorldSwitcher(ViewportClient);
		ViewportClient->CloseRequested(this);
	}
}

FReply FSceneViewport::OnRequestWindowClose()
{
	//TODO:
	//return (ViewportClient && !ViewportClient->WindowCloseRequested()) ? FReply::Handled() : FReply::Unhandled();
	return FReply();
}

std::weak_ptr<SWidget> FSceneViewport::GetWidget()
{
	return GetViewportWidget();
}

FReply FSceneViewport::OnViewportActivated(/*const FWindowActivateEvent& InActivateEvent*/)
{
	// TODO:
	//if (ViewportClient != nullptr)
	//{
	//	FScopedConditionalWorldSwitcher WorldSwitcher(ViewportClient);
	//	ViewportClient->Activated(this, InActivateEvent);

	//	// Determine if we're in permanent capture mode.  This cannot be cached as part of bShouldCaptureMouseOnActivate because it could change between window activate and deactivate
	//	const bool bPermanentCapture = ViewportClient->IsInPermanentCapture();


	//	// If we are activating and had Mouse Capture on deactivate then we should get focus again
	//	// It's important to note in the case of:
	//	//    InActivateEvent.ActivationType == FWindowActivateEvent::EA_ActivateByMouse
	//	// we do NOT acquire focus the reasoning is that the click itself will give us a chance on Mouse down to get capture.
	//	// This also means we don't go and grab capture in situations like:
	//	//    - the user clicked on the application header
	//	//    - the user clicked on some UI
	//	//    - the user clicked in our window but not an area our viewport covers.
	//	if (InActivateEvent.GetActivationType() == FWindowActivateEvent::EA_Activate && (bShouldCaptureMouseOnActivate || bPermanentCapture))
	//	{
	//		return AcquireFocusAndCapture(GetSizeXY() / 2, EFocusCause::WindowActivate);
	//	}
	//}

	return FReply();
}

void FSceneViewport::OnViewportDeactivated(/*const FWindowActivateEvent& InActivateEvent*/)
{
	// TODO:
	//bShouldCaptureMouseOnActivate = !GIsEditor && (bShouldCaptureMouseOnActivate || HasMouseCapture());

	//KeyStateMap.Empty();
	//if (ViewportClient != nullptr)
	//{
	//	FScopedConditionalWorldSwitcher WorldSwitcher(ViewportClient);
	//	ViewportClient->Deactivated(this, InActivateEvent);
	//}
}

void FSceneViewport::SetViewportSize(uint32 NewSizeX, uint32 NewSizeY)
{
	// TODO : 
	//std::shared_ptr<SWindow> Window = FSlateApplication::Get().FindWidgetWindow(ViewportWidget.Pin().ToSharedRef());
	//if (Window.IsValid())
	//{
	//	Window->SetIndependentViewportSize(FVector2D(NewViewportSizeX, NewViewportSizeY));
	//	const FVector2D vp = Window->IsMirrorWindow() ? Window->GetSizeInScreen() : Window->GetViewportSize();
	//	FSlateApplicationBase::Get().GetRenderer()->UpdateFullscreenState(Window.ToSharedRef(), vp.X, vp.Y);
	//	ResizeViewport(NewViewportSizeX, NewViewportSizeY, Window->GetWindowMode());
	//}
}

void FSceneViewport::SetFixedViewportSize(uint32 NewSizeX, uint32 NewSizeY)
{
	if (ViewportWidget.lock() != nullptr)
	{
		if (NewSizeX > 0 && NewSizeY > 0)
		{
			bForceViewportSize = true;
			//std::shared_ptr<SWindow> Window = FSlateApplication::Get().FindWidgetWindow(ViewportWidget.Pin().ToSharedRef());
			//if (Window.IsValid())
			//{
				//ResizeViewport(NewViewportSizeX, NewViewportSizeY, Window->GetWindowMode());
			//}
		}
		else
		{
			bForceViewportSize = false;
			//std::shared_ptr<SWindow> Window = FSlateApplication::Get().FindWidgetWindow(ViewportWidget.Pin().ToSharedRef());
			//if (Window.IsValid())
			{
				//Window->Invalidate(EInvalidateWidget::PaintAndVolatility);
			}
		}
	}
}

bool FSceneViewport::HasFixedSize() const
{
	return bForceViewportSize;
}

std::shared_ptr<class SWindow> FSceneViewport::FindSWindow()
{
	if (ViewportWidget.lock() !=nullptr)
	{
		std::shared_ptr<SViewport> PinnedViewportWidget = ViewportWidget.lock();
		//return FSlateApplication::Get().FindWidgetWindow(PinnedViewportWidget.ToSharedRef());
	}

	return std::shared_ptr<SWindow>();
}

void FSceneViewport::Destroy()
{
	delete ViewportClient;
	ViewportClient = nullptr;
}

void FSceneViewport::ResizeViewport(uint32 NewSizeX, uint32 NewSizeY)
{
	// TODO : ResizeVeiwport
}

void FSceneViewport::UpdateCachedCursorPos(const FGeometry& InGeometry)
{
	// TODO : UpdateCursorPos
	//FVector2D LocalPixelMousePos = InGeometry.AbsoluteToLocal(GetScreenSpacePosition());
	//LocalPixelMousePos.X = FMath::Clamp(LocalPixelMousePos.X * CachedGeometry.Scale, (double)TNumericLimits<int32>::Min(), (double)TNumericLimits<int32>::Max());
	//LocalPixelMousePos.Y = FMath::Clamp(LocalPixelMousePos.Y * CachedGeometry.Scale, (double)TNumericLimits<int32>::Min(), (double)TNumericLimits<int32>::Max());

	//CachedCursorPos = LocalPixelMousePos.IntPoint();
}

void FSceneViewport::UpdateCachedGeometry(const FGeometry& InGeometry)
{
	CachedGeometry = InGeometry;
}

bool FSceneViewport::IsCurrentlyGameViewport()
{
	return true;
}

void FSceneViewport::OnWindowBackBufferResourceDestroyed(void* Backbuffer)
{
	// TODO : ViewportRHI.Release
}

void FSceneViewport::OnPreResizeWindowBackbuffer(void* Backbuffer)
{
	OnWindowBackBufferResourceDestroyed(Backbuffer);
}

void FSceneViewport::OnPostResizeWindowBackbuffer(void* Backbuffer)
{
	// TODO : Get ViewportResource
}

FEditorViewportClient::FEditorViewportClient(const std::weak_ptr<SEditorViewport>& InEditorViewportWidget)
	: bAllowCinematicControl(false)
	, CameraSpeedSetting(4)
	, CameraSpeedScalar(1.0f)
	, Viewport(nullptr)
	, ViewFOV(90.f)
	, FOVAngle(90.f)
	, AspectRatio(1.777777f)
	, bUsingOrbitCamera(false)
	, bDisableInput(false)
	, bDrawVertices(false)
	, bShowWidget(true)
	, FlightCameraSpeedScale(1.0f)
	, LastMouseX(0)
	, LastMouseY(0)
	, CachedMouseX(0)
	, CachedMouseY(0)
	, CurrentMousePos(-1, -1)
	, bIsTracking(false)
	, bDraggingByHandle(false)
	, bCameraLock(false)
	, bIsCameraMoving(false)
	, bIsCameraMovingOnTick(false)
	, EditorViewportWidget(InEditorViewportWidget)
	, NearPlane(-1.0f)
	, FarPlane(0.0f)
{
}

void FEditorViewportClient::SetCameraSetup(const FVector& LocationForOrbiting, const FQuat& InOrbitRotation, const FVector& InOrbitZoom, const FVector& InOrbitLookAt, const FVector& InViewLocation, const FQuat& InViewRotation)
{
	if (bUsingOrbitCamera = false)
	{
		SetViewRotation(InOrbitRotation);
		SetViewLocation(InViewLocation + InOrbitZoom);
		SetLookAtLocation(InOrbitLookAt);
	}
	else
	{
		SetViewLocation(InViewLocation);
		SetViewRotation(InViewRotation);
	}

	// Save settings for toggling between orbit and unlocked camera
	DefaultOrbitLocation = InViewLocation;
	DefaultOrbitRotation = InOrbitRotation;
	DefaultOrbitZoom = InOrbitZoom;
	DefaultOrbitLookAt = InOrbitLookAt;
}

void FEditorViewportClient::SetCameraLock()
{
	EnableCameraLock(!bCameraLock);
}

bool FEditorViewportClient::IsCameraLocked() const
{
	return bCameraLock;
}

void FEditorViewportClient::SetShowGrid()
{
}

void FEditorViewportClient::SetShowBounds(bool bShow)
{
}

void FEditorViewportClient::SetShowCollision()
{
}

bool FEditorViewportClient::InputAxis(FViewport* Viewport, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad)
{
	//IPlatformInputDeviceMapper::Get().RemapControllerIdToPlatformUserAndDevice(ControllerId, UserId, DeviceID);

	return Internal_InputAxis(Viewport, Delta, DeltaTime, NumSamples, bGamepad);
}

void FEditorViewportClient::ReceivedFocus(FViewport* Viewport)
{
	//TODO : ReceiveFocus
}

void FEditorViewportClient::MouseEnter(FViewport* Viewport, int32 x, int32 y)
{
	MouseMove(Viewport, x, y);
}

void FEditorViewportClient::MouseMove(FViewport* Viewport, int32 x, int32 y)
{
	CurrentMousePos = FIntPoint(x, y);
	CachedLastMouseX = x;
	CachedLastMouseY = y;
}

void FEditorViewportClient::MouseLeave(FViewport* Viewport)
{
	CurrentMousePos = FIntPoint(-1, -1);
}

EMouseCursor FEditorViewportClient::GetCursor(FViewport* Viewport, int32 X, int32 Y)
{
	return EMouseCursor();
}

void FEditorViewportClient::CapturedMouseMove(FViewport* InViewport, int32 InMouseX, int32 InMouseY)
{
	CapturedMouseMoves.Add(FIntPoint(InMouseX, InMouseY));
}

void FEditorViewportClient::ProcessAccumulatedPointerInput(FViewport* InViewport)
{
	CapturedMouseMoves.Empty();
}

bool FEditorViewportClient::IsOrtho() const
{
	return !IsPerspective();
}

void FEditorViewportClient::LostFocus(FViewport* Viewport)
{
	//StopTracking();
}

bool FEditorViewportClient::Internal_InputKey()
{
	if (bDisableInput)
	{
		return true;
	} 
	

	// TODO : 입력 처리
	return true;
}

bool FEditorViewportClient::Internal_InputAxis(FViewport* Viewport, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad)
{
	if (bDisableInput)
	{
		return true;
	}
	// TODO :: 키 입력
	return true;
}

FVector FEditorViewportClient::GetWidgetLocation() const
{
	// TODO : 어떤 위젯 넣을지
	return FVector();
}

FMatrix FEditorViewportClient::GetWidgetCoordSystem() const
{
	// TODO : 어떤 위젯 넣을지
	return FMatrix();
}

FMatrix FEditorViewportClient::GetLocalCoordinateSystem() const
{	
	// TODO : 어떤 위젯 넣을지
	return FMatrix();
}

void FEditorViewportClient::AdjustTransformWidgetSize(const int32 SizeDelta)
{

}

void FEditorViewportClient::Tick(float DeltaSeconds)
{
	FViewportCameraTransform& ViewTransform = GetViewTransform();
	const bool bIsAnimating = ViewTransform.UpdateTransition();

	// Perspective일 때
	if (bIsAnimating /*&& GetViewportType() == LVT_Perspective*/)
	{
		PerspectiveCameraMoved();
	}

	if (!bIsAnimating)
	{
		bIsCameraMovingOnTick = bIsCameraMoving;

		// Update any real-time camera movement
		UpdateCameraMovement(DeltaSeconds);

		UpdateMouseDelta();

		EndCameraMovement();
	}

	// Tick the editor modes
	//ModeTools->Tick(this, DeltaTime);
}

void FEditorViewportClient::UpdateMouseDelta()
{
	/*FVector DragDelta = MouseDeltaTracker->GetDelta();*/
}

UWorld* FEditorViewportClient::GetWorld() const
{
	UWorld* OutWorldPtr = nullptr;

	if (OutWorldPtr == nullptr)
	{
		OutWorldPtr = UEngine::Get().GetWorld();
	}
	return OutWorldPtr;
}
//
//bool FEditorViewportClient::IsVisible() const
//{
//	bool bIsVisible = false;
//
//	//if (VisibilityDelegate.IsBound())
//	//{
//	//	// Call the visibility delegate to see if our parent viewport and layout configuration says we arevisible
//	//	bIsVisible = VisibilityDelegate.Execute();
//	//}
//	bIsVisible = true;
//	return bIsVisible;
//}

bool FEditorViewportClient::IsPerspective() const
{
	//TODO : Perspective
	return false;
}

void FEditorViewportClient::CenterViewportAtPoint(const FVector& NewLookAt, bool bInstant)
{
	//TODO : 
	const bool bEnable = false;
	//ToggleOrbitCamera(bEnable);

	FViewportCameraTransform& ViewTransform = GetViewTransform();
	FQuat Rotation(ViewTransform.GetRotation());
	FVector LookatVec = ViewTransform.GetLookAt() - ViewTransform.GetLocation();
	// project current lookat vector onto forward vector to get lookat distance, new position is that far along forward vector
	//double LookatDist = FVector::DotProduct(Rotation.GetForwardVector(), LookatVec);
	//FVector NewLocation = NewLookAt - LookatDist * Rotation.GetForwardVector();

	// ortho and perspective are treated the same here
	ViewTransform.SetLookAt(NewLookAt);
	//ViewTransform.TransitionToLocation(NewLocation, EditorViewportWidget, bInstant);
}

void FEditorViewportClient::MoveViewportCamera(const FVector& InDrag, const FQuat& InRot, bool bDollyCamera)
{
	//TODO : 카메라 움직임
}

const uint32 FEditorViewportClient::MaxCameraSpeeds = 8;

float FEditorViewportClient::GetCameraSpeed() const
{
	const float SpeedSetting = GetCameraSpeed(GetCameraSpeedSetting());
	const float FinalCameraSpeedScale = SpeedSetting * FlightCameraSpeedScale * GetCameraSpeedScalar() * 1;
	return FinalCameraSpeedScale;
}

float FEditorViewportClient::GetCameraSpeed(int32 SpeedSetting) const
{
	//previous mouse speed values were as follows...
	//(note: these were previously all divided by 4 when used be the viewport)
	//#define MOVEMENTSPEED_SLOW			4	~ 1
	//#define MOVEMENTSPEED_NORMAL			12	~ 3
	//#define MOVEMENTSPEED_FAST			32	~ 8
	//#define MOVEMENTSPEED_VERYFAST		64	~ 16

	const int32 SpeedToUse = FMath::Clamp<int32>(SpeedSetting, 1, MaxCameraSpeeds);
	const float Speed[] = { 0.033f, 0.1f, 0.33f, 1.f, 3.f, 8.f, 16.f, 32.f };

	return Speed[SpeedToUse - 1];
}

void FEditorViewportClient::SetCameraSpeedSetting(int32 SpeedSetting)
{
	CameraSpeedSetting = SpeedSetting;
}

int32 FEditorViewportClient::GetCameraSpeedSetting() const
{
	return CameraSpeedSetting;
}

float FEditorViewportClient::GetCameraSpeedScalar() const
{
	return CameraSpeedScalar;
}

void FEditorViewportClient::SetCameraSpeedScalar(float SpeedScalar)
{
	CameraSpeedScalar = FMath::Clamp<float>(SpeedScalar, 1.0f, 3.402823466e+38F);
}

FMatrix FEditorViewportClient::CalcViewRotationMatrix(const FQuat& InViewRotation) const
{
	const FViewportCameraTransform& ViewTransform = GetViewTransform();

	// TODO : 카메라 Matrix
	if (bUsingOrbitCamera)
	{
		// @todo vreditor: Not stereo friendly yet
		return FMatrix();
	}
	else
	{
		// Create the view matrix
		return FMatrix();
	}
}

float FEditorViewportClient::GetNearClipPlane() const
{
	return NearPlane;
}

void FEditorViewportClient::OverrideNearClipPlane(float InNearPlane)
{
	NearPlane = InNearPlane;
}

float FEditorViewportClient::GetFarClipPlaneOverride() const
{
	return FarPlane;
}

void FEditorViewportClient::OverrideFarClipPlane(const float InFarPlane)
{
	FarPlane = InFarPlane;
}

void FEditorViewportClient::ShowWidget(const bool bShow)
{
	bShowWidget = bShow;
}

void FEditorViewportClient::ApplyDeltaToRotateWidget(const FQuat& InRot)
{
	//TODO : Widget 회전
	FVector Rotate = InRot.GetEuler();
	//apply rotation to translate rotate widget
	if (!(Rotate.X == 0 && Rotate.Y == 0 && Rotate.Z == 0))
	{
		//FQuat TranslateRotateWidgetRotation(0, ModeTools->TranslateRotateXAxisAngle, 0);
		//TranslateRotateWidgetRotation += InRot;
		//ModeTools->TranslateRotateXAxisAngle = TranslateRotateWidgetRotation.Yaw;

		//FQuat Widget2DRotation(ModeTools->TranslateRotate2DAngle, 0, 0);
		//Widget2DRotation += InRot;
		//ModeTools->TranslateRotate2DAngle = Widget2DRotation.Pitch;
	}
}

void FEditorViewportClient::OnOrthoZoom(/*const FInputEventState& InputState,*/ float Scale)
{
	//TODO : OrthoZoom
}

void FEditorViewportClient::OnChangeCameraSpeed(/*const FInputEventState& InputState*/)
{
	//TODO : CameraSpeed
}

void FEditorViewportClient::EnableCameraLock(bool bEnable)
{
	bCameraLock = bEnable;

	if (bCameraLock)
	{
		SetViewLocation(DefaultOrbitLocation + DefaultOrbitZoom);
		SetViewRotation(DefaultOrbitRotation);
		SetLookAtLocation(DefaultOrbitLookAt);
	}

	bUsingOrbitCamera = bCameraLock;
}

bool FEditorViewportClient::CalculateEditorConstrainedViewRect(FSlateRect& OutSafeFrameRect, FViewport* InViewport)
{
	const float SizeX = InViewport->GetSizeXY().X;
	const float SizeY = InViewport->GetSizeXY().Y;

	OutSafeFrameRect = FSlateRect(0, 0, SizeX, SizeY);
	float FixedAspectRatio;
	bool bSafeFrameActive = GetActiveSafeFrame(FixedAspectRatio);

	if (bSafeFrameActive)
	{
		// Get the size of the viewport
		float ActualAspectRatio = (float)SizeX / (float)SizeY;

		float SafeWidth = SizeX;
		float SafeHeight = SizeY;

		if (FixedAspectRatio < ActualAspectRatio)
		{
			// vertical bars required on left and right
			SafeWidth = FixedAspectRatio * SizeY;
			float CorrectedHalfWidth = SafeWidth * 0.5f;
			float CentreX = SizeX * 0.5f;
			float X1 = CentreX - CorrectedHalfWidth;
			float X2 = CentreX + CorrectedHalfWidth;
			OutSafeFrameRect = FSlateRect(X1, 0, X2, SizeY);
		}
		else
		{
			// horizontal bars required on top and bottom
			SafeHeight = SizeX / FixedAspectRatio;
			float CorrectedHalfHeight = SafeHeight * 0.5f;
			float CentreY = SizeY * 0.5f;
			float Y1 = CentreY - CorrectedHalfHeight;
			float Y2 = CentreY + CorrectedHalfHeight;
			OutSafeFrameRect = FSlateRect(0, Y1, SizeX, Y2);
		}
	}

	return bSafeFrameActive;
}

void FEditorViewportClient::MoveViewportPerspectiveCamera(const FVector& InDrag, const FQuat& InRot, bool bDollyCamera)
{
	// TODO : PerspectiveCameraMove
}

// Updates real-time camera movement.  Should be called every viewport tick!
void FEditorViewportClient::UpdateCameraMovement(float DeltaTime)
{
	//TODO : UpdateCameraMovement
}

FViewportCameraTransform::FViewportCameraTransform()
	: ViewLocation(FVector::ZeroVector)
	, ViewRotation(FQuat())
	, DesiredLocation(FVector::ZeroVector)
	, LookAt(FVector::ZeroVector)
	, StartLocation(FVector::ZeroVector)
	, OrthoZoom(10000.0)
{
}

void FViewportCameraTransform::SetLocation(const FVector& Position)
{
	ViewLocation = Position;
	DesiredLocation = ViewLocation;
}

bool FViewportCameraTransform::IsPlaying()
{
	return true;
}

void FViewportCameraTransform::TransitionToLocation(const FVector& InDesiredLocation, std::weak_ptr<SWidget> EditorViewportWidget, bool bInstant)
{
	if (bInstant || !EditorViewportWidget.lock())
	{
		SetLocation(InDesiredLocation);
	}
	else
	{
		DesiredLocation = InDesiredLocation;
		StartLocation = ViewLocation;
	}
}

bool FViewportCameraTransform::UpdateTransition()
{
	bool bIsAnimating = false;
	if (ViewLocation != DesiredLocation)
	{
		//float LerpWeight = TransitionCurve->GetLerp();

		if (/*LerpWeight == 1.0f*/true)
		{
			// Failsafe for the value not being exact on lerps
			ViewLocation = DesiredLocation;
		}
		else
		{
			//TODO : Lerp
			//ViewLocation = FMath::Lerp(StartLocation, DesiredLocation, LerpWeight);
		}


		bIsAnimating = true;
	}

	return bIsAnimating;
}

//FMatrix FViewportCameraTransform::ComputeOrbitMatrix() const
//{
//	FTransform Transform =
//		FTransform(-LookAt) *
//		FTransform(FVector(0, ViewRotation.Z, 0)) *
//		FTransform(FVector(0, 0, ViewRotation.Y)) *
//		FTransform(FVector(0, (ViewLocation - LookAt).Length(), 0));
//
//	return Transform.ToMatrixNoScale() * FInverseRotationMatrix(FRotator(0, 90.f, 0));
//}
