#include "CollisionCollisionScene.h"
#include "ColliderDemoObject.h"
#include "DemoConfig.h"

std::wstring CollisionCollisionScene::GetTitleText()
{
	return L"Collision - Collision 충돌";
}

void CollisionCollisionScene::SpawnDemoObjects()
{
	// Circle - Collision
	CreateObject<ColliderDemoObject>({ DemoConfig::SpawnOffsetX, 0.0f }, nullptr, DemoShape::Circle, false);

	// Box - Collision
	CreateObject<ColliderDemoObject>({ -DemoConfig::SpawnOffsetX, 0.0f }, nullptr, DemoShape::Box, false);
}
