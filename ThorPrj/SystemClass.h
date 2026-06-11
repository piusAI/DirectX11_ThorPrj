#pragma once //요즘 쓰는거

#include <windows.h>
#include "InputClass.h"


class SystemClass
{
public:
	explicit SystemClass() : m_Input(0), m_Graphic(0), m_Timer(0), m_CPU(0), m_FPS(0){}
	SystemClass(const SystemClass&) {}
	~SystemClass() {}

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
	
	double GetTime();
	void StartTimer();

	void SetWVPMatrix();

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();
	
	//double GetTime();
	double GetFrameTime();
	


private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;
	InputClass* m_Input;
	class GraphicsClass* m_Graphic;

	class TimerClass* m_Timer;
	class CPUClass* m_CPU;
	class FPSClass* m_FPS;

	double frameTime;
	double countsPerSecond = 0.0;
	__int64 frameTimeOld = 0;
	__int64 CounterStart = 0;


	int frameCount = 0;
	int fps = 0;

	int screenWidth, screenHeight;


};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static SystemClass* ApplicationHandle = 0;

