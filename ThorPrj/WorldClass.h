#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <windows.h>   // HWND 여기서 옴
using namespace DirectX;

#include "MoveQueue.h" //movequeue 한번 선언!

//#include <unordered_map>
#include <vector>
using namespace std;

class ModelClass;

enum EWorld
{
	E_NONE = 0,
	E_StartWorld = 1,
	E_MainLevel = 2
};


class WorldClass
{

public:
	WorldClass();
	~WorldClass();

	bool Initialize(HWND, ID3D11Device*);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX&);
	bool Frame();

	void SetWorld(EWorld world) { WorldType = world; }
	EWorld GetWorld() { return WorldType; }
	//int GetObjectNum();
	int GetPolygonNum();
	int GetObjectNum();
	
private:
	vector<ModelClass*> m_Model;
	class GPUShaderClass* m_GPUShader;
	
	/*XMFLOAT3 HammerTranslate;*/
	const int modelNum = 4;

	class TimerClass* m_Timer;
	EWorld WorldType;

	int polygonNum = 17;

	//int Objectnum=0;



	float m_OrbitTime = 0.0f;


};



