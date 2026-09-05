#pragma once
#include "../Friend_2D_EngineLib/AnimationClip.h"
#include "../Friend_2D_EngineLib/AnimationBaseState.h"
#include "../Friend_2D_EngineLib/AnimatorController.h"
#include "../Friend_2D_EngineLib/ResourceManager.h"

/* [Cat Idle/Walk/Run FSM]
* Reuses the existing Resource/Texture/Cat_*.png + Resource/Data/SpriteSheet + AnimationClip
* JSON assets. Transitions are driven by the "Walk"/"Run" bool parameters, which a
* controlling Script sets every frame from player input.
*/

class CatIdleClip : public AnimationClip
{
public:
	CatIdleClip()
	{
		auto texture = ResourceManager::Get().CreateTexture2D("../Resource/Texture/Cat_Idle.png");
		LoadSpriteSheetFromJson(texture, "../Resource/Data/SpriteSheet/Cat_Idle_Sprites.json");
		LoadAnimationClipFromJson(texture, "../Resource/Data/AnimationClip/Cat_Idle_AniClip.json");
	}
};

class CatWalkClip : public AnimationClip
{
public:
	CatWalkClip()
	{
		auto texture = ResourceManager::Get().CreateTexture2D("../Resource/Texture/Cat_Walk.png");
		LoadSpriteSheetFromJson(texture, "../Resource/Data/SpriteSheet/Cat_Walk_Sprites.json");
		LoadAnimationClipFromJson(texture, "../Resource/Data/AnimationClip/Cat_Walk_AniClip.json");
	}
};

class CatRunClip : public AnimationClip
{
public:
	CatRunClip()
	{
		auto texture = ResourceManager::Get().CreateTexture2D("../Resource/Texture/Cat_Run.png");
		LoadSpriteSheetFromJson(texture, "../Resource/Data/SpriteSheet/Cat_Run_Sprites.json");
		LoadAnimationClipFromJson(texture, "../Resource/Data/AnimationClip/Cat_Run_AniClip.json");
	}
};

class CatIdleState : public AnimationBaseState
{
public:
	CatIdleState(AnimationClip* c, AnimatorController* ac) : AnimationBaseState(c, ac) {}
	void Enter() override {}
	void Exit() override {}
	void Update(float dt) override
	{
		if (controller->GetBool("Run"))       controller->PlayAnimation("Cat_Run");
		else if (controller->GetBool("Walk")) controller->PlayAnimation("Cat_Walk");
	}
};

class CatWalkState : public AnimationBaseState
{
public:
	CatWalkState(AnimationClip* c, AnimatorController* ac) : AnimationBaseState(c, ac) {}
	void Enter() override {}
	void Exit() override {}
	void Update(float dt) override
	{
		if (controller->GetBool("Run"))        controller->PlayAnimation("Cat_Run");
		else if (!controller->GetBool("Walk")) controller->PlayAnimation("Cat_Idle");
	}
};

class CatRunState : public AnimationBaseState
{
public:
	CatRunState(AnimationClip* c, AnimatorController* ac) : AnimationBaseState(c, ac) {}
	void Enter() override {}
	void Exit() override {}
	void Update(float dt) override
	{
		if (!controller->GetBool("Run"))
			controller->PlayAnimation(controller->GetBool("Walk") ? "Cat_Walk" : "Cat_Idle");
	}
};

class CatAnimatorController : public AnimatorController
{
public:
	CatIdleClip* idleClip = nullptr;
	CatWalkClip* walkClip = nullptr;
	CatRunClip* runClip = nullptr;
	CatIdleState* idleState = nullptr;
	CatWalkState* walkState = nullptr;
	CatRunState* runState = nullptr;

	CatAnimatorController()
	{
		idleClip = new CatIdleClip();
		walkClip = new CatWalkClip();
		runClip = new CatRunClip();

		idleState = new CatIdleState(idleClip, this);
		walkState = new CatWalkState(walkClip, this);
		runState = new CatRunState(runClip, this);

		AddState(idleState);
		AddState(walkState);
		AddState(runState);

		ChangeAnimation(idleState);
	}

	~CatAnimatorController() override
	{
		delete idleState; delete walkState; delete runState;
		delete idleClip; delete walkClip; delete runClip;
	}
};
