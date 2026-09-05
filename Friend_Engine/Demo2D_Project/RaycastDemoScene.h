#pragma once
#include "DemoSceneBase.h"

/* Scene 5 : Raycast
* A camera-shaped emitter casts a ray (WASD to move, Left/Right to aim) into a
* handful of static Box/Circle colliders. The hit collider tints red, a debug
* line is drawn out to the hit point (or full length on a miss), and the HUD
* reports the hit object name and distance.
*/
class RaycastDemoScene : public DemoSceneBase
{
protected:
	std::wstring GetTitleText() override;
	std::wstring GetInstructionsText() override;
	void SpawnDemoObjects() override;
};
