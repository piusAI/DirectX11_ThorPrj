#pragma once
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#include "InputState.h"

#include <dinput.h>
#include <unordered_map>
#include <iostream>
using namespace std;

#include <DirectXMath.h>
using namespace DirectX;

class InputClass
{
public:
	InputClass() {}
	InputClass(const InputClass&) {}
	~InputClass() {}



	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	void Initialize();
	void Shutdown();


	bool IsKeyDown(unsigned int);

	void DetectInput(double, HWND, InputState*, int, int);
	bool InitDirectInput(HINSTANCE, HWND);

	void SetworldMatrix(XMMATRIX wM) { worldMatrix = wM; }
	void SetviewMatrix(XMMATRIX vM) { viewMatrix = vM; }
	void SetprojectionMatrix(XMMATRIX pM) { projectionMatrix = pM; }
	

	IDirectInputDevice8* GetKeyborad() { return DIKeyboard; }
	IDirectInputDevice8* GetMouse() { return DIMouse; }

	bool MouseShow = false;

	unordered_map<string, float> screenPosition;
	unordered_map<string, float> WorldPosition;
	unordered_map<string, float> DeprojectScreenToWorld(float, float, float, float);

	bool IsLeftClickedTopLeft() { return m_leftClickedTopLeft; }
	void ResetLeftClickedTopLeft() { m_leftClickedTopLeft = false; }

private:
	bool m_keys[256];

	IDirectInputDevice8* DIKeyboard;
	IDirectInputDevice8* DIMouse;
	
	LPDIRECTINPUT8 DirectInput;
	DIMOUSESTATE mouseLastState;

	float mouseX = 0;
	float mouseY = 0;

	bool prevLMB = false;
	bool prevMMB = false;

	bool m_leftClickedTopLeft = false;


	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	
};

