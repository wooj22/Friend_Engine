#include "ResourceLifetimeScene.h"
#include "DemoConfig.h"
#include "../Friend_2D_EngineLib/GameObject.h"
#include "../Friend_2D_EngineLib/Transform.h"
#include "../Friend_2D_EngineLib/SpriteRenderer.h"
#include "../Friend_2D_EngineLib/ResourceManager.h"
#include "../Friend_2D_EngineLib/RectTransform.h"
#include "../Friend_2D_EngineLib/UI_Text.h"
#include "../Friend_2D_EngineLib/Input.h"
#include "../Friend_2D_EngineLib/Time.h"
#include <sstream>
#include <cstdlib>

namespace
{
	struct NamedTexture { const char* path; const char* name; };

	const NamedTexture UniqueTextures[] = {
		{ "../Resource/Fish.png",     "Fish" },
		{ "../Resource/Mushroom.png", "Mushroom" },
		{ "../Resource/Cloud.png",    "Cloud" },
		{ "../Resource/DarkSky.png",  "DarkSky" },
		{ "../Resource/Sky.png",      "Sky" },
		{ "../Resource/Cat.png",      "CatFull" },
		{ "../Resource/Sun.png",      "Sun" },
		{ "../Resource/Earth.png",    "Earth" },
		{ "../Resource/Moon.png",     "Moon" },
		{ "../Resource/tree.jpg",     "Tree" },
		{ "../Resource/Space.jpg",    "Space" },
		{ "../Resource/atk_1.png",    "Atk1" },
		{ "../Resource/blood.png",    "Blood" },
	};
	constexpr int UniqueTextureCount = sizeof(UniqueTextures) / sizeof(UniqueTextures[0]);
}

std::wstring ResourceLifetimeScene::GetTitleText()
{
	return L"ResourceManager Cache Demo";
}

std::wstring ResourceLifetimeScene::GetInstructionsText()
{
	return L"[Q] 같은 텍스처 20개 생성    [W] 서로 다른 텍스처 20개 생성    [E] 모두 제거    [1~9] 씬 전환";
}

void ResourceLifetimeScene::SpawnDemoObjects()
{
	hud = CreateObject<UI_Text>();
	hud->rectTransform->SetPosition(0.0f, -(DemoConfig::WindowHeight / 2.0f - 45.0f));
	hud->rectTransform->SetSize((float)DemoConfig::WindowWidth, 90.0f);
	hud->screenTextRenderer->SetFontSize(18.0f);
	hud->screenTextRenderer->SetColor(D2D1::ColorF(D2D1::ColorF::White));

	RefreshHud();
}

void ResourceLifetimeScene::SpawnShared(int count)
{
	auto texture = ResourceManager::Get().CreateTexture2D("../Resource/Circle.png");

	for (int i = 0; i < count; ++i)
	{
		GameObject* obj = CreateObject<GameObject>();
		Transform* t = obj->AddComponent<Transform>();
		t->SetPosition((float)((rand() % 700) - 350), (float)((rand() % 260) - 100));
		t->SetScale(0.12f, 0.12f);

		SpriteRenderer* r = obj->AddComponent<SpriteRenderer>();
		r->sprite = ResourceManager::Get().CreateSprite(texture, "Circle");
		r->layer = 5;

		spawnedObjects.push_back(obj);
	}
}

void ResourceLifetimeScene::SpawnUnique(int count)
{
	for (int i = 0; i < count; ++i)
	{
		const NamedTexture& nt = UniqueTextures[uniqueTextureCursor % UniqueTextureCount];
		uniqueTextureCursor++;

		auto texture = ResourceManager::Get().CreateTexture2D(nt.path);

		GameObject* obj = CreateObject<GameObject>();
		Transform* t = obj->AddComponent<Transform>();
		t->SetPosition((float)((rand() % 700) - 350), (float)((rand() % 260) - 100));
		t->SetScale(0.12f, 0.12f);

		SpriteRenderer* r = obj->AddComponent<SpriteRenderer>();
		r->sprite = ResourceManager::Get().CreateSprite(texture, nt.name);
		r->layer = 5;

		spawnedObjects.push_back(obj);
	}
}

void ResourceLifetimeScene::ClearAll()
{
	for (GameObject* obj : spawnedObjects)
		obj->Destroy();
	spawnedObjects.clear();

	ResourceManager::Get().Trim();
}

void ResourceLifetimeScene::RefreshHud()
{
	std::wstring mem = ResourceManager::Get().GetMemoryUsageWString();

	std::wostringstream oss;
	oss << L"활성 오브젝트: " << spawnedObjects.size() << L"개\n" << mem;
	hud->screenTextRenderer->SetText(oss.str());
}

void ResourceLifetimeScene::Update()
{
	__super::Update();

	if (Input::GetKeyDown('Q')) SpawnShared(20);
	if (Input::GetKeyDown('W')) SpawnUnique(20);
	if (Input::GetKeyDown('E')) ClearAll();

	hudTimer += Time::GetDeltaTime();
	if (hudTimer >= 0.3f)
	{
		hudTimer = 0.0f;
		RefreshHud();
	}
}
