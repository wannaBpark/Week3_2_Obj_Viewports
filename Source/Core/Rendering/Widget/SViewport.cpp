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

FReply SEditorViewport::OnKeyDown(const EKeyCode& InKeyCode)
{
    return Super::OnKeyDown(InKeyCode);
}

SLevelViewport::SLevelViewport()
{
}

SLevelViewport::~SLevelViewport()
{
}

FReply SEditorViewport::OnFocusReceived()
{
    //TODO : forward focus to the viewport
    return FReply();
    //return FReply::Handled().SetUserFocus(ViewportWidget.ToSharedRef(), InFocusEvent.GetCause());
}

void SEditorViewport::Tick(const FGeometry& AllottedGeometry, const float InDeltaTime)
{
    Super::Tick(AllottedGeometry, InDeltaTime);
}

bool SEditorViewport::IsVisible() const
{
    return true;
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

void SLevelViewport::Tick(const FGeometry& ParentGeometry, float DeltaTime)
{
    Super::Tick(ParentGeometry, DeltaTime);
}

FReply SLevelViewport::OnKeyDown(const EKeyCode& InKeyCode)
{
    FReply result = Super::OnKeyDown(InKeyCode);

    return result;
}

FReply SLevelViewport::OnFocusReceived()
{
    FReply result = Super::OnFocusReceived();

    return result;
}

void SLevelViewport::OnFocusLost()
{
    Super::OnFocusLost();
}

FReply SLevelViewport::OnMouseButtonDown(EMouseButton InMouseButton, const FPointer& InPointer)
{
    FReply result = Super::OnMouseButtonDown(InMouseButton, InPointer);
    if (bIsMouseCaptured)
    {
        SceneViewport->OnMouseButtonDown(InMouseButton, InPointer);
    }
    return result;
}

FReply SLevelViewport::OnMouseButtonUp(EMouseButton InMouseButton, const FPointer& InPointer)
{
    FReply result = Super::OnMouseButtonUp(InMouseButton, InPointer);

    if (bIsMouseCaptured)
    {
        SceneViewport->OnMouseButtonUp(InMouseButton, InPointer);
    }
    
    return result;
}

FReply SLevelViewport::OnMouseMove(const FPointer& InPointer)
{
    FReply result = Super::OnMouseMove(InPointer);

    if (bIsMouseCaptured)
    {
        Client->MouseMove(InPointer);
        SceneViewport->OnMouseMove(InPointer);
    }
    
    return result;
}

void SLevelViewport::OnMouseEnter(const FPointer& InPointer)
{
    SceneViewport->OnMouseEnter(InPointer);
}

void SLevelViewport::OnMouseLeave(const FPointer& InPointer)
{
    SceneViewport->OnMouseLeave(InPointer);
}

FReply SLevelViewport::OnMouseWheel(const FPointer& InPointer)
{
    FReply result = Super::OnMouseWheel(InPointer);

    return result;
}

SViewport::SViewport(FVector2D InViewportSize)
    : ViewportSize(InViewportSize)
    , bRenderDirectlyToWindow(false)
{
}

SViewport::~SViewport()
{
}

void SViewport::UpdateViewportSize(const FVector2D& NewSize)
{
}

bool SViewport::HasMouseCapture() const
{
    return IsMouseCaptured();
}

void SViewport::SetViewportInterface(std::shared_ptr<ISlateViewport> InViewportInterface)
{
    if (ViewportInterface.lock() != InViewportInterface)
    {
        ViewportInterface = InViewportInterface;
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
    }
}

void SViewport::SetActive(bool bActive)
{
}

void SViewport::Tick(const FGeometry& AllottedGeometry, const float InDeltaTime)
{
    Super::Tick(AllottedGeometry, InDeltaTime);
    if (ViewportInterface.lock() != nullptr)
    {
        ViewportInterface.lock()->Tick(AllottedGeometry, InDeltaTime);
    }
}

FCursorReply SViewport::OnCursorQuery(const FPointer& InPointer) const
{
    return ViewportInterface.lock() != nullptr ? ViewportInterface.lock()->OnCursorQuery(InPointer) : FCursorReply::Cursor(EMouseCursor::Default);
}

std::optional<std::shared_ptr<SWidget>> SViewport::OnMapCursor(const FCursorReply& CursorReply) const
{
    return ViewportInterface.lock() != nullptr ? ViewportInterface.lock()->OnMapCursor(CursorReply) : std::optional< std::shared_ptr<SWidget>>();
}

FReply SViewport::OnMouseButtonDown(EMouseButton InMouseButton, const FPointer& InPointer)
{
    return ViewportInterface.lock() != nullptr ? ViewportInterface.lock()->OnMouseButtonDown(InMouseButton, InPointer) : FReply();
}

FReply SViewport::OnMouseButtonUp(EMouseButton InMouseButton, const FPointer& InPointer)
{
    return ViewportInterface.lock() != nullptr ? ViewportInterface.lock()->OnMouseButtonUp(InMouseButton, InPointer) : FReply();
}

void SViewport::OnMouseEnter(const FPointer& InPointer)
{
    SWidget::OnMouseEnter(InPointer);

    if (ViewportInterface.lock() != nullptr)
    {
        ViewportInterface.lock()->OnMouseEnter(InPointer);
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

FReply SViewport::OnMouseMove(const FPointer& InPointer)
{
    return ViewportInterface.lock() != nullptr ? ViewportInterface.lock()->OnMouseMove(InPointer) : FReply();

}

FReply SViewport::OnMouseWheel(const FPointer& InPointer)
{
    return ViewportInterface.lock() != nullptr ? ViewportInterface.lock()->OnMouseWheel(InPointer) : FReply();

}

FReply SViewport::OnMouseButtonDoubleClick(const FPointer& InPointer)
{
    return ViewportInterface.lock() != nullptr ? ViewportInterface.lock()->OnMouseButtonDoubleClick(InPointer) : FReply();

}

FReply SViewport::OnKeyDown(const EKeyCode& InKeyCode)
{
    return ViewportInterface.lock() != nullptr ? ViewportInterface.lock()->OnKeyDown(InKeyCodeInKeyCode) : FReply();

}

FReply SViewport::OnKeyUp(const EKeyCode& InKeyCode)
{
    return ViewportInterface.lock() != nullptr ? ViewportInterface.lock()->OnKeyUp(InKeyCode) : FReply();

}

FReply SViewport::OnKeyChar(const EKeyCode& InKeyCode)
{
    return ViewportInterface.lock() != nullptr ? ViewportInterface.lock()->OnKeyChar(InKeyCode) : FReply();

}

FReply SViewport::OnFocusReceived()
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
    SWidget::ArrangeSingleChild(AllottedGeometry, ArrangedChildren, std::const_pointer_cast<SWidget>(shared_from_this()), FVector2D(1.0f,1.0f));
}
