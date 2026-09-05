#pragma once
#include <vector>
#include "DemoSceneBase.h"

class UI_Text;

/* Scene 7 : ResourceManager cache / object lifetime
* [Q] spawns a batch of sprites that all reuse ONE cached texture (Circle.png) -
*     watch VRAM barely move no matter how many are spawned.
* [W] spawns a batch of sprites that each load a DIFFERENT texture from a small
*     pool of Resource/ images - watch VRAM actually grow.
* [E] destroys every spawned object and calls ResourceManager::Trim() so the
*     freed GPU memory is visibly reclaimed.
* The HUD reports the live object count plus ResourceManager::GetMemoryUsageWString().
*/
class ResourceLifetimeScene : public DemoSceneBase
{
private:
	std::vector<GameObject*> spawnedObjects;
	UI_Text* hud = nullptr;
	float hudTimer = 0.0f;
	int uniqueTextureCursor = 0;

	void SpawnShared(int count);
	void SpawnUnique(int count);
	void ClearAll();
	void RefreshHud();

protected:
	std::wstring GetTitleText() override;
	std::wstring GetInstructionsText() override;
	void SpawnDemoObjects() override;

public:
	void Update() override;
};
