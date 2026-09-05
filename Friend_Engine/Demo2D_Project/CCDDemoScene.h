#pragma once
#include "DemoSceneBase.h"

/* Scene 6 : Continuous Collision Detection (CCD)
* A fast bullet is fired at a thin wall. In Discrete mode it tunnels straight
* through (the per-fixed-step position jump skips over the thin wall entirely);
* in Continuous mode, Rigidbody::CollisionContinuousDetection sweeps the path in
* sub-steps and stops the bullet at the wall instead. [Space] toggles the mode.
*/
class CCDDemoScene : public DemoSceneBase
{
protected:
	std::wstring GetTitleText() override;
	std::wstring GetInstructionsText() override;
	void SpawnDemoObjects() override;
};
