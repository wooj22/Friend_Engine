#include "GameApp.h"
#include "Singleton.h"

// static member init
bool GameApp::isLoop = true;

/// 윈도우 프로시저
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	GameApp* pThis = nullptr;

	// 윈도우 생성시
	if (uMsg == WM_NCCREATE)
	{
		// lParam매개변수에서 this 추출
		CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
		pThis = reinterpret_cast<GameApp*>(cs->lpCreateParams);

		// 윈도우 핸들의 GWLP_USERDATA(사용자 데이터 공간)에 this 저장
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
	}
	else {
		pThis = reinterpret_cast<GameApp*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	if (pThis)
		pThis->MessageProc(hwnd, uMsg, wParam, lParam);		// 객체의 멤버 함수 호출

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

/// 메시지 처리
void GameApp::MessageProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	// 게임 콘텐츠에서 override
}

/// Init
void GameApp::Init()
{
	// module path save
	char szPath[MAX_PATH] = { 0, };
	GetModuleFileNameA(NULL, szPath, MAX_PATH);
	modulePath = szPath;
	OutputDebugStringA(string(string(modulePath) + string("\n")).c_str());

	// directory path save
	GetCurrentDirectoryA(MAX_PATH, szPath);
	workingPath = szPath;
	OutputDebugStringA(string(string(workingPath) + string("\n")).c_str());

	// get program handle
	hInstance = GetModuleHandle(NULL);

	// window class 정의
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WindowProc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = hInstance;
	wc.lpszClassName = winClassName.c_str();
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	RegisterClassEx(&wc);

	SIZE clientSize = { width, height };
	RECT clientRect = { 0, 0, clientSize.cx, clientSize.cy };
	//AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, FALSE);		// 윈도우 모드 기준 크기 맞춤

	// window 창 생성  -> WM_NCCREATE 메시지 발생
	hWnd = CreateWindowEx(
		0,
		winClassName.c_str(),
		titleName.c_str(),
		WS_OVERLAPPEDWINDOW,		// 테두리 없음/ WS_OVERLAPPEDWINDOW (테두리 있음)
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		clientRect.right - clientRect.left,
		clientRect.bottom - clientRect.top,
		NULL,
		NULL,
		hInstance,
		this  // this가 WindowProc를 통해 CREATESTRUCT 구조체 타입의 lParamdp 에 저장된다
	);

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	OutputDebugStringA("[Woo Engine] Window Create\n");


	CoInitialize(nullptr);			// com 객체 초기화	
	
	// init
	Input::Init(hWnd);
	Time::Init();
	audioSystem.Init();
	renderSystem.Init(hWnd, width, height);
	resourceManager.Init();	 // rendersystem init 후에 호출해야 함

	OutputDebugStringA("[Woo Engine] System Init\n");
}

/// Update
void GameApp::Update()
{
	Input::Update();
	Time::Update();
	InvokeSystem::Update();
	
	sceneManager.Update();
	scriptSystem.Update();
	animatorSystem.Update();
	transformSystem.Update();

	uiSystem.Update();
	renderSystem.Update();
	audioSystem.Update();

	scriptSystem.LateUpdate();
	cameraSystem.Update();
}

/// Fixed Update
void GameApp::FixedUpdate()
{
	// 여기 물리 update 로직
	scriptSystem.FixedUpdate();
	physicsSystem.FixedUpdate();
	colliderSystem.FixedUpdate();
}

/// Render
void GameApp::Render()
{
	renderSystem.Render();
}

/// GameLoop
void GameApp::Loop()
{
	MSG msg = {};
	while (isLoop)
	{
		// message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) {
				isLoop = false;
				OutputDebugStringA("[Woo Engine] WM_QUIT!\n");
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// physics update -> fixed udpate
		accumulator += Time::GetDeltaTime();
		while (accumulator >= fixedDeltaTime)
		{
			FixedUpdate();
			accumulator -= fixedDeltaTime;
		}

		// update
		Update();
		Render();
	}
}

/// Release
void GameApp::UnInit()
{
	sceneManager.UnInit();
	audioSystem.UnInit();
	renderSystem.UnInit();   
	resourceManager.UnInit();
	CoUninitialize();			 // com 객체 해제

	OutputDebugStringA("[Woo Engine] GameApp Release\n");
}
