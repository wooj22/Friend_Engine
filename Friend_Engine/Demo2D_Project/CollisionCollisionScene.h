#pragma once
#include "DemoSceneBase.h"

/* Scene 3 : Collision - Collision
* A Circle(Collision) and a Box(Collision), each moved with the keyboard.
* When they collide (OnCollisionEnter) they turn red; movement is not blocked
* by the engine, but the event/normal/depth readout shows the collision firing.
*/
class CollisionCollisionScene : public DemoSceneBase
{
protected:
	std::wstring GetTitleText() override;
	void SpawnDemoObjects() override;
};
