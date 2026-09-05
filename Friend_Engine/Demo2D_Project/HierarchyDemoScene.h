#pragma once
#include "DemoSceneBase.h"

/* Scene 4 : Transform Hierarchy (Local -> World)
* A Sun (root, WASD-controlled), an Earth orbiting the Sun (child), and a Moon
* orbiting the Earth (grandchild). Moving the Sun carries Earth and Moon with it
* while they keep orbiting in their own local space, and each object's on-screen
* label shows its Local position (relative to its parent) vs its World position.
*/
class HierarchyDemoScene : public DemoSceneBase
{
protected:
	std::wstring GetTitleText() override;
	std::wstring GetInstructionsText() override;
	void SpawnDemoObjects() override;
};
