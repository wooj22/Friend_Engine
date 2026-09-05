#pragma once
#include "DemoSceneBase.h"

/* Scene 8 : Animator FSM (Idle / Walk / Run)
* Reuses the existing Cat_Idle/Cat_Walk/Cat_Run sprite-sheet + AnimationClip JSON
* assets through a small CatAnimatorController (AnimatorController subclass).
* WASD moves the cat and drives the "Walk"/"Run" bool parameters that the
* Idle/Walk/Run AnimationBaseState subclasses read to decide the next clip.
*/
class AnimationFSMScene : public DemoSceneBase
{
protected:
	std::wstring GetTitleText() override;
	std::wstring GetInstructionsText() override;
	void SpawnDemoObjects() override;
};
