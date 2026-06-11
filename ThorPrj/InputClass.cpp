#include "InputClass.h"

#include <cstdio>

#include "MoveQueue.h"



void InputClass::Initialize()
{

	int i=0;

	//모든 키 눌리지 않은 상태로 초기화!
	// 윈도우의 Virtual Key code가 0~255범위임
	for (int i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}


	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();
	XMMATRIX projectionMatrix = XMMatrixIdentity();


	screenPosition.insert(make_pair("x", 0));
	screenPosition.insert(make_pair("y", 0));


	WorldPosition.insert(make_pair("x", 0));
	WorldPosition.insert(make_pair("y", 0));
	WorldPosition.insert(make_pair("z", 0));

	return;

}



void InputClass::Shutdown()
{
	if (DIMouse)
	{
		DIMouse->Unacquire();
		DIMouse->Release();
		DIMouse = nullptr;
	}

	if (DIKeyboard)
	{
		DIKeyboard->Unacquire();
		DIKeyboard->Release();
		DIKeyboard = nullptr;
	}

	if (DirectInput)
	{
		DirectInput->Release();
		DirectInput = nullptr;
	}
}

void InputClass::KeyUp(unsigned int input)
{
	//해당키 키배열에 저장
	m_keys[input] = true;
	return;
}

void InputClass::KeyDown(unsigned int input)
{
	// release처럼 떼면 해당 상태 지우기
	m_keys[input] = false;
	return;
}

bool InputClass::IsKeyDown(unsigned int key)
{
	// 눌렸는지 상태 return
	return m_keys[key];
}


void InputClass::DetectInput(double time, HWND hwnd, InputState* _inputState, int screenWidth, int screenHeight)
{

	// _inputState->moveLeftRight = 0.0f;
	// _inputState->moveBackForward = 0.0f; cam update에서 초기화 했었으

	DIMOUSESTATE mouseCurrState;
	BYTE keyboardState[256];

	DIKeyboard->Acquire();
	// 03마우스 잠시 해제
	DIMouse->Acquire(); // 획득 확인

	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);
	DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	if (keyboardState[DIK_ESCAPE] & 0x80)
		PostMessage(hwnd, WM_DESTROY, 0, 0);

	float speed = 15.0f * (float) time;

	
	if (keyboardState[DIK_A] & 0x80 || keyboardState[DIK_LEFTARROW] & 0x80 )
	{
		_inputState->moveLeftRight -= speed;
	}
	if (keyboardState[DIK_D] & 0x80 || keyboardState[DIK_RIGHTARROW] & 0x80)
	{
		_inputState->moveLeftRight += speed;
	}
	if (keyboardState[DIK_W] & 0x80 || keyboardState[DIK_UPARROW] & 0x80)
	{
		_inputState->moveBackForward += speed;
	}
	if (keyboardState[DIK_S] & 0x80 || keyboardState[DIK_DOWNARROW] & 0x80)
	{
		_inputState->moveBackForward -= speed;
	}

	if (keyboardState[DIK_E] & 0x80)
	{
		_inputState->moveUpDown += speed;
	}

	if (keyboardState[DIK_Q] & 0x80)
	{
		_inputState->moveUpDown -= speed;
	}
	// DetectInput에 임시로

	//char buf[64];
	//sprintf_s(buf, "speed: %f\n", speed);
	//OutputDebugStringA(buf);

	// 03 마우스 잠시 해제

	
	if ((mouseCurrState.lX != mouseLastState.lX) && (mouseCurrState.rgbButtons[1] & 0x80) || (mouseCurrState.lY != mouseLastState.lY) && (mouseCurrState.rgbButtons[1] & 0x80))
	{
		_inputState->camYaw += mouseLastState.lX * 0.001f;

		_inputState->camPitch += mouseCurrState.lY * 0.001f;

		mouseLastState = mouseCurrState;
	}
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	ScreenToClient(hwnd, &cursorPos);
	
	screenPosition["x"] = cursorPos.x;
	screenPosition["y"] = cursorPos.y;
	
	DeprojectScreenToWorld(screenWidth, screenHeight, cursorPos.x, cursorPos.y);

	bool currLMB = mouseCurrState.rgbButtons[0] & 0x80;

	if (!prevLMB && currLMB) //LMB
	{
		//SetCursor(LoadCursor(NULL, IDC_ARROW));
		ShowCursor(true);
		
		/*OutputDebugStringW(L"Left Button\n");
		wchar_t buf[256];
		swprintf_s(buf, L"ClickX : %d, ClickY : %d \n", cursorPos.x, cursorPos.y);
		OutputDebugStringW(buf);*/

		wchar_t buf2[256];
		swprintf_s(buf2,L"ScreenPosition:  X : %f, Y : %f \n", screenPosition["x"], screenPosition["y"]);
		OutputDebugStringW(buf2);

		wchar_t buf3[256];
		swprintf_s(buf3, L"WorldPosition:  X : %f, Y : %f, Z : %f \n", WorldPosition["x"], WorldPosition["y"], WorldPosition["z"]);
		OutputDebugStringW(buf3);
		
		//quueue에 넣어줌
		MoveQueue::MoveQ.push({ WorldPosition["x"], WorldPosition["y"], WorldPosition["z"] });
		

		//범위지정해서 
		if (cursorPos.x >= 180 && cursorPos.x <= 400 && cursorPos.y >= 123 && cursorPos.y <= 320)
		{
			m_leftClickedTopLeft = true;
		}

	}
	prevLMB = currLMB;

	bool currMMB = mouseCurrState.rgbButtons[2] & 0x80;
	if (!prevMMB && currMMB) //MMB
	{
		//SetCursor(LoadCursor(NULL, IDC_ARROW));
		ShowCursor(false);
		OutputDebugStringW(L"Middle Button\n");
	}
	prevMMB = currMMB;

	// Update Camera 호출 Systemclass에서해줌


	return;
}

