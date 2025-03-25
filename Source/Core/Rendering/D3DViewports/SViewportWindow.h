#pragma once
#include "SWindow.h"
#include "Debug/DebugConsole.h"
#include "Core/Rendering/URenderer.h"
#include "Object/World/World.h"
#include <iostream>


class SViewportWindow final : public SWindow {
private:
    inline static bool bIsAnyFullScreen = false;
    inline static TArray<std::shared_ptr<ACamera>> AllCameras;

    D3D11_VIEWPORT DXViewport;
    std::shared_ptr<ACamera> Camera;
    uint32 CameraIdx = 0;
    bool bHovered = false;          // 마우스 Hover 여부
    bool bFullScreen = false;
    FRect OriginalRect;
public:
    void UpdateViewport() 
    {
        DXViewport.TopLeftX = static_cast<float>(Rect.X);
        DXViewport.TopLeftY = static_cast<float>(Rect.Y);
        DXViewport.Width = static_cast<float>(Rect.W);
        DXViewport.Height = static_cast<float>(Rect.H);
        DXViewport.MinDepth = 0.0f;
        DXViewport.MaxDepth = 1.0f;
    }

    void SetCamera(std::shared_ptr<ACamera> InCamera, uint32 idx)
    {
        if (InCamera) { Camera = InCamera; CameraIdx = idx; }
    }

    void Render() override 
    {
        // 사촌들 중 하나가 fullscreen인데 내가 아닌 경우엔 렌더하지 않음
        if (bRenderable && (!bIsAnyFullScreen || bFullScreen)) {
            if (bFullScreen)
            {
                UE_LOG("My Fullscreen Rect : %d %d %d %d",
                    Rect.X, Rect.Y, Rect.W, Rect.H);
            }
            UpdateViewport();
            if (Camera)
            {
                Camera->SetViewportSize(DXViewport.Width, DXViewport.Height);
                FEditorManager::Get().SetCamera(this->Camera.get());
            }

            UEngine::Get().GetRenderer()->SetViewport(DXViewport);
            UEngine::Get().GetWorld()->Render();
            bRenderable = false;
        }
    }

    virtual void OnMouseMove(const FPoint& MousePos) override
    {
        bHovered = IsInRect(MousePos, Rect);
        if (bHovered)
        {
            //UE_LOG("Hovered Window %d %d", Rect.X, Rect.Y);
        }
    }

    virtual void OnKeyDown(EKeyCode Key) override
    {
        if (!bHovered) { return; }

        if (Key == EKeyCode::F) {
            ToggleFullScreen();
        }
        else if (Key == EKeyCode::_1) {
            CycleCamera();
        }
    }

    void ToggleFullScreen()
    {
        if (!bFullScreen)
        {
            uint32 FullWidth = static_cast<uint32>(UEngine::Get().GetScreenWidth());
            uint32 FullHeight = static_cast<uint32>(UEngine::Get().GetScreenHeight());

            OriginalRect = Rect;
            SetRect({ 0, 0, FullWidth, FullHeight });
            bIsAnyFullScreen = true;
        }
        else
        {
            bIsAnyFullScreen = false;
        }
        bFullScreen = !bFullScreen;
        SetNeedsRender();
    }

    void CycleCamera()
    {
        Camera = UEngine::Get().GetCameraByIdx(++CameraIdx);
        //UE_LOG("SViewportWindow : CycleCamera triggered");
    }

    virtual void SetRect(const FRect& InRect) override
    { 
        if (bFullScreen) return;
        Rect = InRect; SetNeedsRender(); 
    }
};