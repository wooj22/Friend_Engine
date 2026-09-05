#include "DemoSceneBase.h"
#include "DemoApp.h"
#include "DemoConfig.h"
#include "../Friend_2D_EngineLib/SceneManager.h"
#include "../Friend_2D_EngineLib/Transform.h"
#include "../Friend_2D_EngineLib/RectTransform.h"
#include "../Friend_2D_EngineLib/UI_Text.h"
#include "../Friend_2D_EngineLib/GameApp.h"

std::wstring DemoSceneBase::GetInstructionsText()
{
	return L"Box: WASD    Circle: 방향키    [Space] Reset    [1~9] 씬전환";
}

void DemoSceneBase::Awake()
{
	// camera
	cam = CreateObject<GameObject>();
	cam->AddComponent<Transform>();
	cam->AddComponent<Camera>((float)DemoConfig::WindowWidth, (float)DemoConfig::WindowHeight);

	CreateHeader();

	// demo content
	SpawnDemoObjects();
}

void DemoSceneBase::CreateHeader()
{
	float halfHeight = DemoConfig::WindowHeight / 2.0f;

	UI_Text* title = CreateObject<UI_Text>();
	title->rectTransform->SetPosition(0.0f, halfHeight - 45.0f);
	title->rectTransform->SetSize((float)DemoConfig::WindowWidth, 50.0f);
	title->screenTextRenderer->SetText(GetTitleText());
	title->screenTextRenderer->SetFontSize(32.0f);
	title->screenTextRenderer->SetColor(D2D1::ColorF(D2D1::ColorF::White));

	UI_Text* instructions = CreateObject<UI_Text>();
	instructions->rectTransform->SetPosition(0.0f, halfHeight - 85.0f);
	instructions->rectTransform->SetSize((float)DemoConfig::WindowWidth, 60.0f);
	instructions->screenTextRenderer->SetText(GetInstructionsText());
	instructions->screenTextRenderer->SetFontSize(18.0f);
	instructions->screenTextRenderer->SetColor(D2D1::ColorF(D2D1::ColorF::LightPink));
}

void DemoSceneBase::Update()
{
	__super::Update();

	// quit
	if (Input::GetKeyDown(VK_ESCAPE)) GameApp::Quit();

	// scene change
	if (Input::GetKeyDown('1')) SceneManager::Get().ChangeScene(DemoApp::TRIGGER_TRIGGER);
	if (Input::GetKeyDown('2')) SceneManager::Get().ChangeScene(DemoApp::TRIGGER_COLLISION);
	if (Input::GetKeyDown('3')) SceneManager::Get().ChangeScene(DemoApp::COLLISION_COLLISION);
	if (Input::GetKeyDown('4')) SceneManager::Get().ChangeScene(DemoApp::TRANSFORM_HIERARCHY);
	if (Input::GetKeyDown('5')) SceneManager::Get().ChangeScene(DemoApp::RAYCAST);
	if (Input::GetKeyDown('6')) SceneManager::Get().ChangeScene(DemoApp::CCD);
	if (Input::GetKeyDown('7')) SceneManager::Get().ChangeScene(DemoApp::RESOURCE_LIFETIME);
	if (Input::GetKeyDown('8')) SceneManager::Get().ChangeScene(DemoApp::ANIMATION_FSM);
	if (Input::GetKeyDown('9')) SceneManager::Get().ChangeScene(DemoApp::RIGIDBODY);
}
