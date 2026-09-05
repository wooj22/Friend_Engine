#pragma once
#include "DemoSceneBase.h"

/* Scene 2 : Trigger - Collision
* A Circle(Trigger) and a Box(Collision), each moved with the keyboard.
* By this engine's rule, if either side is a Trigger the pair always resolves as a
* Trigger event (OnTriggerEnter), so they turn red but are never physically blocked,
* passing straight through each other.
*/
class TriggerCollisionScene : public DemoSceneBase
{
protected:
	std::wstring GetTitleText() override;
	void SpawnDemoObjects() override;
};
