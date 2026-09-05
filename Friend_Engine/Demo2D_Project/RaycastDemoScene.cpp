#include "RaycastDemoScene.h"
#include "DemoConfig.h"
#include "../Friend_2D_EngineLib/GameObject.h"
#include "../Friend_2D_EngineLib/Transform.h"
#include "../Friend_2D_EngineLib/SpriteRenderer.h"
#include "../Friend_2D_EngineLib/BoxCollider.h"
#include "../Friend_2D_EngineLib/CircleCollider.h"
#include "../Friend_2D_EngineLib/ColliderSystem.h"
#include "../Friend_2D_EngineLib/RaycastHit.h"
#include "../Friend_2D_EngineLib/ResourceManager.h"
#include "../Friend_2D_EngineLib/Script.h"
#include "../Friend_2D_EngineLib/RectTransform.h"
#include "../Friend_2D_EngineLib/UI_Text.h"
#include "../Friend_2D_EngineLib/Input.h"
#include "../Friend_2D_EngineLib/Time.h"
#include "../Friend_2D_EngineLib/RenderSystem.h"
#include "../Friend_2D_EngineLib/Camera.h"
#include <sstream>
#include <iomanip>
#include <cmath>

namespace
{
	// A static, non-trigger obstacle the ray can hit.
	class RaycastTargetObject : public GameObject
	{
	public:
		Transform* transform;
		SpriteRenderer* spriteRenderer;
		ICollider* collider;

		RaycastTargetObject(bool isBox, const std::string& texturePath, const std::string& spriteName)
		{
			transform = AddComponent<Transform>();
			spriteRenderer = AddComponent<SpriteRenderer>();
			spriteRenderer->renderMode = RenderMode::UnlitColorTint;
			spriteRenderer->layer = 10;
			spriteRenderer->SetColor(1.0f, 1.0f, 1.0f);

			transform->SetScale(0.4f, 0.4f);   // 306px source -> ~122px on screen

			auto texture = ResourceManager::Get().CreateTexture2D(texturePath);
			spriteRenderer->sprite = ResourceManager::Get().CreateSprite(texture, spriteName);

			// BoxCollider::size / CircleCollider::radius are LOCAL units, multiplied
			// by transform->GetWorldScale() (see BoxCollider::UpdateBounds /
			// CircleCollider::UpdateBounds) - use the sprite's native 306x306 size
			// here so the 0.4 scale above shrinks the collider to match the visual
			// ~122px sprite, instead of shrinking an already-final-size value twice.
			if (isBox)
			{
				BoxCollider* b = AddComponent<BoxCollider>();
				b->size = { 306.0f, 306.0f };
				b->isTrigger = false;
				collider = b;
			}
			else
			{
				CircleCollider* c = AddComponent<CircleCollider>();
				c->radius = 153.0f;
				c->isTrigger = false;
				collider = c;
			}
		}
	};

	// Moves with WASD, aims with Left/Right, casts a ray every frame and
	// visualizes it with a debug line + hit marker.
	class RaycastEmitterScript : public Script
	{
	private:
		Transform* tr = nullptr;
		UI_Text* hud = nullptr;
		ICollider* lastHit = nullptr;
		float angle = 0.0f;   // radians, 0 = facing +x

	public:
		void Awake() override
		{
			tr = gameObject->transform;

			hud = Instantiate<UI_Text>();
			hud->rectTransform->SetPosition(0.0f, -(DemoConfig::WindowHeight / 2.0f - 40.0f));
			hud->rectTransform->SetSize((float)DemoConfig::WindowWidth, 40.0f);
			hud->screenTextRenderer->SetFontSize(20.0f);
			hud->screenTextRenderer->SetColor(D2D1::ColorF(D2D1::ColorF::White));
		}

