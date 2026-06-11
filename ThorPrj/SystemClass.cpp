#include "SystemClass.h"
#include <cstdio>

#include "TimerClass.h"
#include "GraphicsClass.h"
#include "CPUClass.h"
#include "FPSClass.h"



bool SystemClass::Initialize()
{
	//그래픽 렌더시 입력 객체 / app 객체 생성 및 초기화

	bool result;

	screenWidth = 0;
	screenHeight = 0;

	InitializeWindows(screenWidth, screenHeight);

	m_Input = new InputClass();
	m_Input->Initialize();
	result = m_Input->InitDirectInput(m_hinstance, m_hwnd); //이제 input init!
	if (!result)
	{
		MessageBox(0, L"Direct Input Initialization - 실패!", L"Error", MB_OK);
		return false;
	}

	m_Graphic = new GraphicsClass();
	result = m_Graphic->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result) return false;

	m_FPS = new FPSClass;
	if (!m_FPS) return false;
	m_FPS->Initialize();

	m_CPU = new CPUClass();
	if (!m_CPU) return false;
	m_CPU->Initialize();

	m_Timer = new TimerClass;
	result = m_Timer->Initialize();
	if (!result)
	{
		MessageBox(m_hwnd, L"Timer Object 초기화 실패 !", L" Timer Error", MB_OK);
		return false;
	}



	return true;
}



void SystemClass::Shutdown()
{
	if (m_Graphic)
	{
		m_Graphic->Shutdown();
		delete m_Graphic;
		m_Graphic = nullptr;
	}
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = nullptr;
	}

	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = nullptr;
	}

	if (m_CPU)
	{
		m_CPU->Shutdown();
		delete m_CPU;
		m_CPU = nullptr;
	}

	if (m_FPS)
	{
		delete m_FPS;
		m_FPS = nullptr;
	}

	ShutdownWindows();

	return;
}


// Run pseudo
// 완료 되지 않을동안
// 윈도우 메세지 확인하고
// Process system message
// Process Application Loop
// Frame처리중, 사용자 종료 원하는지 확인
void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	//메시지 구조 초기화!
	ZeroMemory(&msg, sizeof(MSG));
	
	//창, 사용자로부터 종료메세지 나타날때까지 반복!
	done = false;
	while (!done)
	{
		// window message 처리!!
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// 윈도우에서 앱 종료신호 보내면 종료!
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{

			result = Frame();
			if (!result) done = true;

			frameCount++;
			if (GetTime() > 1.0f)
			{
				fps = frameCount;
				frameCount = 0;
				StartTimer();
			}

			// WorldViewProjection Set
			SetWVPMatrix();

			frameTime = GetFrameTime();
			m_Input->DetectInput(frameTime, m_hwnd, m_Graphic->GetInputState(), screenWidth, screenHeight); //02 C



			if (m_Input->IsLeftClickedTopLeft())
			{
				m_Input->ResetLeftClickedTopLeft();
				if (m_Graphic->GetActiveWorld() == m_Graphic->GetStartWorld())
				{
					m_Graphic->SwitchToMainWorld();
				}
				else if (m_Graphic->GetActiveWorld() == m_Graphic->GetMainWorld())
				{
					m_Graphic->SwitchToStartWorld();
				}
			}




			// camera update
			m_Graphic->UpdateCamera();

			m_Graphic->UpdateScene(frameTime);
		

		}

	}
		return;
}


bool SystemClass::Frame()
{
	bool result;
	m_Timer->Frame();
	m_FPS->Frame();
	m_CPU->Frame();

	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	result = m_Graphic->Frame(m_FPS->GetFps(), m_CPU->GetCpuPercentage());
	if (!result) return false;

	
	return true;
}


// 메세지 핸들러 함수는 특정 정보 수신 가능
// 키 여부 읽어 입력 객체에 전달!
LRESULT SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	case WM_KEYDOWN:
		m_Input->KeyDown(static_cast<unsigned int>(wparam));
		return 0;
	case WM_KEYUP:
		m_Input->KeyUp(static_cast<unsigned int>(wparam));
		return 0;
		
	//나머지 메세지는 그냥 기본 message 처리기
	default:
		return DefWindowProc(hwnd, umsg, wparam, lparam);

	}
	return LRESULT();
}

