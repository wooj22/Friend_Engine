#pragma once
#include "../Friend_2D_EngineLib/GameObject.h"
#include "../Friend_2D_EngineLib/Transform.h"
#include "../Friend_2D_EngineLib/SpriteRenderer.h"
#include "../Friend_2D_EngineLib/Rigidbody.h"
#include "../Friend_2D_EngineLib/CircleCollider.h"
#include "../Friend_2D_EngineLib/BoxCollider.h"
#include "../Friend_2D_EngineLib/ResourceManager.h"
#include "ColliderControlScript.h"
#include "DemoConfig.h"

enum class DemoShape { Circle, Box };

/* [ColliderDemoObject]
* A collider demo object using the Resource/Circle.png or Resource/Box.png sprite.
* Depending on isTrigger, the same object works either as a Trigger collider or a
* Collision (physical) collider. The Box is moved with WASD, the Circle with the
* arrow keys (ColliderControlScript), so events can be triggered on demand.
*
* Trigger objects keep a kinematic Rigidbody (needed only because OnCollisionEnter
* requires a Rigidbody to be present) and are moved directly via Transform, so they
* never physically block - they can freely overlap.
* Collision (non-trigger) objects get a real, non-kinematic Rigidbody driven by
* velocity, so the engine's own collision resolution (Rigidbody::CorrectPosition,
* driven from BoxCollider/CircleCollider::OnCollisionEnter) actually stops them at
* contact - the same mechanism the CCD demo scene uses.
*/
class ColliderDemoObject : public GameObject
{
public:
	Transform* transform;
	SpriteRenderer* spriteRenderer;
	Rigidbody* rigidbody;
	ICollider* collider;
	ColliderControlScript* controller;

	ColliderDemoObject(DemoShape shape, bool isTrigger)
	{
		transform = AddComponent<Transform>();
		spriteRenderer = AddComponent<SpriteRenderer>();
		rigidbody = AddComponent<Rigidbody>();
		controller = AddComponent<ColliderControlScript>();

		transform->SetScale(DemoConfig::ColliderObjectScale, DemoConfig::ColliderObjectScale);

		rigidbody->isKinematic = isTrigger;
		rigidbody->useGravity = false;
		rigidbody->drag = 0.0f;

		spriteRenderer->renderMode = RenderMode::UnlitColorTint;
		spriteRenderer->layer = 10;

		if (shape == DemoShape::Circle)
		{
			auto texture = ResourceManager::Get().CreateTexture2D("../Resource/Circle.png");
			spriteRenderer->sprite = ResourceManager::Get().CreateSprite(texture, "Circle");

			CircleCollider* circleCollider = AddComponent<CircleCollider>();
			circleCollider->radius = DemoConfig::CircleRadius;
			circleCollider->isTrigger = isTrigger;
			collider = circleCollider;

			controller->inputScheme = DemoInputScheme::Arrows;
			name = "Circle";
		}
		else
		{
			auto texture = ResourceManager::Get().CreateTexture2D("../Resource/Box.png");
			spriteRenderer->sprite = ResourceManager::Get().CreateSprite(texture, "Box");

			BoxCollider* boxCollider = AddComponent<BoxCollider>();
			boxCollider->size = { DemoConfig::BoxSize, DemoConfig::BoxSize };
			boxCollider->isTrigger = isTrigger;
			collider = boxCollider;

			controller->inputScheme = DemoInputScheme::WASD;
			name = "Box";
		}

		tag = isTrigger ? "Trigger" : "Collision";
	}
};
