#pragma once
#include <Windows.h>
#include "D3DClass.h"
#include "CameraClass.h"

#include "InputState.h"

// 시스템 클래스에서 생성되는 또 다른 객체임
// 모든 app 그래픽 기능 여기 클래스에 캡슐화!
// 향후 모든 그래픽 객체 포함할거임!

//////////////
// 전역 변수 //
// ///////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 100.f;
const float SCREEN_NEAR = 0.3f;

class GraphicsClass
{
	public:
		GraphicsClass() : m_Direct3D(0), m_Camera(0), m_MainWorld(0), m_Text(0){}
		GraphicsClass(const GraphicsClass&):m_Direct3D(0) {}
		~GraphicsClass() {}

		bool Initialize(int, int, HWND);
		void Shutdown();
		//bool Frame();

		bool Frame(int, int);

		InputState* GetInputState();
		void UpdateCamera();
		void UpdateScene(double);

		XMMATRIX GetworldMatrix() { return worldMatrix; }
		XMMATRIX GetviewMatrix() { return viewMatrix; }
		XMMATRIX GetprojectionMatrix() { return projectionMatrix; }

		void SwitchToMainWorld() { m_ActiveWorld = m_MainWorld; }
		void SwitchToStartWorld() { m_ActiveWorld = m_StartWorld; }
		class WorldClass* GetActiveWorld() const { return m_ActiveWorld; }
		class WorldClass* GetMainWorld() const { return m_MainWorld; }
		class WorldClass* GetStartWorld() const { return m_StartWorld; }

		
private:
		bool Render();
	

private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	class WorldClass* m_MainWorld;
	class WorldClass* m_StartWorld;

	class WorldClass* m_ActiveWorld;

	//class BitmapClass* m_Bitmap;
	class TextClass* m_Text;
	// ModelClass* m_Model;
	// TextureShaderClass* m_TextureShader;

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix; //멤버 변수로 승격..!
	int Polygonnum = 0;
	int ObjectNum = 0;

	int screen_Width;
	int screen_Height;
};

