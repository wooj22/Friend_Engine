#include "RigidbodyDemoScene.h"
#include "DemoConfig.h"
#include "../Friend_2D_EngineLib/GameObject.h"
#include "../Friend_2D_EngineLib/Transform.h"
#include "../Friend_2D_EngineLib/SpriteRenderer.h"
#include "../Friend_2D_EngineLib/Rigidbody.h"
#include "../Friend_2D_EngineLib/BoxCollider.h"
#include "../Friend_2D_EngineLib/ResourceManager.h"
#include "../Friend_2D_EngineLib/Script.h"
#include "../Friend_2D_EngineLib/RectTransform.h"
#include "../Friend_2D_EngineLib/UI_Text.h"
#include "../Friend_2D_EngineLib/Input.h"
#include "../Friend_2D_EngineLib/Time.h"
#include <sstream>
#include <iomanip>

namespace
{
	constexpr float GravityScale = 90.0f;   // engine gravity is a tiny 9.8 px/s^2 by default - scaled up so the fall reads like a real platformer character
	constexpr float GroundY = -250.0f;
	constexpr float GroundTopY = GroundY + 20.0f; // half of the 40px-tall ground

	// WASD-less: [A]/[D] apply a horizontal force (drag slows it back down),
	// [Space] applies an upward impulse (jump) while grounded. Grounded state is
	// tracked locally from OnCollisionEnter/Stay/Exit since Rigidbody::isGrounded
	// is private to the engine.
	class RigidbodyPlayerScript : public Script
	{
	private:
		Transform* tr = nullptr;
		Rigidbody* rb = nullptr;
		UI_Text* hud = nullptr;
		bool grounded = false;

	public:
		// NOTE: Rigidbody::AddImpulse stores impulse/mass, then FixedUpdate adds
		// impulse/mass again on top of that - so the impulse's actual effect on
		// velocity is jumpImpulse/(mass*mass), not jumpImpulse/mass. Tuned around
		// that so a heavier box still gets a strong, visible jump.
		float thrustForce = 3600.0f;
		float jumpImpulse = 6300.0f;

		void Awake() override
		{
			tr = gameObject->transform;
			rb = gameObject->GetComponent<Rigidbody>();

			hud = Instantiate<UI_Text>();
			hud->rectTransform->SetPosition(0.0f, -(DemoConfig::WindowHeight / 2.0f - 40.0f));
			hud->rectTransform->SetSize((float)DemoConfig::WindowWidth, 40.0f);
			hud->screenTextRenderer->SetFontSize(20.0f);
			hud->screenTextRenderer->SetColor(D2D1::ColorF(D2D1::ColorF::White));
		}

		void Update() override
		{
			float x = 0.0f;
			if (Input::GetKey('A')) x -= 1.0f;
			if (Input::GetKey('D')) x += 1.0f;
			if (x != 0.0f) rb->AddForce(Vector2(x * thrustForce, 0.0f));

			if (grounded && Input::GetKeyDown(VK_SPACE))
			{
				rb->AddImpulse(Vector2(0.0f, jumpImpulse));
			}

			std::wostringstream oss;
			oss << std::fixed << std::setprecision(0);
			oss << L"Velocity: (" << rb->velocity.x << L", " << rb->velocity.y << L")    Grounded: " << (grounded ? L"true" : L"false");
			hud->screenTextRenderer->SetText(oss.str());
		}

		void OnCollisionEnter(ICollider* other, const ContactInfo& contact) override
		{
			if (contact.normal.y > 0.0f) grounded = true;
		}

		void OnCollisionStay(ICollider* other, const ContactInfo& contact) override
		{
			if (contact.normal.y > 0.0f) grounded = true;
		}

		void OnCollisionExit(ICollider* other, const ContactInfo& contact) override
		{
			if (contact.normal.y > 0.0f) grounded = false;
		}
	};
}

std::wstring RigidbodyDemoScene::GetTitleText()
{
	return L"Rigidbody Demo (Gravity / Force / Impulse / Drag)";
}

std::wstring RigidbodyDemoScene::GetInstructionsText()
{
	return L"플레이어 박스: [A]/[D] 힘, [Space] 점프    [1~9] 씬 전환";
}

void RigidbodyDemoScene::SpawnDemoObjects()
{
	// ground
	GameObject* ground = CreateObject<GameObject>();
	ground->name = "Ground";
	Transform* groundTransform = ground->AddComponent<Transform>();
	groundTransform->SetPosition(0.0f, GroundY);
	groundTransform->SetScale(1200.0f / 306.0f, 40.0f / 306.0f);
	SpriteRenderer* groundRenderer = ground->AddComponent<SpriteRenderer>();
	auto groundTexture = ResourceManager::Get().CreateTexture2D("../Resource/Box.png");
	groundRenderer->sprite = ResourceManager::Get().CreateSprite(groundTexture, "Box");
	groundRenderer->layer = 10;
	BoxCollider* groundCollider = ground->AddComponent<BoxCollider>();
	groundCollider->size = { 306.0f, 306.0f }; // native size - scaled by transform above to 1200x40
	groundCollider->isTrigger = false;

	// player box: force + drag + jump impulse
	GameObject* player = CreateObject<GameObject>();
	player->name = "PlayerBox";
	Transform* playerTransform = player->AddComponent<Transform>();
	playerTransform->SetPosition(0.0f, GroundTopY + 30.6f);
	playerTransform->SetScale(0.2f, 0.2f);
	SpriteRenderer* playerRenderer = player->AddComponent<SpriteRenderer>();
	auto boxTexture = ResourceManager::Get().CreateTexture2D("../Resource/Box.png");
	playerRenderer->sprite = ResourceManager::Get().CreateSprite(boxTexture, "Box");
	playerRenderer->layer = 10;
	Rigidbody* playerRb = player->AddComponent<Rigidbody>();
	playerRb->isKinematic = false;
	playerRb->useGravity = true;
	playerRb->gravityScale = GravityScale;
	playerRb->mass = 3.0f;
	playerRb->drag = 0.05f;
	BoxCollider* playerCollider = player->AddComponent<BoxCollider>();
	playerCollider->size = { 306.0f, 306.0f };
	playerCollider->isTrigger = false;
	player->AddComponent<RigidbodyPlayerScript>();
}
