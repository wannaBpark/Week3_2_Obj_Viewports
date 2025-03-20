#include "FSceneManager.h"
#include "Object/World/World.h"

void FSceneManager::AddScene(UWorld* world)
{
	uint32 size = worldMap.size();
	worldMap.insert({ size, world });
}