double SystemClass::GetTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart - CounterStart) / countsPerSecond;
}

void SystemClass::StartTimer()
{
	LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency(&frequencyCount);

	countsPerSecond = double(frequencyCount.QuadPart);

	QueryPerformanceCounter(&frequencyCount);
	CounterStart = frequencyCount.QuadPart;
}

void SystemClass::SetWVPMatrix()
{
	m_Input->SetworldMatrix(m_Graphic->GetworldMatrix());
	m_Input->SetviewMatrix(m_Graphic->GetviewMatrix());
	m_Input->SetprojectionMatrix(m_Graphic->GetprojectionMatrix());
}

// Applicationclass.h에다가 FULLSCREEN 전역 변수를 넣음

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX)); //여기 0으로 꼭 밀기!!
	DEVMODE dmScreenSettings;
	int posX, posY;

	ApplicationHandle = this;

	m_hinstance = GetModuleHandle(NULL);

	m_applicationName = L"Engine";

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);

	wc.hIconSm = wc.hIcon;
	
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// window class cache 저장
	RegisterClassEx(&wc);

	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// 전체 화면 / 창모드에 따른 화면 설정
	if (FULL_SCREEN)

	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//전체 화면으로 수정
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		posX = posY = 0;
		
	}
	else
	{
		screenWidth = 1920;
		screenHeight = 1080;

		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//창 생성 창 핸들 가져오기!
	/*
		m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
			posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);
	*/
	
	// window
	m_hwnd = CreateWindowEx(NULL, m_applicationName, L"Thor Hammer Project",
		WS_OVERLAPPEDWINDOW,
		posX, posY,
		screenWidth, screenHeight,
		NULL,
		NULL,
		m_hinstance,
		NULL
	);


	if (m_hwnd == NULL) {
		DWORD error = GetLastError();
		char err_msg[128];
		sprintf_s(err_msg, "윈도우 생성 실패! 에러 코드: %lu\n", error);
		OutputDebugStringA(err_msg);
		return;
	}

	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Mouse Cursor 보이도록
	ShowCursor(false);

	return;

}

void SystemClass::ShutdownWindows()
{
	ShowCursor(true);

	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// 창 제거
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// 클래스에 대한 포인터 해제
	ApplicationHandle = NULL;
	return;
}


//WndProc함수는 윈도우 프로시져, 메시지 처리용 이벤트 처리 함수, Callback함수!
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{

	//if (umessage == WM_LBUTTONDOWN)
	//{
	//	OutputDebugStringW(L"LBUTTONDOWN 진입!\n");
	//	/*
	//	int ClickX = GET_X_LPARAM(lparam);
	//	int ClickY = GET_Y_LPARAM(lparam);

	//	wchar_t buf[256];

	//	swprintf_s(buf, L"ClickX : %d, ClickY:%d \n", ClickX, ClickY);
	//	OutputDebugStringW(buf);*/


	//	return 0;
	//}
	switch (umessage)
	{
		// 창 파괴시
	case WM_DESTROY:

		PostQuitMessage(0);
		return 0;

		// 창 닫을시
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

		// 왼쪽 마우스 클릭시

	

		// 나머지는 그냥 default로 처리
	default:
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);


	}
}


/*
double SystemClass::GetTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart - CounterStart) / countsPerSecond;
}

*/
double SystemClass::GetFrameTime()
{
	LARGE_INTEGER currentTime;
	__int64 tickCount;
	QueryPerformanceCounter(&currentTime);

	tickCount = currentTime.QuadPart - frameTimeOld;
	frameTimeOld = currentTime.QuadPart;

	if (tickCount < 0.0f)
		tickCount = (long)0.0;

	return float(tickCount) / countsPerSecond;
}


