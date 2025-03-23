#include "Engine.h"

#include <iostream>
#include "Object/ObjectFactory.h"
#include "Object/World/World.h"
#include "Debug/DebugConsole.h"
#include "Object/Gizmo/Axis.h"
#include "Core/Input/PlayerInput.h"
#include "Core/Input/PlayerController.h"
#include "Object/Actor/Camera.h"
#include "Object/Actor/Sphere.h"
#include "Object/Actor/WorldGrid.h"
#include "Static/FEditorManager.h"
#include "Object/Gizmo/WorldGizmo.h"
#include "Core/FSceneManager.h"

#include "Core/Rendering/D3DViewports/SViewportWindow.h"

class AArrow;
class APicker;
// ImGui WndProc 정의
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT UEngine::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // ImGui의 메시지를 처리
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
    {
        return true;
    }
    
    switch (uMsg)
    {
        // 창이 제거될 때 (창 닫기, Alt+F4 등)
    case WM_DESTROY:
        PostQuitMessage(0); // 프로그램 종료
        break;
    case WM_KEYDOWN:
        APlayerInput::Get().KeyDown(static_cast<EKeyCode>(wParam));
        if ((lParam>>30)%2 != 0)
        {
            APlayerInput::Get().KeyOnceUp(static_cast<EKeyCode>( wParam ));
        }
        break;
    case WM_KEYUP:
        APlayerInput::Get().KeyUp(static_cast<EKeyCode>( wParam ));
        break;
    case WM_LBUTTONDOWN:
        APlayerInput::Get().HandleMouseInput(hWnd, lParam, true, false);
        break;
    case WM_LBUTTONUP:
        APlayerInput::Get().HandleMouseInput(hWnd, lParam, false, false);
        break;
    case WM_RBUTTONDOWN:
        APlayerInput::Get().HandleMouseInput(hWnd, lParam, true, true);
        break;
    case WM_RBUTTONUP:
        APlayerInput::Get().HandleMouseInput(hWnd, lParam, false, true);
        break;
    case WM_SIZE:
		UEngine::Get().UpdateWindowSize(LOWORD(lParam), HIWORD(lParam));
		break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

void UEngine::Initialize(
    HINSTANCE hInstance, const WCHAR* InWindowTitle, const WCHAR* InWindowClassName, int InScreenWidth,
    int InScreenHeight,
    EScreenMode InScreenMode
)
{
    WindowInstance = hInstance;
    WindowTitle = InWindowTitle;
    WindowClassName = InWindowClassName;
    ScreenMode = InScreenMode;
    ScreenWidth = InScreenWidth;
    ScreenHeight = InScreenHeight;

    InitWindow(InScreenWidth, InScreenHeight);
    InitRenderer();

    InitWorld();

    InitializedScreenWidth = ScreenWidth;
    InitializedScreenHeight = ScreenHeight;
    
    ui.Initialize(WindowHandle, *Renderer, ScreenWidth, ScreenHeight);
    
	UE_LOG("Engine Initialized!");
}

void UEngine::Run()
{
    // FPS 제한
    constexpr int TargetFPS = 750;
    constexpr double TargetDeltaTime = 1000.0f / TargetFPS; // 1 FPS의 목표 시간 (ms)

    // 고성능 타이머 초기화
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);

    LARGE_INTEGER StartTime;
    QueryPerformanceCounter(&StartTime);


    IsRunning = true;
    while (IsRunning)
    {
        // DeltaTime 계산 (초 단위)
        const LARGE_INTEGER EndTime = StartTime;
        QueryPerformanceCounter(&StartTime);

        const float DeltaTime =
            static_cast<float>(StartTime.QuadPart - EndTime.QuadPart) / static_cast<float>(Frequency.QuadPart);

        APlayerInput::Get().PreProcessInput();
        
        // 메시지(이벤트) 처리
        MSG Msg;
        while (PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
        {
            // 키 입력 메시지를 번역
            TranslateMessage(&Msg);

            // 메시지를 등록한 Proc에 전달
            DispatchMessage(&Msg);

            if (Msg.message == WM_QUIT)
            {
                IsRunning = false;
                break;
            }

        }
		// Renderer Update
        Renderer->Prepare();          
        //Renderer->PrepareShader();    // 각 rendercomponent에서 호출

		// World Update
		if (World)
		{
			World->Tick(DeltaTime);
            // 변경 : 각 뷰포트가 모두 렌더 한 후
            RootWindow->Render();
            RootWindow->UpdateLayout();
            

		    World->LateTick(DeltaTime);
		}

        //각 Actor에서 TickActor() -> PlayerTick() -> TickPlayerInput() 호출하는데 지금은 Message에서 처리하고 있다.
        APlayerInput::Get().TickPlayerInput(); //잘못된 위치. 위에 달린 주석대로 처리해야 정상 플레이어 액터 내에서만 처리해야할것같다.
        
        // TickPlayerInput
        APlayerController::Get().ProcessPlayerInput(DeltaTime);
        
		// ui Update
        ui.Update();

        Renderer->SwapBuffer();

        // FPS 제한
        double ElapsedTime;
        do
        {
            Sleep(0);

            LARGE_INTEGER CurrentTime;
            QueryPerformanceCounter(&CurrentTime);

            ElapsedTime = static_cast<double>(CurrentTime.QuadPart - StartTime.QuadPart) * 1000.0 / static_cast<double>(Frequency.QuadPart);
        } while (ElapsedTime < TargetDeltaTime);
    }
}


void UEngine::Shutdown()
{
    ShutdownWindow();
}


void UEngine::InitWindow(int InScreenWidth, int InScreenHeight)
{
    // 윈도우 클래스 등록
    WNDCLASSW wnd_class{};
    wnd_class.lpfnWndProc = WndProc;
    wnd_class.hInstance = WindowInstance;
    wnd_class.lpszClassName = WindowClassName;
    RegisterClassW(&wnd_class);

    // 원하는 클라이언트 영역 크기를 위한 윈도우 크기 계산 (딱 맞도록)
    RECT windowRect = { 0, 0, InScreenWidth, InScreenHeight };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;

    // 화면 중앙에 위치하도록 좌표 계산
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int windowX = (screenWidth - windowWidth) / 2;
    int windowY = (screenHeight - windowHeight) / 2;

    // Window Handle 생성
    WindowHandle = CreateWindowExW(
        0, WindowClassName, WindowTitle,
        WS_OVERLAPPEDWINDOW,
        windowX, windowY,
        windowWidth, windowHeight,
        nullptr, nullptr, WindowInstance, nullptr
    );

    //// Window Handle 생성
    WindowHandle = CreateWindowExW(
        0, WindowClassName, WindowTitle,
        WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        InScreenWidth, InScreenHeight,
        nullptr, nullptr, WindowInstance, nullptr
    );

    // TODO: 전체화면 구현
    if (ScreenMode != EScreenMode::Windowed)
    {
        std::cout << "not implement Fullscreen and Borderless mode." << '\n';
    }

    // 윈도우 포커싱
    ShowWindow(WindowHandle, SW_SHOW);
    SetForegroundWindow(WindowHandle);
    SetFocus(WindowHandle);

#pragma region Multi Viewports               // 
    auto topLeft = std::make_shared<SViewportWindow>();
    auto topRight = std::make_shared<SViewportWindow>();
    auto bottomLeft = std::make_shared<SViewportWindow>();
    auto bottomRight = std::make_shared<SViewportWindow>();
    auto TopVerticalSplitter = std::make_shared<SSplitterV>(topLeft, topRight);
    auto BottomVerticalSplitter = std::make_shared<SSplitterV>(bottomLeft, bottomRight);

    SViewportWindows.Add(topLeft); SViewportWindows.Add(topRight);
    SViewportWindows.Add(bottomLeft); SViewportWindows.Add(bottomRight);

    RootWindow = std::make_unique<SSplitterH>(TopVerticalSplitter, BottomVerticalSplitter);
    RootWindow->SetRect({
        0, 0,                               // TopLeft, TopRight 
        static_cast<uint32>(InScreenWidth), // Width
        static_cast<uint32>(InScreenHeight) // Height
    });
    RootWindow->UpdateLayout();
    UE_LOG("InScreen Resolution : %d %d", static_cast<uint32>(InScreenWidth), static_cast<uint32>(InScreenHeight));
#pragma endregion
    //AllocConsole(); // 콘솔 창 생성

    //// 표준 출력 및 입력을 콘솔과 연결
    //freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    //freopen_s((FILE**)stdin, "CONIN$", "r", stdin);

    //std::cout << "Debug Console Opened!" << '\n';
}

void UEngine::InitRenderer()
{
	// 렌더러 초기화
	Renderer = std::make_unique<URenderer>();
	Renderer->Create(WindowHandle, ScreenWidth, ScreenHeight);
	Renderer->CreateShader();
	Renderer->CreateConstantBuffer();
}

void UEngine::InitWorld()
{
    World = FObjectFactory::ConstructObject<UWorld>();
    World->SceneName = "MainScene";
    FSceneManager::Get().AddScene(World);

    FEditorManager::Get().SetCamera(World->SpawnActor<ACamera>());

    //// Test
    //AArrow* Arrow = World->SpawnActor<AArrow>();
    //World->SpawnActor<ASphere>();
    
    World->SpawnActor<AAxis>();
    APicker* Picker = World->SpawnActor<APicker>();
    FEditorManager::Get().SetBoundingBox(Picker->GetBoundingBoxComp());
    FEditorManager::Get().SetStringComp(Picker->GetStringComponent());

    World->SpawnActor<AWorldGrid>();
    AWorldGizmo* WorldGizmo = World->SpawnActor<AWorldGizmo>();
	/*World->BeginPlay();*/
}

void UEngine::ShutdownWindow()
{
    DestroyWindow(WindowHandle);
    WindowHandle = nullptr;

    UnregisterClassW(WindowClassName, WindowInstance);
    WindowInstance = nullptr;

	ui.Shutdown();
}

void UEngine::UpdateWindowSize(UINT InScreenWidth, UINT InScreenHeight)
{
	ScreenWidth = InScreenWidth;
	ScreenHeight = InScreenHeight;

    if(Renderer)
    {
        Renderer->OnUpdateWindowSize(ScreenWidth, ScreenHeight);
    }

	if (ui.bIsInitialized)
	{
		ui.OnUpdateWindowSize(ScreenWidth, ScreenHeight);
	}

	if (Renderer)
	{
		Renderer->OnResizeComplete();
	}

    // [Added] 다중 뷰포트를 관리하는 Root window의 Rect Update
    if (RootWindow) 
    {
        RootWindow->SetRect({
            0, 0,                               // TopLeft, TopRight 
            static_cast<uint32>(InScreenWidth), // Width
            static_cast<uint32>(InScreenHeight) // Height
        });
        RootWindow->UpdateLayout();
    }
}

UObject* UEngine::GetObjectByUUID(uint32 InUUID) const
{
    if (const auto Obj = GObjects.Find(InUUID))
    {
        return Obj->get();
    }
    return nullptr;
}
