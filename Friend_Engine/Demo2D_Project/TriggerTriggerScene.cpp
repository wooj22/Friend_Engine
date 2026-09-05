#include "TriggerTriggerScene.h"
#include "ColliderDemoObject.h"
#include "DemoConfig.h"

std::wstring TriggerTriggerScene::GetTitleText()
{
	return L"Trigger - Trigger 충돌";
}

void TriggerTriggerScene::SpawnDemoObjects()
{
	// Circle - Trigger
	CreateObject<ColliderDemoObject>({ DemoConfig::SpawnOffsetX, 0.0f }, nullptr, DemoShape::Circle, true);

	// Box - Trigger
	CreateObject<ColliderDemoObject>({ -DemoConfig::SpawnOffsetX, 0.0f }, nullptr, DemoShape::Box, true);
}
