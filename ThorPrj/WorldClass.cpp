#include "WorldClass.h"
#include "ModelClass.h"

#include "GPUShaderClass.h"
#include "TimerClass.h"
#include "MoveQueue.h"

//#include <random>


WorldClass::WorldClass() :m_GPUShader(0), m_Timer(0), WorldType(EWorld::E_NONE)
{
	
	// Model수, 초기화
	m_Model.resize(polygonNum ,nullptr);


	//ActorMove.insert(make_pair("Hammer", XMFLOAT3(5,0,0)));
}

WorldClass::~WorldClass()
{

}



bool WorldClass::Initialize(HWND hwnd, ID3D11Device* device)
{
	bool result;

	switch (WorldType) {
	case EWorld::E_MainLevel:
	{
		
		while(!MoveQueue::MoveQ.empty())
			MoveQueue::MoveQ.pop();

		MoveQueue::MoveQ.push({ 0,0,0 });
		//vector<bool*> m_result = { 4, 0 };
		bool m_result[18] = { };

		m_Timer = new TimerClass();
		m_Timer->Initialize();

		m_Model[0] = new ModelClass(0, 0, 0); //hammer
		m_Model[1] = new ModelClass; // OrnateHandle
		m_Model[2] = new ModelClass; // Platform
		m_Model[3] = new ModelClass; // Dome sphere
		m_Model[4] = new ModelClass; // Castle
		m_Model[5] = new ModelClass; // Ending
		m_Model[6] = new ModelClass; // Fense

		m_Model[7] = new ModelClass; // Flag_R
		m_Model[8] = new ModelClass; // Flag_B

		m_Model[9] = new ModelClass; // Column1
		m_Model[10] = new ModelClass; // Column2

		m_Model[11] = new ModelClass; // Reliquary

		m_Model[12] = new ModelClass; // portal


		//start test
		m_Model[13] = new ModelClass; // Column1 ->star 배치 다시해야함
		m_Model[14] = new ModelClass; // Column1 ->star 배치 다시해야함
		m_Model[15] = new ModelClass; // Column1 ->star 배치 다시해야함

		//hela
		m_Model[16] = new ModelClass; // Column1 ->star 배치 다시해야함




		for (int i = 0; i < m_Model.size(); ++i)
		{
			if (!m_Model[i]) return false;
		}


		//instance 먼저!
		m_Model[7]->SetInstanceNum(6);
		m_Model[8]->SetInstanceNum(6);
		m_Model[9]->SetInstanceNum(3);
		m_Model[10]->SetInstanceNum(3);
		m_Model[11]->SetInstanceNum(3);
		m_Model[12]->SetInstanceNum(5);


		m_Model[7]->ModelInstance();
		m_Model[8]->ModelInstance();
		m_Model[9]->ModelInstance();
		m_Model[10]->ModelInstance();
		m_Model[11]->ModelInstance();
		m_Model[12]->ModelInstance();


		m_Model[7]->SetInstancePosition(0, 21.f, 0, 6.9f);
		m_Model[7]->SetInstancePosition(1, -7.f, 0, 9.7f);
		m_Model[7]->SetInstancePosition(2, 7.f, 0, 9.7f);
		m_Model[7]->SetInstancePosition(3, -22.f, 0, 6.0f);

		m_Model[7]->SetInstancePosition(4, -15.f, 0, -8.0f);
		m_Model[7]->SetInstancePosition(5, 15.f, 0, -8.0f);


		m_Model[8]->SetInstancePosition(0, 21.f, 0, -6.9f);
		m_Model[8]->SetInstancePosition(1, -7.f, 0, -9.5f);
		m_Model[8]->SetInstancePosition(2, 7.f, 0, -9.5f);
		m_Model[8]->SetInstancePosition(3, -22.f, 0, -6.0f);

		m_Model[8]->SetInstancePosition(4, -15.f, 0, 8.0f);
		m_Model[8]->SetInstancePosition(5, 15.f, 0, 8.0f);

		m_Model[9]->SetInstancePosition(0, 11.f, 0, 9.5f);
		m_Model[9]->SetInstancePosition(1, -16.f, 0, -4.2f);
		m_Model[9]->SetInstancePosition(2, -12.f, 0, 8.4f);

		m_Model[10]->SetInstancePosition(0, 0.f, 0, -9.0f);
		m_Model[10]->SetInstancePosition(1, 0.f, 0, 9.f);
		m_Model[10]->SetInstancePosition(2, 12.f, 0, -8.4f);

		m_Model[11]->SetInstancePosition(0, -5.f, 0, -2.5f);
		m_Model[11]->SetInstancePosition(1, -7.f, 0, 6.f);
		m_Model[11]->SetInstancePosition(2, 4.f, 0, -4.4f);
		m_Model[11]->SetInstancePosition(3, 9.f, 0, 3.6f);


		m_Model[12]->SetInstancePosition(0, -16.3f, 0, 3.8f);
		m_Model[12]->SetInstancePosition(1, -7.1f, 0, -4.6f);
		m_Model[12]->SetInstancePosition(2, 0.7f, 0, 5.2f);
		m_Model[12]->SetInstancePosition(3, 6.4f, 0, -3.3f);
		m_Model[12]->SetInstancePosition(4, 12.1f, 0, 4.1f);



		m_result[0] = m_Model[0]->Initialize(device, L"./data/Hammer.obj", L"./data/T_Hammer.dds");
		m_result[1] = m_Model[1]->Initialize(device, L"./data/SM_OrnateHandle.obj", L"./data/T_OrnateHandle.dds");
		m_result[2] = m_Model[2]->Initialize(device, L"./data/SM_Platform.obj", L"./data/T_Platform.dds");
		m_result[3] = m_Model[3]->Initialize(device, L"./data/SM_DomeSphere.obj", L"./data/T_Night.dds");
		m_result[4] = m_Model[4]->Initialize(device, L"./data/SM_Castle01.obj", L"./data/T_Castle01.dds");
		m_result[5] = m_Model[5]->Initialize(device, L"./data/SM_Ending.obj", L"./data/T_Ending.dds");
		m_result[6] = m_Model[6]->Initialize(device, L"./data/SM_Fense.obj", L"./data/T_Fense.dds");



		m_result[7] = m_Model[7]->Initialize(device, L"./data/SM_Flag.obj", L"./data/T_Flag_R.dds");
		m_result[8] = m_Model[8]->Initialize(device, L"./data/SM_Flag.obj", L"./data/T_Flag_B.dds");

		m_result[9] = m_Model[9]->Initialize(device, L"./data/SM_Column01.obj", L"./data/T_Column01.dds");
		m_result[10] = m_Model[10]->Initialize(device, L"./data/SM_Column02.obj", L"./data/T_Column02.dds");

		m_result[11] = m_Model[11]->Initialize(device, L"./data/SM_Reliquary.obj", L"./data/T_Reliquary01.dds");
		m_result[12] = m_Model[12]->Initialize(device, L"./data/SM_Portal01.obj", L"./data/T_Portal01.dds");


		m_result[13] = m_Model[13]->Initialize(device, L"./data/SM_ScifiStar.obj", L"./data/T_ScifiStar01.dds");
		m_result[14] = m_Model[14]->Initialize(device, L"./data/SM_Star02.obj", L"./data/T_Star02.dds");
		m_result[15] = m_Model[15]->Initialize(device, L"./data/SM_CrystalStar.obj", L"./data/T_CrystalStar.dds");

		m_result[16] = m_Model[16]->Initialize(device, L"./data/SM_Hella.obj", L"./data/T_Hella.dds"); //hella


		// 위치 맞추고
		m_Model[1]->SetTranslate(5.f, 0.f, 0.f);
		m_Model[4]->SetTranslate(-25.f, 0.f, 0.f);
		m_Model[5]->SetTranslate(22.f, 0.f, 0.f);

		//굳이 안해도 될듯 set으로 들어가서
		/*m_Model[13]->SetTransform(18.f, 24.f, -10.f);
		m_Model[14]->SetTransform(24.f, 26.f, -20.f);
		m_Model[15]->SetTransform(-20.f, 20.f, 20.f);*/

		m_Model[16]->SetTranslate(0.f, 23.f, 14.f);






		for (int i = 0; i < m_Model.size(); ++i)
		{
			if (!m_result[i])
			{
				MessageBox(hwnd, L"모델 초기화 불가", L"MODEL ERROR!", MB_OK);
				return false;
			}
		}
	}
	break;

	case EWorld::E_StartWorld:
	{
		//switch할떄 timer 메모리 누수 날수도있으니 확인해보기
		

		m_Timer = new TimerClass();
		m_Timer->Initialize();
		
		m_Model[0] = new ModelClass(0, 0, 0);
		if (!m_Model[0]) return false;
		bool IsResult = m_Model[0]->Initialize(device, L"./data/Quad.obj", L"./data/StartMenu.dds");
		
		if (!IsResult)
		{
			MessageBox(hwnd, L"start world ui 이미지 초기화 불가", L"IMAGE ERROR", MB_OK);
			return false;
		}
		break;
	}

	default:
		return false;
	}

		m_GPUShader = new GPUShaderClass();
		if (!m_GPUShader) return false;
	
		result = m_GPUShader->Initialize(device, hwnd);
		if (!result)
		{
			MessageBox(hwnd, L"GPU Shader OBJ 초기화 불가!", L"GPU SHADER ERROR!", MB_OK);
			return false;
		}
		return true;
	}


