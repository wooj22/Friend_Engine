#pragma once
#include "DemoSceneBase.h"

/* Scene 9 : Rigidbody (gravity / force / impulse / drag)
* A player-controlled box on the ground - [A]/[D] applies a horizontal force
* (drag gradually slows it back down after release) and [Space] applies an
* upward impulse (jump) while grounded.
*/
class RigidbodyDemoScene : public DemoSceneBase
{
protected:
	std::wstring GetTitleText() override;
	std::wstring GetInstructionsText() override;
	void SpawnDemoObjects() override;
};
