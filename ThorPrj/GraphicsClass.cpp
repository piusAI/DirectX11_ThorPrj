#include "GraphicsClass.h"
#include <cstdio>

#include "WorldClass.h"
#include "TextClass.h"
#include "BitmapClass.h"


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	screen_Width = screenWidth;
	screen_Height = screenHeight;



	//화면 facing되도록
	XMMATRIX baseViewMatrix = XMMatrixIdentity();

	m_Direct3D = new D3DClass;
	char msg[128];
	sprintf_s(msg, "Width: %d Height: %d\n", screenWidth, screenHeight);
	OutputDebugStringA(msg);

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L" D3D 초기화 불가!", L"에러", MB_OK);
		return false;
	}

	m_Camera = new CameraClass(0.05f, 16.0f, -18.f); //초기 생성시 바로 넣음
	m_Camera->SetRotation(80.f, 0.f, -1.0f); // 잠시 원래대로
	m_Camera->Render();


	//start World
	m_StartWorld = new WorldClass;
	if (!m_StartWorld) return false;

	m_StartWorld->SetWorld(EWorld::E_StartWorld);
	result = m_StartWorld->Initialize(hwnd, m_Direct3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"월드 초기화 불가", L"World Error!", MB_OK);
		return false;
	}

	//main World
	m_MainWorld = new WorldClass;
	if (!m_MainWorld) return false;

	m_MainWorld->SetWorld(EWorld::E_MainLevel);
	result = m_MainWorld->Initialize(hwnd, m_Direct3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"월드 초기화 불가", L"World Error!", MB_OK);
		return false;
	}



	//m_Camera->GetViewMatrix(baseViewMatrix);

	// 2D 텍스트 렌더링에 사용되는 baseViewMatrix는 카메라가 이동하거나 회전하지 않은 기본 상태(0, 0, -1에서 원점을 바라봄)여야 합니다.
	// m_Camera의 회전 및 이동 값이 적용된 뷰 행렬을 사용하면 2D 텍스트가 화면 밖으로 밀려나 보이지 않게 됩니다.
	XMVECTOR eyePosition = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	XMVECTOR focusPoint = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	baseViewMatrix = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);




	m_Text = new TextClass;
	if (!m_Text) return false;
	result = m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if(!result)
	{
		MessageBox(hwnd, L"Text 초기화 실패!", L"Text Error!", MB_OK);
		return false;
	}

	m_ActiveWorld = m_StartWorld;

	//m_Bitmap = new BitmapClass;
	//if (!m_Bitmap) return;

	return true;

	
}

void GraphicsClass::Shutdown()
{


	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}
	// D3D도 객체 해제
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
	
	if (m_MainWorld)
	{
		m_MainWorld->Shutdown();
		delete m_MainWorld;
		m_MainWorld = 0;
	}

	if (m_StartWorld)
	{
		m_StartWorld->Shutdown();
		delete m_StartWorld;
		m_StartWorld = 0;
	}

	//if (m_Bitmap)
	//{
	//	m_Bitmap->Shutdown();
	//	delete m_Bitmap;
	//	m_Bitmap = 0;
	//}

	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

}
//bool GraphicsClass::Frame()
//{
//
//	bool result;
//
//	//static float rot = 0.0f;
//
//	//rot += XM_PI * 0.005f;
//	//if (rot > 360.f)
//	//{
//	//	rot -= 360.0f;
//	//}
//	//result = Render(rot);
//	//if (!result) return false;
//	result = Render();
//	if (!result) return false;
//	return true;
//}
bool GraphicsClass::Frame(int fps, int cpu)
{
	bool result;
	
	
	// static float rotation = 0.0f;

	// update rotation each frame
	/*
	rotation += (float)XM_PI * 0.0005f;
	if (rotation > 360.f) rotation -= 360.f;


	//object spin이어서 뺌
	*/
	result = m_ActiveWorld->Frame();
	if (!result) return false;


	result = m_Text->SetFPS(fps, m_Direct3D->GetDeviceContext());
	if (!result)
	{
		OutputDebugStringA("SetFPS 실패!\n");
		return false;
	}

	result = m_Text->SetCPU(cpu, m_Direct3D->GetDeviceContext());
	if (!result)
	{
		OutputDebugStringA("CPUset 실패!\n");
		return false;
	}
	
	Polygonnum = m_ActiveWorld->GetPolygonNum();

	result = m_Text->SetPolygonNum(Polygonnum, m_Direct3D->GetDeviceContext());
	if (!result)
	{
		OutputDebugStringA("Polygon Num 실패!\n");
		return false;
	}

	ObjectNum = m_ActiveWorld->GetObjectNum();
	result = m_Text->SetObjectsNum(ObjectNum, m_Direct3D->GetDeviceContext());
	if (!result)
	{
		OutputDebugStringA("Object Num 실패!\n");
		return false;
	}


	result = m_Text->SetResolutionNum(screen_Width, screen_Height, m_Direct3D->GetDeviceContext());
	if (!result)
	{
		OutputDebugStringA("Object Num 실패!\n");
		return false;
	}

	result = Render();
	if (!result) return false;
	return true;
}

InputState* GraphicsClass::GetInputState()
{
	return m_Camera->GetInputState();
}

void GraphicsClass::UpdateCamera()
{
	m_Camera->UpdateCamera();
}

void GraphicsClass::UpdateScene(double time)
{
	m_Direct3D->UpdateScene(time);
}


//물고 물어서 Render까지 내려옴
bool GraphicsClass::Render()
{
	XMMATRIX orthoMatrix, camMatrix;
	bool result;

	
	m_Direct3D->BeginScene(0.14f, 0.2f, 0.07f, 1.0f);

	m_Camera->Render();
	// 사실상 get이아닌 set
	m_Camera->GetViewMatrix(viewMatrix);

	m_Direct3D->GetWorldMatrix(worldMatrix);

	m_Direct3D->GetOrthoMatrix(orthoMatrix); //ortho set 
	
	m_Camera->GetCamView(camMatrix); //02C camview matrix 추가
	
	// input 카메러 들어가야함!
	viewMatrix *= camMatrix; //02C 추가 행렬곱으로 돌리기? 

	
	//01 BG image작업 ->나는 필요 x
	//result = m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 200, 200);
	//if (!result) return false;


	if (m_ActiveWorld == m_MainWorld)
	{
		//다시 object 올릴 차례
		m_Direct3D->TurnOffAlphaBlending();
		m_Direct3D->TurnZBufferOn();

		// 여기 getdevice context해서 실제로 인자로 이제 들어옴
		// world 오브젝트 올리기전에..?
		m_Direct3D->GetProjectionMatrix(projectionMatrix);
		result = m_MainWorld->Render(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_Camera->RealGetCameraView());
		if (!result) return false;

	}

	if (m_ActiveWorld == m_StartWorld)
	{
		m_Direct3D->TurnZBufferOff();
		//m_Direct3D->GetProjectionMatrix(projectionMatrix);

		// startworld는 그대로 보도록
		XMMATRIX identityMatrix = XMMatrixIdentity();
		
		// 스타트 월드 렌더링 (여기에 등록된 대형 배경 UI나 삼각형이 그려짐)
		result = m_StartWorld->Render(m_Direct3D->GetDeviceContext(), identityMatrix, identityMatrix, identityMatrix, m_Camera->RealGetCameraView() /*여기선 어차피 안쓰는 빌보드긴함*/);
		if (!result) return false;

	}



	// UI text처럼 Buffer off, Alphablend ON다음에
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->TurnOnAlphaBlending();

	result = m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result) return false;

	m_Direct3D->EndScene();

	return true;
}