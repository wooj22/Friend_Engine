#include "CCDDemoScene.h"
#include "DemoConfig.h"
#include "../Friend_2D_EngineLib/GameObject.h"
#include "../Friend_2D_EngineLib/Transform.h"
#include "../Friend_2D_EngineLib/SpriteRenderer.h"
#include "../Friend_2D_EngineLib/Rigidbody.h"
#include "../Friend_2D_EngineLib/CircleCollider.h"
#include "../Friend_2D_EngineLib/BoxCollider.h"
#include "../Friend_2D_EngineLib/ResourceManager.h"
#include "../Friend_2D_EngineLib/Script.h"
#include "../Friend_2D_EngineLib/RectTransform.h"
#include "../Friend_2D_EngineLib/UI_Text.h"
#include "../Friend_2D_EngineLib/Input.h"
#include "../Friend_2D_EngineLib/Time.h"
#include <sstream>

namespace
{
	constexpr float BulletStartX = -450.0f;
	constexpr float BulletSpeed = 3000.0f;     // fixed step (0.02s) -> 60px/step
	constexpr float WallX = 0.0f;
	constexpr float WallHalfThickness = 10.0f; // size.x = 20 -> smaller than the 60px/step jump

	// Fires a bullet at the wall, reports Discrete vs Continuous outcome on the HUD,
	// and loops (relaunch) so the behaviour can be watched repeatedly.
	class CCDBulletScript : public Script
	{
	private:
		Transform* tr = nullptr;
		Rigidbody* rb = nullptr;
		SpriteRenderer* renderer = nullptr;
		UI_Text* hud = nullptr;

		Vector2 startPosition;
		bool waitingToReset = false;
		float pauseTimer = 0.0f;

		void Launch()
		{
			tr->SetPosition(startPosition);
			rb->velocity = Vector2(BulletSpeed, 0.0f);
			if (renderer) renderer->SetColor(1.0f, 1.0f, 1.0f);
			waitingToReset = false;
			pauseTimer = 0.0f;
		}

	public:
		void Awake() override
		{
			tr = gameObject->transform;
			rb = gameObject->GetComponent<Rigidbody>();
			renderer = gameObject->GetComponent<SpriteRenderer>();
			startPosition = tr->GetPosition();

			hud = Instantiate<UI_Text>();
			hud->rectTransform->SetPosition(0.0f, -(DemoConfig::WindowHeight / 2.0f - 40.0f));
			hud->rectTransform->SetSize((float)DemoConfig::WindowWidth, 60.0f);
			hud->screenTextRenderer->SetFontSize(22.0f);
			hud->screenTextRenderer->SetColor(D2D1::ColorF(D2D1::ColorF::White));

			Launch();
		}

		void Update() override
		{
			if (Input::GetKeyDown(VK_SPACE))
			{
				rb->collisionDetection = (rb->collisionDetection == CollisionDetection::Discrete)
					? CollisionDetection::Continuous : CollisionDetection::Discrete;
				Launch();
			}

			bool blocked = rb->velocity.SqrMagnitude() < 1.0f && tr->GetPosition().x < WallX;
			bool passedThrough = tr->GetPosition().x > WallX + 150.0f;

			if (blocked && renderer) renderer->SetColor(1.0f, 0.25f, 0.25f);

			if (blocked || passedThrough)
			{
				if (!waitingToReset)
				{
					waitingToReset = true;
					pauseTimer = 0.0f;
				}
				pauseTimer += Time::GetDeltaTime();
				if (pauseTimer >= 1.0f) Launch();
			}

			std::wstring modeText = (rb->collisionDetection == CollisionDetection::Discrete) ? L"Discrete" : L"Continuous";
			std::wstring resultText = passedThrough ? L"터널링 발생 (벽 통과)"
				: (blocked ? L"벽에 막혔습니다" : L"이동 중...");

			std::wostringstream oss;
			oss << L"모드: " << modeText;
			hud->screenTextRenderer->SetText(oss.str());
		}
	};
}

std::wstring CCDDemoScene::GetTitleText()
{
	return L"CCD (Continuous Collision Detection) Demo";
}

std::wstring CCDDemoScene::GetInstructionsText()
{
	return L"[Space] Discrete / Continuous 전환    [1~9] 씬 전환";
}

void CCDDemoScene::SpawnDemoObjects()
{
	// wall (thin, Box.png stretched vertically)
	GameObject* wall = CreateObject<GameObject>();
	wall->name = "Wall";
	Transform* wallTransform = wall->AddComponent<Transform>();
	wallTransform->SetPosition(WallX, 0.0f);
	wallTransform->SetScale((WallHalfThickness * 2.0f) / 306.0f, 400.0f / 306.0f);
	SpriteRenderer* wallRenderer = wall->AddComponent<SpriteRenderer>();
	auto wallTexture = ResourceManager::Get().CreateTexture2D("../Resource/Box.png");
	wallRenderer->sprite = ResourceManager::Get().CreateSprite(wallTexture, "Box");
	wallRenderer->layer = 10;
	BoxCollider* wallCollider = wall->AddComponent<BoxCollider>();
	// BoxCollider::size is in LOCAL units and gets multiplied by the transform's
	// scale (see BoxCollider::UpdateBounds) - use the sprite's native 306x306
	// size here so the same non-uniform scale above shrinks the collider to
	// exactly match the visual 20x400 wall, instead of shrinking it twice.
	wallCollider->size = { 306.0f, 306.0f };
	wallCollider->isTrigger = false;

	// bullet
	GameObject* bullet = CreateObject<GameObject>();
	bullet->name = "Bullet";
	Transform* bulletTransform = bullet->AddComponent<Transform>();
	bulletTransform->SetPosition(BulletStartX, 0.0f);
	bulletTransform->SetScale(30.0f / 306.0f, 30.0f / 306.0f);
	SpriteRenderer* bulletRenderer = bullet->AddComponent<SpriteRenderer>();
	bulletRenderer->renderMode = RenderMode::UnlitColorTint;
	auto bulletTexture = ResourceManager::Get().CreateTexture2D("../Resource/Circle.png");
	bulletRenderer->sprite = ResourceManager::Get().CreateSprite(bulletTexture, "Circle");
	bulletRenderer->layer = 10;

	Rigidbody* bulletRb = bullet->AddComponent<Rigidbody>();
	bulletRb->isKinematic = false;
	bulletRb->useGravity = false;
	bulletRb->drag = 0.0f;
	bulletRb->collisionDetection = CollisionDetection::Discrete;

	CircleCollider* bulletCollider = bullet->AddComponent<CircleCollider>();
	// same reasoning as the wall: radius is scaled by transform->GetWorldScale(),
	// so use the native half-size (153) here, not the desired 15px world radius.
	bulletCollider->radius = 153.0f;
	bulletCollider->isTrigger = false;

	bullet->AddComponent<CCDBulletScript>();
}
