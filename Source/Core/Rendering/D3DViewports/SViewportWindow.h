#pragma once
#include "SWindow.h"
#include "Debug/DebugConsole.h"
#include "Core/Rendering/URenderer.h"
#include "Object/World/World.h"
#include <iostream>


class SViewportWindow final : public SWindow {
    using Super = SWindow;
private:
    // 모든 viewport는 dragging, 전체화면 여부를 공유하여 자신의 입력처리에 반영
    inline static bool bIsAnyFullScreen = false;
    inline static bool bIsAnyDragging = false;
    inline static TArray<std::shared_ptr<ACamera>> AllCameras;

    D3D11_VIEWPORT DXViewport;
    std::shared_ptr<ACamera> Camera;
    uint32 CameraIdx = 0;
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
            UpdateViewport();
            if (Camera)
            {
                Camera->SetViewportSize(DXViewport.Width, DXViewport.Height);
                FEditorManager::Get().SetCamera(this->Camera.get());
            }

            UEngine::Get().GetRenderer()->SetViewport(DXViewport);
            UEngine::Get().GetWorld()->Render();
            bRenderable = false;
            // TickPlayerInput
            //UEngine::Get().
            // TODO : 내 윈도우를 hover하며 카메라조작하다가
            // 다른 윈도우에 점유되면 안움직이도록 해야 함
        }
    }

    virtual void OnMouseDown(const FPoint& MousePos) override
    {
        //UE_LOG("MOUSE Down By Viewport %d %d", Rect.X, Rect.Y);
        if (IsHover(MousePos) /*&& !bIsAnyDragging*/)
        {
            bIsDragging = bIsAnyDragging = true;
            FEditorManager::Get().SetInputCamera(this->Camera.get());
            //UE_LOG("Set Camera By Viewport %d %d", Rect.X, Rect.Y);
        }
    }

    virtual void OnMouseMove(const FPoint& MousePos) override
    {
        bHovered = IsHover(MousePos);
    }

    virtual void OnMouseUp(const FPoint& MousePos) override
    {
        if (bIsDragging && bIsAnyDragging)  // 드래깅되고 있고, 점유된 화면만이 false로 변경
        {
            bIsAnyDragging = false;
            //UE_LOG("UNSET Camera By Viewport %d %d", Rect.X, Rect.Y);
        }
        bIsDragging = false;
    }

    virtual void OnKeyDown(EKeyCode Key) override
    {
        if (!bHovered) { return; }

        if (Key == EKeyCode::F) {
            if (!bFullScreen && bIsAnyFullScreen) { return; }
            ToggleFullScreen();
        }
        else if (Key == EKeyCode::_1) {
            CycleCamera();
        }
    }

    void ToggleFullScreen()
    {
        if (!bFullScreen) // 자기 자신을 전체화면으로 바꿉니다
        {
            uint32 FullWidth = static_cast<uint32>(UEngine::Get().GetScreenWidth());
            uint32 FullHeight = static_cast<uint32>(UEngine::Get().GetScreenHeight());

            OriginalRect = Rect;
            SetRect({ 0, 0, FullWidth, FullHeight });
            bIsAnyFullScreen = true;
        }
        else               // 이미 전체화면인 경우 사촌 viewport에 FullScreen변수에 false를 전달합니다
        {
            bIsAnyFullScreen = false; 
        }
        bFullScreen = !bFullScreen;
        SetNeedsRender();
    }

    void CycleCamera()  // Get Next Camera
    {
        Camera = UEngine::Get().GetCameraByIdx(++CameraIdx); 
    }

    virtual void SetRect(const FRect& InRect) override
    { 
        if (bFullScreen) return;
        Rect = InRect; SetNeedsRender(); 
    }
};