#pragma once

#include <DirectXMath.h>
using namespace DirectX;

#include "AlignedAllocationPolicy.h"
#include <vector>

#include "InputState.h"


class CameraClass : public AlignedAllocationPolicy<16>
{
public:
	/*
	CameraClass(): m_positionX(0), m_positionY(0), m_positionZ(0),
					m_rotationX(0), m_rotationY(0), m_rotationZ(0){}	//기본 생성자 없애기
	*/
	CameraClass();
	CameraClass(float xpos, float ypos, float zpos); //카메라 초기위치 생성자
	CameraClass(float xpos, float ypos, float zpos, float xrot, float yrot, float zrot); //카메라 초기위치 생성자

	CameraClass(const CameraClass& other) {}
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	XMFLOAT3 GetPosition() { return XMFLOAT3(m_positionX, m_positionY, m_positionZ); }
	XMFLOAT3 GetRotation() { return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ); }


	void Render();
	void GetViewMatrix(XMMATRIX&);
	void UpdateCamera();

	void GetCamView(XMMATRIX&);
	InputState* GetInputState();

	XMMATRIX& RealGetCameraView() { return camView; };

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;

	InputState* _inputState = nullptr;

	XMMATRIX m_viewMatrix;
	XMVECTOR camTarget;
	XMVECTOR DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); //ForwardVector
	XMVECTOR DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f); //Right Vector
	
	XMVECTOR camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); //init시
	XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR camPosition;

	XMMATRIX camRotationMatrix, camView;

	//XMMATRIX MVP;
};


