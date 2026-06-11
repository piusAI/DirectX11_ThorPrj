#include "TransformComponent.h"
#include "MoveQueue.h"

TransformComponent::TransformComponent()
{

	RSTMatrix = XMMatrixIdentity();
}

bool TransformComponent::InitTransform()
{
	XMMATRIX M_Translate = XMMatrixTranslation(Translate.x, Translate.y, Translate.z);
	XMMATRIX M_Rotation = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
	XMMATRIX M_Scale = XMMatrixScaling(Scale.x, Scale.y, Scale.z);

	//R*S*T
	RSTMatrix = M_Rotation * M_Translate * M_Scale;

	return true;
}




void TransformComponent::MoveToTarget(float deltaTime, XMFLOAT3 TargetPosition, float PosSpeed = 1, float RotSpeed = 1)
{
	// Actor 포지션으로부터 targetPosition까지 이동
	XMFLOAT3 currentPos = GetActorPosition();

	XMVECTOR cur = XMLoadFloat3(&currentPos);
	XMVECTOR tar = XMLoadFloat3(&TargetPosition);

	XMVECTOR dir = XMVector3Normalize(tar - cur);

	float dist = XMVectorGetX(XMVector3Length(tar - cur));
	if (dist < 1.0f)
	{

		if(!MoveQueue::MoveQ.empty())
		MoveQueue::MoveQ.pop();
		return; 
	}

	XMFLOAT3 moveDir;
	XMStoreFloat3(&moveDir, dir);

	//RotateForward(deltaTime, moveDir, RotSpeed);
	//MoveOffset(deltaTime, moveDir, PosSpeed);

	MoveRotationOffset(deltaTime, moveDir, PosSpeed, RotSpeed);
}


//전체 다 billboard로
void TransformComponent::SetBillboard(XMMATRIX& cameraView)
{
	XMMATRIX invView = XMMatrixInverse(nullptr, cameraView);
	//cameraView.r[3] = XMVectorSet(0, 0, 0, 1);
	invView.r[3] = XMVectorSet(0, 0, 0, 1);  // 이동 성분 제거

	invView.r[0] = XMVectorNegate(invView.r[0]); // Right 반전
	invView.r[1] = XMVectorNegate(invView.r[1]); // Up 반전
	//invView.r[2] = XMVectorNegate(invView.r[2]); // Forward 반전

	XMMATRIX M_Scale = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
	XMMATRIX M_Translate = XMMatrixTranslation(Translate.x, Translate.y, Translate.z);

	RSTMatrix = M_Scale * invView * M_Translate;
}

XMFLOAT3 TransformComponent::GetActorPosition()
{
	XMFLOAT4X4 matFloat;
	XMStoreFloat4x4(&matFloat, RSTMatrix);

	//현재위치뽑아줌
	return XMFLOAT3(matFloat._41, matFloat._42, matFloat._43);
}

//void TransformComponent::MoveRotationOffset(float deltaTime, XMFLOAT3 TargetDir, float PosSpeed = 1, float RotSpeed = 1)
//{
//	XMVECTOR rotAxis = RotateForward(deltaTime, TargetDir, RotSpeed);
//
//	XMMATRIX T = XMMatrixTranslation(TargetDir.x * deltaTime * PosSpeed, TargetDir.y * deltaTime * PosSpeed, TargetDir.z * deltaTime * PosSpeed);
//	XMMATRIX R = XMMatrixRotationNormal(rotAxis, RotSpeed * deltaTime);
//	
//	RSTMatrix = R * RSTMatrix;
//	RSTMatrix *= T;
//
//}

void TransformComponent::MoveRotationOffset(float deltaTime, XMFLOAT3 TargetDir, float PosSpeed, float RotSpeed)
{
	XMFLOAT4X4 mat;
	XMStoreFloat4x4(&mat, RSTMatrix);
	XMVECTOR currentForward = XMVector3Normalize(XMVectorSet(mat._21, mat._22, mat._23, 0));
	XMVECTOR targetV = XMVector3Normalize(XMLoadFloat3(&TargetDir));

	float dot = XMVectorGetX(XMVector3Dot(currentForward, targetV));
	dot = max(-1.0f, min(1.0f, dot)); // clamp
	float angle = acosf(dot);

	if (angle > 0.001f)
	{
		XMVECTOR rotAxis = XMVector3Normalize(XMVector3Cross(currentForward, targetV));
		float step = min(angle, RotSpeed * deltaTime); // 넘지않도록
		RSTMatrix = XMMatrixRotationNormal(rotAxis, step) * RSTMatrix;
	}

	// 이동
	RSTMatrix *= XMMatrixTranslation(
		TargetDir.x * deltaTime * PosSpeed,
		TargetDir.y * deltaTime * PosSpeed,
		TargetDir.z * deltaTime * PosSpeed
	);
}

void TransformComponent::MoveOffset(float deltaTime, XMFLOAT3 translate, float PosSpeed = 1)
{
	//translate
	// translate랑 나누어서 해야함
	RSTMatrix *= XMMatrixTranslation(translate.x * deltaTime * PosSpeed, translate.y * deltaTime * PosSpeed, translate.z * deltaTime * PosSpeed);

}

XMVECTOR TransformComponent::RotateForward(float deltaTime, XMFLOAT3 TargetDir, float RotSpeed)
{

	XMFLOAT4X4 mat;
	XMStoreFloat4x4(&mat, RSTMatrix);
	XMVECTOR currentForward = XMVectorSet(mat._31, mat._32, mat._33, 0);

	XMVECTOR targetV = XMLoadFloat3(&TargetDir);

	XMVECTOR rotAxis = XMVector3Cross(currentForward, targetV);

	if (XMVectorGetX(XMVector3Length(rotAxis)) < 0.001f) return rotAxis;

	rotAxis = XMVector3Normalize(rotAxis);
	//RSTMatrix *= XMMatrixRotationNormal(rotAxis, RotSpeed * deltaTime);

	return rotAxis;
}
//void TransformComponent::NoMove()
//{
//	//ActorPosition
//	//안해도 될듯..?
//}