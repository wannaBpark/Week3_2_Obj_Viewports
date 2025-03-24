#pragma once

#include <memory>

#include "HAL/PlatformType.h"
#include "Rendering/URenderer.h"
#include "Rendering/UI/UI.h"
#include "AbstractClass/Singleton.h"
#include "Container/Map.h"
#include "Core/Container/Array.h"

class UObject;
class UWorld;

enum class EScreenMode : uint8
{
    Windowed,    // 창 모드
    Fullscreen,  // 전체화면 모드
    Borderless,  // 테두리 없는 창 모드
};

enum class EViewMode : uint8
{
    Unlit,
    Lit,
	Wireframe,
};

class UEngine : public TSingleton<UEngine>
{
public:
    // 각종 윈도우 관련 메시지(이벤트)를 처리하는 함수
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    /**
     * Application을 초기화 합니다.
     * @param hInstance 창 인스턴스
     * @param InWindowTitle 윈도우 창 이름
     * @param InWindowClassName 윈도우 클래스 이름
     * @param InScreenWidth 화면 너비
     * @param InScreenHeight 화면 높이
     * @param InScreenMode 창 모드
     * @return 초기화 여부
     */
    void Initialize(
        HINSTANCE hInstance, const WCHAR* InWindowTitle, const WCHAR* InWindowClassName, int InScreenWidth,
        int InScreenHeight, EScreenMode InScreenMode = EScreenMode::Windowed
    );
    void Run();

    /**
     * Application에서 사용한 자원을 정리합니다.
     */
    void Shutdown();

	class URenderer* GetRenderer() const { return Renderer.get(); }
	float GetScreenRatio() const { return static_cast<float>(ScreenWidth) / ScreenHeight; }
    int GetScreenWidth() const { return ScreenWidth; }
    int GetScreenHeight() const { return ScreenHeight; }
    int GetInitializedScreenWidth() const { return InitializedScreenWidth; }
    int GetInitializedScreenHeight() const { return InitializedScreenHeight; }
    

private:
    void InitWindow(int InScreenWidth, int InScreenHeight);
    void InitRenderer();
    void InitWorld();
    void ShutdownWindow();
    void UpdateWindowSize(UINT InScreenWidth, UINT InScreenHeight);

public:
	UWorld* GetWorld() const { return World; }

    HWND GetWindowHandle() const { return WindowHandle; }

    template <typename ObjectType>
        requires std::derived_from<ObjectType, UObject>
    ObjectType* GetObjectByUUID(uint32 InUUID) const;
    UObject* GetObjectByUUID(uint32 InUUID) const;

private:
    bool IsRunning = false;
    EScreenMode ScreenMode = EScreenMode::Windowed;

    const WCHAR* WindowTitle = nullptr;
    const WCHAR* WindowClassName = nullptr;
    HWND WindowHandle = nullptr;
    HINSTANCE WindowInstance = nullptr;

    int InitializedScreenWidth = 0;
    int InitializedScreenHeight = 0;

    int ScreenWidth = 0;
    int ScreenHeight = 0;

    

private:
	std::unique_ptr<URenderer> Renderer;

private:
	UI ui;

private:
    class UWorld* World;

public:
    // TArray<std::shared_ptr<UObject>> GObjects;
    TMap<uint32, std::shared_ptr<UObject>> GObjects;

private:
    // private 생성자를 갖는 클래스의 인스턴스를 unique_ptr로 선언할 수 없음. Shutdown시 해제 필요
    class FEngineConfig* EngineConfig;
public:
	FEngineConfig* GetEngineConfig() const { return EngineConfig; }
};

template <typename ObjectType> requires std::derived_from<ObjectType, UObject>
ObjectType* UEngine::GetObjectByUUID(uint32 InUUID) const
{
    if (const std::shared_ptr<UObject>* Obj = GObjects.Find(InUUID))
    {
        if (const auto PriComp = std::dynamic_pointer_cast<ObjectType, UObject>(*Obj))
        {
            return PriComp.get();
        }
    }
    return nullptr;
}
