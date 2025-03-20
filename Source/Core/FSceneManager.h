#pragma once
#include "unordered_map"
#include "Core/AbstractClass/Singleton.h"
#include "../EEnum.h"
#include "Core/HAL/PlatformType.h"

class UWorld;
class FSceneManager : public TSingleton<FSceneManager>
{
public:
	int GetSceneSize() { return worldMap.size(); }
	void AddScene(UWorld* world); UWorld* GetScene(uint32 index) { return worldMap[index]; }
	void SetCurSceneIndex(uint32 value) { curSceneIndex = value; } uint32 GetCurSceneIndex() const { return curSceneIndex; }
	void ToggleShowFlag(EShowFlag showFlag) { showFlagMasking ^= showFlag; }
	uint32 GetShowFlagMask() const { return showFlagMasking; }

private:
	std::unordered_map<uint32, UWorld*> worldMap;
	inline static uint32 showFlagMasking = 0xFFFFFFFF;
	inline static uint32 curSceneIndex = 0;
};