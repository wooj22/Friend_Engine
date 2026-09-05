#pragma once
#include "../Friend_2D_EngineLib/GameApp.h"
#include "DemoConfig.h"

class DemoApp : public GameApp

{
public:
	// scene index (must match the registration order in SceneManager::CreateScene())
	enum SceneName
	{
		TRIGGER_TRIGGER,
		TRIGGER_COLLISION,
		COLLISION_COLLISION,
		TRANSFORM_HIERARCHY,
		RAYCAST,
		CCD,
		RESOURCE_LIFETIME,
		ANIMATION_FSM,
		RIGIDBODY
	};

	DemoApp(int inputWidth = DemoConfig::WindowWidth, int inputHeight = DemoConfig::WindowHeight)
		: GameApp(inputWidth, inputHeight) {
	}
	~DemoApp() override = default;

	void Init() override;
};
