#pragma once
#include "SWindow.h"
#include "Debug/DebugConsole.h"
#include "Core/Rendering/URenderer.h"
#include "Object/World/World.h"
#include <iostream>


class SViewportWindow final : public SWindow {
private:
    D3D11_VIEWPORT DXViewport;
    std::shared_ptr<ACamera> Camera;
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

    void Render() override 
    {
        if (bRenderable) {
            UpdateViewport();
            /*UE_LOG("Rendering Viewport (%f, %f) - (%f, %f)", 
                DXViewport.TopLeftX, DXViewport.TopLeftY, 
                DXViewport.TopLeftX + DXViewport.Width, 
                DXViewport.TopLeftY + DXViewport.Height );*/

            UEngine::Get().GetRenderer()->SetViewport(DXViewport);
            UEngine::Get().GetWorld()->Render();
            bRenderable = false;
        }
    }
};