#include "AnimationFSMScene.h"
#include "DemoConfig.h"
#include "CatAnimatorData.h"
#include "../Friend_2D_EngineLib/GameObject.h"
#include "../Friend_2D_EngineLib/Transform.h"
#include "../Friend_2D_EngineLib/SpriteRenderer.h"
#include "../Friend_2D_EngineLib/Animator.h"
#include "../Friend_2D_EngineLib/Script.h"
#include "../Friend_2D_EngineLib/RectTransform.h"
#include "../Friend_2D_EngineLib/UI_Text.h"
#include "../Friend_2D_EngineLib/Input.h"
#include "../Friend_2D_EngineLib/Time.h"

namespace
{
	// Moves the cat with WASD and drives the Animator's "Walk"/"Run" bool
	// parameters; the FSM (CatIdle/Walk/RunState) reads those and switches clips.
	class CatControlScript : public Script
	{
	private:
		Transform* tr = nullptr;
		SpriteRenderer* renderer = nullptr;
		Animator* animator = nullptr;
		UI_Text* hud = nullptr;

	public:
		void Awake() override
		{
			tr = gameObject->transform;
			renderer = gameObject->GetComponent<SpriteRenderer>();
			animator = gameObject->GetComponent<Animator>();

			hud = Instantiate<UI_Text>();
			hud->rectTransform->SetPosition(0.0f, -(DemoConfig::WindowHeight / 2.0f - 40.0f));
			hud->rectTransform->SetSize((float)DemoConfig::WindowWidth, 40.0f);
			hud->screenTextRenderer->SetFontSize(20.0f);
			hud->screenTextRenderer->SetColor(D2D1::ColorF(D2D1::ColorF::White));
		}

		void Update() override
		{
			float x = 0.0f, y = 0.0f;
			if (Input::GetKey('A')) x -= 1.0f;
			if (Input::GetKey('D')) x += 1.0f;
			if (Input::GetKey('W')) y += 1.0f;
			if (Input::GetKey('S')) y -= 1.0f;

			Vector2 dir(x, y);
			bool moving = !dir.IsZero();
			bool running = moving && Input::GetKey(VK_SHIFT);

			AnimatorController* controller = animator ? animator->controller : nullptr;
			if (controller)
			{
				controller->SetBool("Walk", moving && !running);
				controller->SetBool("Run", running);
			}

			if (moving)
			{
				float speed = running ? 420.0f : 200.0f;
				Vector2 n = dir.Normalized();
				tr->Translate(n * speed * Time::GetDeltaTime());
				if (renderer) renderer->flipX = (n.x < 0.0f);
			}

			if (controller && controller->curState && controller->curState->clip)
			{
				std::string clipName = controller->curState->clip->name;
				std::wstring wideName(clipName.begin(), clipName.end());
				hud->screenTextRenderer->SetText(L"State: " + wideName);
			}
		}
	};

	class CatDemoObject : public GameObject
	{
	public:
		Transform* transform;
		SpriteRenderer* spriteRenderer;
		Animator* animator;

		CatDemoObject()
		{
			transform = AddComponent<Transform>();
			spriteRenderer = AddComponent<SpriteRenderer>();
			animator = AddComponent<Animator>();
			AddComponent<CatControlScript>();

			transform->SetScale(2.4f, 2.4f);   // 50px frames -> ~120px on screen
			spriteRenderer->layer = 10;

			name = "Cat";
		}

		void Awake() override
		{
			animator->SetController(new CatAnimatorController());
		}
	};
}

std::wstring AnimationFSMScene::GetTitleText()
{
	return L"Animator FSM Demo (Idle / Walk / Run)";
}

std::wstring AnimationFSMScene::GetInstructionsText()
{
	return L"이동: WASD    [Shift] 달리기    [1~9] 씬 전환";
}

void AnimationFSMScene::SpawnDemoObjects()
{
	CreateObject<CatDemoObject>();
}