		void Update() override
		{
			// move emitter
			float mx = 0.0f, my = 0.0f;
			if (Input::GetKey('A')) mx -= 1.0f;
			if (Input::GetKey('D')) mx += 1.0f;
			if (Input::GetKey('W')) my += 1.0f;
			if (Input::GetKey('S')) my -= 1.0f;

			Vector2 move(mx, my);
			if (!move.IsZero())
				tr->Translate(move.Normalized() * 220.0f * Time::GetDeltaTime());

			// aim
			if (Input::GetKey(VK_LEFT))  angle += 2.2f * Time::GetDeltaTime();
			if (Input::GetKey(VK_RIGHT)) angle -= 2.2f * Time::GetDeltaTime();

			Vector2 origin = tr->GetWorldPosition();
			Vector2 direction(cosf(angle), sinf(angle));

			Ray ray;
			ray.origin = origin;
			ray.direction = direction;
			RaycastHit hit = ColliderSystem::Get().Raycast(ray, 2000.0f);

			// un-tint whatever we hit last frame if it's no longer the target
			if (lastHit && lastHit != hit.collider)
			{
				SpriteRenderer* prevRenderer = lastHit->gameObject->GetComponent<SpriteRenderer>();
				if (prevRenderer) prevRenderer->SetColor(1.0f, 1.0f, 1.0f);
			}

			Vector2 endPoint = hit.collider ? hit.point : (origin + direction * 2000.0f);

			const auto screenMatrix = Transform::renderMatrix * Camera::GetMainInverseMatrix() * Transform::unityMatrix;
			RenderSystem::Get().DebugDrawLine({ origin.x, origin.y }, { endPoint.x, endPoint.y }, screenMatrix, 2.0f);

			std::wostringstream oss;
			oss << std::fixed << std::setprecision(0);

			if (hit.collider)
			{
				SpriteRenderer* renderer = hit.collider->gameObject->GetComponent<SpriteRenderer>();
				if (renderer) renderer->SetColor(1.0f, 0.25f, 0.25f);

				// NOTE: DebugDrawLine negates Y internally ("y 반전" in RenderSystem::DebugDrawLine),
				// but DebugDrawCircle/DebugDrawRect do not - they expect the caller to pre-negate
				// (see CircleCollider::DebugColliderDraw, which builds its local point as
				// (offset.x, -offset.y)). Negate here too so the marker lines up with the ray.
				D2D1_ELLIPSE marker = D2D1::Ellipse(D2D1::Point2F(0.0f, 0.0f), 8.0f, 8.0f);
				auto markerMatrix = D2D1::Matrix3x2F::Translation(hit.point.x, -hit.point.y) * screenMatrix;
				RenderSystem::Get().DebugDrawCircle(marker, markerMatrix, 2.0f);

				std::string objName = hit.collider->gameObject->name;
				std::wstring wideName(objName.begin(), objName.end());
				oss << L"Hit GameObject: " << wideName << L"    Distance: " << hit.distance;
			}
			else
			{
				oss << L"충돌 발생 x";
			}

			hud->screenTextRenderer->SetText(oss.str());
			lastHit = hit.collider;
		}
	};
}

std::wstring RaycastDemoScene::GetTitleText()
{
	return L"Raycast Demo";
}

std::wstring RaycastDemoScene::GetInstructionsText()
{
	return L"이동: WASD    조준: ← / →    [1~9] 씬 전환";
}

void RaycastDemoScene::SpawnDemoObjects()
{
	// emitter
	GameObject* emitter = CreateObject<GameObject>();
	emitter->name = "Emitter";
	Transform* emitterTransform = emitter->AddComponent<Transform>();
	emitterTransform->SetPosition(-420.0f, 0.0f);
	emitterTransform->SetScale(0.12f, 0.12f);
	SpriteRenderer* emitterRenderer = emitter->AddComponent<SpriteRenderer>();
	auto emitterTexture = ResourceManager::Get().CreateTexture2D("../Resource/Camera.png");
	emitterRenderer->sprite = ResourceManager::Get().CreateSprite(emitterTexture, "RaycastEmitter");
	emitterRenderer->layer = 10;
	emitter->AddComponent<RaycastEmitterScript>();

	// targets
	RaycastTargetObject* box1 = CreateObject<RaycastTargetObject>({ 260.0f, 150.0f }, nullptr, true, "../Resource/Box.png", "Box");
	box1->name = "Box";

	RaycastTargetObject* circle1 = CreateObject<RaycastTargetObject>({ -180.0f, -140.0f }, nullptr, false, "../Resource/Circle.png", "Circle");
	circle1->name = "Circle";

	RaycastTargetObject* box2 = CreateObject<RaycastTargetObject>({ 320.0f, -170.0f }, nullptr, true, "../Resource/Box.png", "Box");
	box2->name = "Box";
}
