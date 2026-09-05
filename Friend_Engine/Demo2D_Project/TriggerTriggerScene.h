#pragma once
#include "DemoSceneBase.h"

/* Scene 1 : Trigger - Trigger
* A Circle(Trigger) and a Box(Trigger), each moved with the keyboard.
* When they overlap (OnTriggerEnter) they turn red but are never physically blocked,
* so they can freely pass through each other.
*/
class TriggerTriggerScene : public DemoSceneBase
{
protected:
	std::wstring GetTitleText() override;
	void SpawnDemoObjects() override;
};
