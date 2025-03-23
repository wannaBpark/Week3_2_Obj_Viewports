#pragma once
#include "SWindow.h"
#include "Debug/DebugConsole.h"

class SViewportWindow final : public SWindow {
private:
    D3D11_VIEWPORT DXViewport;

public:
    void UpdateViewport() {
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
            UE_LOG("Rendering Viewport (%d, %d) - (%d, %d) - (%d - %d) ",
                DXViewport.TopLeftX, DXViewport.TopLeftY,
                DXViewport.TopLeftX, DXViewport.Width,
                DXViewport.TopLeftY, DXViewport.Height
                );
            bRenderable = false;
        }
    }
};