#include "HierarchyDemoScene.h"
#include "DemoConfig.h"
#include "../Friend_2D_EngineLib/GameObject.h"
#include "../Friend_2D_EngineLib/Transform.h"
#include "../Friend_2D_EngineLib/SpriteRenderer.h"
#include "../Friend_2D_EngineLib/ResourceManager.h"
#include "../Friend_2D_EngineLib/Script.h"
#include "../Friend_2D_EngineLib/RectTransform.h"
#include "../Friend_2D_EngineLib/UI_Text.h"
#include "../Friend_2D_EngineLib/Input.h"
#include "../Friend_2D_EngineLib/Time.h"
#include <sstream>
#include <iomanip>
#include <cmath>

namespace
{
	enum class HierarchyMode { PlayerControlled, Orbiting };

	// Moves a node either by WASD (root) or by orbiting its parent (child/grandchild),
	// and keeps a small on-screen label showing its Local vs World position.
	class HierarchyNodeScript : public Script
	{
	private:
		Transform* tr = nullptr;
		UI_Text* label = nullptr;
		float angle = 0.0f;

	public:
		HierarchyMode mode = HierarchyMode::PlayerControlled;
		float orbitRadius = 0.0f;
		float orbitSpeed = 1.0f;    // radians/sec
		float moveSpeed = DemoConfig::MoveSpeed;
		std::wstring displayName = L"Node";

		void Awake() override
		{
			tr = gameObject->transform;

			// start the orbit angle from the initial local position so there is no pop
			Vector2 startLocal = tr->GetPosition();
			angle = atan2f(startLocal.y, startLocal.x);

			label = Instantiate<UI_Text>();
			label->screenTextRenderer->SetFontSize(14.0f);
			label->screenTextRenderer->SetColor(D2D1::ColorF(D2D1::ColorF::White));
			label->screenTextRenderer->layer = 20;
			label->rectTransform->SetSize(240.0f, 70.0f);
		}

		void Update() override
		{
			if (mode == HierarchyMode::PlayerControlled)
			{
				float x = 0.0f, y = 0.0f;
				if (Input::GetKey('A')) x -= 1.0f;
				if (Input::GetKey('D')) x += 1.0f;
				if (Input::GetKey('W')) y += 1.0f;
				if (Input::GetKey('S')) y -= 1.0f;

				Vector2 in(x, y);
				if (!in.IsZero())
					tr->Translate(in.Normalized() * moveSpeed * Time::GetDeltaTime());
			}
			else
			{
				angle += orbitSpeed * Time::GetDeltaTime();
				tr->SetPosition(orbitRadius * cosf(angle), orbitRadius * sinf(angle));
			}

			Vector2 world = tr->GetWorldPosition();
			Vector2 local = tr->GetPosition();

			std::wostringstream oss;
			oss << std::fixed << std::setprecision(0);
			oss << displayName
				<< L"\nWorld: (" << world.x << L", " << world.y << L")"
				<< L"\nLocal: (" << local.x << L", " << local.y << L")";

			label->rectTransform->SetPosition(world.x, world.y - 70.0f);
			label->screenTextRenderer->SetText(oss.str());
		}
	};

	// A GameObject with its Transform/SpriteRenderer already attached in the
	// constructor, so Scene::CreateObject's automatic position/parent wiring
	// (which reads GetComponent<Transform>() right after construction) works.
	class HierarchyNodeObject : public GameObject
	{
	public:
		Transform* transform;
		SpriteRenderer* spriteRenderer;
		HierarchyNodeScript* script;

		HierarchyNodeObject(const std::string& texturePath, const std::string& spriteName, float scale)
		{
			transform = AddComponent<Transform>();
			spriteRenderer = AddComponent<SpriteRenderer>();
			script = AddComponent<HierarchyNodeScript>();

			transform->SetScale(scale, scale);

			auto texture = ResourceManager::Get().CreateTexture2D(texturePath);
			spriteRenderer->sprite = ResourceManager::Get().CreateSprite(texture, spriteName);
			spriteRenderer->layer = 10;
		}
	};
}

std::wstring HierarchyDemoScene::GetTitleText()
{
	return L"Transform Hierarchy Demo (Local → World)";
}

std::wstring HierarchyDemoScene::GetInstructionsText()
{
	return L"태양: WASD로 이동    지구/달은 자동 공전    [1~9] 씬 전환";
}

void HierarchyDemoScene::SpawnDemoObjects()
{
	// Sun - root, moved directly by the player
	HierarchyNodeObject* sun = CreateObject<HierarchyNodeObject>(
		{ 0.0f, 0.0f }, nullptr, "../Resource/Sun.png", "Sun", 1.5f);
	sun->name = "Sun";
	sun->script->displayName = L"태양 (부모)";
	sun->script->mode = HierarchyMode::PlayerControlled;
	sun->script->moveSpeed = 250.0f;

	// Earth - orbits the Sun (child)
	HierarchyNodeObject* earth = CreateObject<HierarchyNodeObject>(
		{ 180.0f, 0.0f }, sun, "../Resource/Earth.png", "Earth", 1.3f);
	earth->name = "Earth";
	earth->script->displayName = L"지구 (자식)";
	earth->script->mode = HierarchyMode::Orbiting;
	earth->script->orbitRadius = 180.0f;
	earth->script->orbitSpeed = 0.6f;

	// Moon - orbits the Earth (grandchild)
	HierarchyNodeObject* moon = CreateObject<HierarchyNodeObject>(
		{ 55.0f, 0.0f }, earth, "../Resource/Moon.png", "Moon", 1.8f);
	moon->name = "Moon";
	moon->script->displayName = L"달 (손자)";
	moon->script->mode = HierarchyMode::Orbiting;
	moon->script->orbitRadius = 55.0f;
	moon->script->orbitSpeed = 2.2f;
}
