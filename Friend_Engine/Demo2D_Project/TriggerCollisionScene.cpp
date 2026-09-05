#include "TriggerCollisionScene.h"
#include "ColliderDemoObject.h"
#include "DemoConfig.h"

std::wstring TriggerCollisionScene::GetTitleText()
{
	return L"Trigger - Collision 충돌";
}

void TriggerCollisionScene::SpawnDemoObjects()
{
	// Circle - Trigger
	CreateObject<ColliderDemoObject>({ DemoConfig::SpawnOffsetX, 0.0f }, nullptr, DemoShape::Circle, true);

	// Box - Collision
	CreateObject<ColliderDemoObject>({ -DemoConfig::SpawnOffsetX, 0.0f }, nullptr, DemoShape::Box, false);
}
