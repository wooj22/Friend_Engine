#pragma once
#include "../Friend_2D_EngineLib/Script.h"
#include "../Friend_2D_EngineLib/Vector2.h"
#include "../Friend_2D_EngineLib/ICollider.h"
#include "DemoConfig.h"
#include <string>

class Transform;
class SpriteRenderer;
class Rigidbody;
class UI_Text;

enum class DemoInputScheme { WASD, Arrows };

/* [ColliderControlScript]
* Lets the player move a collider demo object directly with the keyboard
* (WASD for the Box, Arrow keys for the Circle) so Trigger/Collision events can be
* triggered on demand while screen-recording.
*
* - Trigger colliders are moved directly via Transform::Translate, so they never
*   physically block and can freely overlap.
* - Collision (non-trigger) colliders are moved by setting Rigidbody::velocity
*   each frame instead, so the engine's own physics (Rigidbody::FixedUpdate +
*   CorrectPosition) actually stops them at contact with another Collision object.
* - While overlapping another collider (Trigger or Collision), the sprite tints red.
* - A small on-screen label follows the object and shows the latest event
*   (Enter/Stay/Exit) together with the contact Normal and Depth, for the camera.
* - [Space] resets this object back to its spawn position.
*/
class ColliderControlScript : public Script
{
private:
	Transform* tr = nullptr;
	Rigidbody* rb = nullptr;
	SpriteRenderer* renderer = nullptr;
	ICollider* collider = nullptr;
	UI_Text* debugLabel = nullptr;

	Vector2 startPosition;

	bool overlapping = false;
	std::wstring lastEventText;

public:
	DemoInputScheme inputScheme = DemoInputScheme::WASD;
	float speed = DemoConfig::MoveSpeed;

	void Awake() override;
	void Update() override;
	void LateUpdate() override;

	// collision event
	void OnTriggerEnter(ICollider* other, const ContactInfo& contact) override;
	void OnTriggerStay(ICollider* other, const ContactInfo& contact) override;
	void OnTriggerExit(ICollider* other, const ContactInfo& contact) override;
	void OnCollisionEnter(ICollider* other, const ContactInfo& contact) override;
	void OnCollisionStay(ICollider* other, const ContactInfo& contact) override;
	void OnCollisionExit(ICollider* other, const ContactInfo& contact) override;

private:
	Vector2 ReadMoveInput() const;
	void SetEventText(const wchar_t* typeLabel, const wchar_t* eventName, const ContactInfo& contact);
};
