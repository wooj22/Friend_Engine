#pragma once
#include <string>
#include "../Friend_2D_EngineLib/Scene.h"
#include "../Friend_2D_EngineLib/Camera.h"
#include "../Friend_2D_EngineLib/Input.h"

/* [DemoSceneBase]
* Common base for every portfolio demo scene.
* Handles camera creation, the title/instructions header, scene switching via
* number keys, and [ESC] to quit; each derived scene only needs to provide its
* title text (and, optionally, its own control legend) and place its content.
*/
class DemoSceneBase : public Scene
{
private:
	GameObject* cam = nullptr;

	void CreateHeader();

protected:
	virtual std::wstring GetTitleText() = 0;
	virtual std::wstring GetInstructionsText();
	virtual void SpawnDemoObjects() = 0;

public:
	void Awake() override;
	void Update() override;
};
