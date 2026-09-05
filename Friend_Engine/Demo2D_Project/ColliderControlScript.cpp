#include "ColliderControlScript.h"
#include "../Friend_2D_EngineLib/GameObject.h"
#include "../Friend_2D_EngineLib/Transform.h"
#include "../Friend_2D_EngineLib/SpriteRenderer.h"
#include "../Friend_2D_EngineLib/Rigidbody.h"
#include "../Friend_2D_EngineLib/RectTransform.h"
#include "../Friend_2D_EngineLib/UI_Text.h"
#include "../Friend_2D_EngineLib/Time.h"
#include "../Friend_2D_EngineLib/Input.h"
#include <sstream>
#include <iomanip>

void ColliderControlScript::Awake()
{
	tr = gameObject->transform;
	rb = gameObject->GetComponent<Rigidbody>();
	renderer = gameObject->GetComponent<SpriteRenderer>();
	collider = gameObject->GetComponent<ICollider>();

	startPosition = tr->GetPosition();

	debugLabel = Instantiate<UI_Text>();
	debugLabel->screenTextRenderer->SetFontSize(16.0f);
	debugLabel->screenTextRenderer->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
	debugLabel->screenTextRenderer->layer = 20;
	debugLabel->rectTransform->SetSize(260.0f, 90.0f);
}

void ColliderControlScript::Update()
{
	// [Space] reset to spawn position
	if (Input::GetKeyDown(VK_SPACE))
	{
		tr->SetPosition(startPosition);
		if (rb) rb->velocity = Vector2::zero;
	}

	Vector2 moveInput = ReadMoveInput();
	bool isCollisionType = collider && !collider->isTrigger;

	if (isCollisionType && rb)
	{
		// Non-trigger colliders are driven through the Rigidbody's velocity, so the
		// engine's own collision resolution (Rigidbody::CorrectPosition, called from
		// BoxCollider/CircleCollider::OnCollisionEnter) can actually stop them at
		// contact with another Collision object instead of overlapping.
		rb->velocity = moveInput.IsZero() ? Vector2::zero : moveInput.Normalized() * speed;
	}
	else if (!moveInput.IsZero())
	{
		// Trigger colliders are moved directly - never blocked, free to overlap.
		tr->Translate(moveInput.Normalized() * speed * Time::GetDeltaTime());
	}

	// follow the object with the debug label (Transform::GetPosition() reads the
	// live local position field directly, so this is safe to do in Update()).
	if (debugLabel) debugLabel->rectTransform->SetPosition(tr->GetPosition());
}

void ColliderControlScript::LateUpdate()
{
	// DebugColliderDraw() reads transform->GetScreenMatrix(), which is only a
	// cached value - it's recomputed in TransformSystem::Update(), which runs
	// AFTER scriptSystem.Update() but BEFORE scriptSystem.LateUpdate() in
	// GameApp::Update(). Drawing here (instead of in Update()) means the debug
	// outline uses this frame's just-moved position instead of lagging one
	// frame behind the sprite.
	if (collider) collider->DebugColliderDraw();
}

Vector2 ColliderControlScript::ReadMoveInput() const
{
	float x = 0.0f, y = 0.0f;

	if (inputScheme == DemoInputScheme::WASD)
	{
		if (Input::GetKey('A')) x -= 1.0f;
		if (Input::GetKey('D')) x += 1.0f;
		if (Input::GetKey('W')) y += 1.0f;
		if (Input::GetKey('S')) y -= 1.0f;
	}
	else
	{
		if (Input::GetKey(VK_LEFT))  x -= 1.0f;
		if (Input::GetKey(VK_RIGHT)) x += 1.0f;
		if (Input::GetKey(VK_UP))    y += 1.0f;
		if (Input::GetKey(VK_DOWN))  y -= 1.0f;
	}

	return Vector2(x, y);
}

void ColliderControlScript::SetEventText(const wchar_t* typeLabel, const wchar_t* eventName, const ContactInfo& contact)
{
	std::wostringstream oss;
	oss << std::fixed << std::setprecision(3);
	oss << typeLabel << L" " << eventName << L"\n"
		<< L"Normal: (" << contact.normal.x << L", " << contact.normal.y << L")\n"
		<< L"Depth: " << contact.depth;

	lastEventText = oss.str();
	if (debugLabel) debugLabel->screenTextRenderer->SetText(lastEventText);
}

void ColliderControlScript::OnTriggerEnter(ICollider* other, const ContactInfo& contact)
{
	overlapping = true;
	if (renderer) renderer->SetColor(1.0f, 0.25f, 0.25f);
	SetEventText(L"Trigger", L"Enter", contact);
}

void ColliderControlScript::OnTriggerStay(ICollider* other, const ContactInfo& contact)
{
	SetEventText(L"Trigger", L"Stay", contact);
}

void ColliderControlScript::OnTriggerExit(ICollider* other, const ContactInfo& contact)
{
	overlapping = false;
	if (renderer) renderer->SetColor(1.0f, 1.0f, 1.0f);
	SetEventText(L"Trigger", L"Exit", contact);
}

void ColliderControlScript::OnCollisionEnter(ICollider* other, const ContactInfo& contact)
{
	overlapping = true;
	if (renderer) renderer->SetColor(1.0f, 0.25f, 0.25f);
	SetEventText(L"Collision", L"Enter", contact);
}

void ColliderControlScript::OnCollisionStay(ICollider* other, const ContactInfo& contact)
{
	SetEventText(L"Collision", L"Stay", contact);
}

void ColliderControlScript::OnCollisionExit(ICollider* other, const ContactInfo& contact)
{
	overlapping = false;
	if (renderer) renderer->SetColor(1.0f, 1.0f, 1.0f);
	SetEventText(L"Collision", L"Exit", contact);
}