bool InputClass::InitDirectInput(HINSTANCE hInstance, HWND hwnd)
{
	HRESULT hr;
	hr = DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&DirectInput,
		NULL);

	hr = DirectInput->CreateDevice(GUID_SysKeyboard,
		&DIKeyboard,
		NULL);

	// 03 마우스 잠시 해제
	hr = DirectInput->CreateDevice(GUID_SysMouse, &DIMouse, NULL);

	hr = DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = DIKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	// 03 마우스 잠시 해제
	 hr = DIMouse->SetDataFormat(&c_dfDIMouse);
	 hr = DIMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

	return true;
}


unordered_map<string, float> InputClass::DeprojectScreenToWorld(float screenWidth, float screenHeight, float screenX, float screenY)
{

	// 01 Screen -> NDC (clip 공간)
	float ndcX = (2.0f * screenX / screenWidth) - 1.0f;
	float ndcY = 1.0f - (2.0f * screenY / screenHeight); // Y 반전!

	// 02 NDC -> View (Projection 역행렬)
	XMMATRIX invProj = XMMatrixInverse(nullptr, projectionMatrix);
	XMVECTOR rayClip = XMVectorSet(ndcX, ndcY, 1.0f, 1.0f);
	XMVECTOR rayView = XMVector3TransformCoord(rayClip, invProj);
	//rayView = XMVectorSetZ(rayView, 1.0f); // 방향벡터 Z=1

	// 03 View -> World (View 역행렬)
	XMMATRIX invView = XMMatrixInverse(nullptr, viewMatrix);
	XMVECTOR rayWorld = XMVector3TransformNormal(rayView, invView);
	rayWorld = XMVector3Normalize(rayWorld);

	// 04 Camera World Position
	XMVECTOR rayOrigin = XMVector3TransformCoord(XMVectorZero(), invView);

	// 05 Ray-Plane Intersection (Y = 5.0f 평면과의 교차점 계산)
	// rayOrigin.y + t * rayWorld.y = 5.0f  =>  t = (5.0f - rayOrigin.y) / rayWorld.y
	float originY = XMVectorGetY(rayOrigin);
	float dirY = XMVectorGetY(rayWorld);
	float targetY = 5.0f;

	if (fabs(dirY) > 0.0001f)
	{
		float t = (targetY - originY) / dirY;
		XMVECTOR intersection = XMVectorAdd(rayOrigin, XMVectorScale(rayWorld, t));

		WorldPosition["x"] = XMVectorGetX(intersection);
		WorldPosition["y"] = targetY; // Y축을 5.0f로 고정
		WorldPosition["z"] = XMVectorGetZ(intersection);
	}
	else
	{
		// 평행한 경우 예외 처리
		WorldPosition["x"] = XMVectorGetX(rayOrigin);
		WorldPosition["y"] = targetY;
		WorldPosition["z"] = XMVectorGetZ(rayOrigin);
	}

	return WorldPosition;
}