void WorldClass::Shutdown()
{
	if (m_GPUShader)
	{
		m_GPUShader->Shutdown();
		delete m_GPUShader;
		m_GPUShader = 0;
	}
	for (int i = 0; i < m_Model.size(); i++)
	{
		if (m_Model[i])
		{
			m_Model[i]->Shutdown();
			delete m_Model[i];
			m_Model[i] = 0;
		}
	}
	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}
}
bool WorldClass::Frame()
{
	m_Timer->Frame();
	return true;
}

int WorldClass::GetPolygonNum() {
	return (int)m_Model.size();
}

int WorldClass::GetObjectNum()
{
	int num = 0;
	for (int i = 0; i < (int)m_Model.size(); i++)
	{
		if (m_Model[1])
			num += m_Model[i]->GetInstanceCount();
	}
	return num;
}
bool WorldClass::Render(ID3D11DeviceContext* devCon, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX& cameraView)
{
	bool result;
	for (int i = 0; i < m_Model.size(); i++)
	{
		if(m_Model[i]){
		m_Model[i]->Render(devCon);
		result = m_GPUShader->Render(devCon, m_Model[i]->GetIndexCount(), m_Model[i]->GetInstanceCount(),
			worldMatrix, m_Model[i]->GetTransformMatrix(), viewMatrix,
			projectionMatrix, m_Model[i]->GetTexture());

		//	Tick도 개별로
		if (!result) return false;
	}
}
	
	if (WorldType == EWorld::E_MainLevel)
	{
		//망치에만 넣어주기
		if (m_Model[0])
		{
			if (!MoveQueue::MoveQ.empty())
			{
				m_Model[0]->Tick(m_Timer->GetDeltaTime(),
					XMFLOAT3(MoveQueue::MoveQ.front().X, MoveQueue::MoveQ.front().Y, MoveQueue::MoveQ.front().Z)
					, 4.0f/* speed */, 2.0f/* rot*/
				); //망치 offset
			}
		}


		XMFLOAT3 center = XMFLOAT3(0.0f, 0.0f, 0.0f);

		float radius = 7.0f; // 궤도 반지름
		float speed = 0.4f; // 공전 속도

		m_OrbitTime += m_Timer->GetDeltaTime() * speed;


		//13, 14, 15에도 이동되도록!
		if (m_Model[13])
		{
			float angle = m_OrbitTime + XM_2PI * 0.0f / 3.0f;  // 0도
			XMFLOAT3 pos = XMFLOAT3(
				center.x + cosf(angle) * radius,
				center.y,
				center.z + sinf(angle) * radius
			);
			m_Model[13]->SetTranslate(pos.x, pos.y+35.f, pos.z);
		}

		if (m_Model[14])
		{
			float angle = m_OrbitTime + XM_2PI * 1.0f / 3.0f;  // 120도
			XMFLOAT3 pos = XMFLOAT3(
				center.x + cosf(angle) * radius,
				center.y,
				center.z + sinf(angle) * radius
			);
			m_Model[14]->SetTranslate(pos.x, pos.y+ 34.f, pos.z);
		}

		if (m_Model[15])
		{
			float angle = m_OrbitTime + XM_2PI * 2.0f / 3.0f;  // 240도
			XMFLOAT3 pos = XMFLOAT3(
				center.x + cosf(angle) * radius,
				center.y,
				center.z + sinf(angle) * radius
			);
			m_Model[15]->SetTranslate(pos.x, pos.y+ 32.f, pos.z);
		}


		if (m_Model[16])
		{
			m_Model[16]->GetTransformComponent()->SetBillboard(cameraView);
		}
	}
	return true;
	
}
