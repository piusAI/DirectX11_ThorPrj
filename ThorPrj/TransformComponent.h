#pragma once

#include <DirectXMath.h>

using namespace DirectX;
struct TransformComponent {


	TransformComponent();
	// XMFLOAT3 -> XMMATRIX�� ��ȯ�ؼ� �����ؾ���
	bool InitTransform();

	void MoveOffset(float, XMFLOAT3, float);
	void MoveToTarget(float,XMFLOAT3, float, float);

	
	void MoveRotationOffset(float deltaTime, XMFLOAT3 translate, float PosSpeed, float PosRotate);
	void SetBillboard(XMMATRIX&);
	XMFLOAT3 GetActorPosition();
	XMVECTOR RotateForward(float, XMFLOAT3, float);


	//void NoMove();

	XMFLOAT3 Translate = XMFLOAT3(0, 0, 0);
	XMFLOAT3 Rotation = XMFLOAT3(0, 0, 0);
	XMFLOAT3 Scale = XMFLOAT3(1, 1, 1);

	XMMATRIX RSTMatrix;
	XMFLOAT3 CurrentPosition;

};

