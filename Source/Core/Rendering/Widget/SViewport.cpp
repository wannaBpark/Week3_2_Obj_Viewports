#include "SViewport.h"

SEditorViewport::SEditorViewport()
{
}

SEditorViewport::~SEditorViewport()
{
    if (Client != nullptr)
    {
        Client->Viewport = nullptr;
    }

    // Release our reference to the viewport client
    Client.reset();
}

void SEditorViewport::Construct()
{
    // Create ViewportWIdget

    Client = MakeEditorViewportClient();

    SceneViewport = make_shared<FSceneViewport>(Client.get(), ViewportWidget);

    Client->Viewport = SceneViewport.get();
}

FReply SEditorViewport::OnKeyDown(const FGeometry& MyGeometry, const EKeyCode& InKeyCode)
{
    return FReply();
    //FReply Reply = FReply::Unhandled();
    //if (CommandList->ProcessCommandBindings(InKeyEvent))
    //{
    //    Reply = FReply::Handled();
    //    Client->Invalidate();
    //}

    //return Reply;
}

bool SEditorViewport::SupportsKeyboardFocus() const
{
    return true;
}

FReply SEditorViewport::OnFocusReceived(const FGeometry& MyGeometry)
{
    //TODO : forward focus to the viewport
    return FReply();
    //return FReply::Handled().SetUserFocus(ViewportWidget.ToSharedRef(), InFocusEvent.GetCause());
}

void SEditorViewport::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    // LastTickTime = FPlatformTime::Seconds();
}

bool SEditorViewport::IsRealtime() const
{
    return true;
    //return Client->IsRealtime();
}

bool SEditorViewport::IsVisible() const
{
    return false;
}

void SEditorViewport::SetRenderDirectlyToWindow(const bool bInRenderDirectlyToWindow)
{
    ViewportWidget->SetRenderDirectlyToWindow(bInRenderDirectlyToWindow);
}

bool SEditorViewport::IsTranslateRotateModeVisible() const
{
    return true;
}

UWorld* SEditorViewport::GetWorld() const
{
    return Client->GetWorld();
}

SViewport::SViewport()
    : ViewportSize(FVector2D(0,0))
    , bRenderDirectlyToWindow(false)
{
}

SViewport::~SViewport()
{
}

void SViewport::Construct()
{
}

void SViewport::SetViewportInterface(std::shared_ptr<ISlateViewport> InViewportInterface)
{
    if (ViewportInterface.lock() != InViewportInterface)
    {
        ViewportInterface = InViewportInterface;
        Invalidate(EInvalidateWidgetReason::Paint);
    }
}

void SViewport::SetContent(std::shared_ptr<SWidget> InContent)
{
}

void SViewport::OnWindowClosed(const std::shared_ptr<SWindow>& InWindowBeingClosed)
{
    if (ViewportInterface.lock() != nullptr)
    {
        ViewportInterface.lock()->OnViewportClosed();
    }
}

void SViewport::SetRenderDirectlyToWindow(const bool bInRenderDirectlyToWindow)
{
    if (bRenderDirectlyToWindow != bInRenderDirectlyToWindow)
    {
        bRenderDirectlyToWindow = bInRenderDirectlyToWindow;
        Invalidate(EInvalidateWidgetReason::Paint);
    }
}

void SViewport::SetActive(bool bActive)
{
}

void SViewport::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    if (ViewportInterface.lock() != nullptr)
    {
        ViewportInterface.lock()->Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
    }
}

FCursorReply SViewport::OnCursorQuery(const FGeometry& MyGeometry, const FPointer& InPointer) const
{
    return ViewportInterface.lock() != nullptr ? ViewportInterface.lock()->OnCursorQuery(MyGeometry) : FCursorReply::Cursor(EMouseCursor::Default);
}

std::optional<std::shared_ptr<SWidget>> SViewport::OnMapCursor(const FCursorReply& CursorReply) const
{
    return ViewportInterface.lock() != nullptr ? ViewportInterface.lock()->OnMapCursor(CursorReply) : std::optional< std::shared_ptr<SWidget>>();
}

FReply SViewport::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointer& InPointer)
{
    return ViewportInterface.lock() != nullptr ? ViewportInterface.lock()->OnMouseButtonDown(MyGeometry) : FReply();
}

FReply SViewport::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointer& InPointer)
{
    return ViewportInterface.lock() != nullptr ? ViewportInterface.lock()->OnMouseButtonUp(MyGeometry) : FReply();
}

void SViewport::OnMouseEnter(const FGeometry& MyGeometry, const FPointer& InPointer)
{
    SWidget::OnMouseEnter(MyGeometry, InPointer);

    if (ViewportInterface.lock() != nullptr)
    {
        ViewportInterface.lock()->OnMouseEnter(MyGeometry);
    }
}

void SViewport::OnMouseLeave(const FPointer& InPointer)
{
    SWidget::OnMouseLeave(InPointer);

    if (ViewportInterface.lock() != nullptr)
    {
        ViewportInterface.lock()->OnMouseLeave();
    }
}

FReply SViewport::OnMouseMove(const FGeometry& MyGeometry, const FPointer& InPointer)
{
    return ViewportInterface.lock() != nullptr ? ViewportInterface.lock()->OnMouseMove(MyGeometry) : FReply();

}

FReply SViewport::OnMouseWheel(const FGeometry& MyGeometry, const FPointer& InPointer)
{
    return ViewportInterface.lock() != nullptr ? ViewportInterface.lock()->OnMouseWheel(MyGeometry) : FReply();

}

FReply SViewport::OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointer& InPointer)
{
    return ViewportInterface.lock() != nullptr ? ViewportInterface.lock()->OnMouseButtonDoubleClick(MyGeometry) : FReply();

}

FReply SViewport::OnKeyDown(const FGeometry& MyGeometry, const EKeyCode& InKeyCode)
{
    return ViewportInterface.lock() != nullptr ? ViewportInterface.lock()->OnKeyDown(MyGeometry) : FReply();

}

FReply SViewport::OnKeyUp(const FGeometry& MyGeometry, const EKeyCode& InKeyCode)
{
    return ViewportInterface.lock() != nullptr ? ViewportInterface.lock()->OnKeyUp(MyGeometry) : FReply();

}

FReply SViewport::OnKeyChar(const FGeometry& MyGeometry, const EKeyCode& InKeyCode)
{
    return ViewportInterface.lock() != nullptr ? ViewportInterface.lock()->OnKeyChar(MyGeometry) : FReply();

}

FReply SViewport::OnFocusReceived(const FGeometry& MyGeometry)
{
    return ViewportInterface.lock() != nullptr ? ViewportInterface.lock()->OnFocusReceived() : FReply();

}

void SViewport::OnFocusLost()
{
    if (ViewportInterface.lock() != nullptr)
    {
        ViewportInterface.lock()->OnFocusLost();
    }
}

void SViewport::OnFinishedPointerInput()
{
    std::shared_ptr<ISlateViewport> PinnedInterface = ViewportInterface.lock ();
    if (PinnedInterface != nullptr)
    {
        PinnedInterface->OnFinishedPointerInput();
    }
}

void SViewport::OnArrangeChildren(const FGeometry& AllottedGeometry, TArray<FArrangedWidget>& ArrangedChildren) const
{
    //if (ArrangedChildren.Allows3DWidgets() && CustomHitTestPath.IsValid())
    //{
    //    CustomHitTestPath->ArrangeCustomHitTestChildren(ArrangedChildren);
    //}
}
