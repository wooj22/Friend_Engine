#include "DemoApp.h"
#include "../Friend_2D_EngineLib/RectTransform.h"
#include "TriggerTriggerScene.h"
#include "TriggerCollisionScene.h"
#include "CollisionCollisionScene.h"
#include "HierarchyDemoScene.h"
#include "RaycastDemoScene.h"
#include "CCDDemoScene.h"
#include "ResourceLifetimeScene.h"
#include "AnimationFSMScene.h"
#include "RigidbodyDemoScene.h"

void DemoApp::Init()
{
	__super::Init();

	// world (0,0) -> screen center, y-up (unity style coordinates)
	Transform::unityMatrix =
		D2D1::Matrix3x2F::Scale(1.0f, -1.0f) * Transform::unityMatrix *
		D2D1::Matrix3x2F::Translation(width / 2.0f, height / 2.0f);
	RectTransform::unityMatrix =
		D2D1::Matrix3x2F::Scale(1.0f, -1.0f) * D2D1::Matrix3x2F::Translation(width / 2.0f, height / 2.0f);

	// scene init - registration order must match DemoApp::SceneName
	sceneManager.CreateScene<TriggerTriggerScene>();
	sceneManager.CreateScene<TriggerCollisionScene>();
	sceneManager.CreateScene<CollisionCollisionScene>();
	sceneManager.CreateScene<HierarchyDemoScene>();
	sceneManager.CreateScene<RaycastDemoScene>();
	sceneManager.CreateScene<CCDDemoScene>();
	sceneManager.CreateScene<ResourceLifetimeScene>();
	sceneManager.CreateScene<AnimationFSMScene>();
	sceneManager.CreateScene<RigidbodyDemoScene>();
	sceneManager.SetCurrentScene(TRIGGER_TRIGGER);
	sceneManager.Init();
}
