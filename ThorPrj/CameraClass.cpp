#include "CameraClass.h"


CameraClass::CameraClass()
{
	_inputState = new InputState();

	camPosition = XMVectorSet(.0f, .0f, .0f, 0.0f); //Camposition도 잡아주고
	camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);
}
//카메라 초기 위치 생성자
CameraClass::CameraClass(float xpos, float ypos, float zpos) : m_positionX(xpos), m_positionY(ypos), m_positionZ(zpos),
m_rotationX(0), m_rotationY(0), m_rotationZ(0) {
	
	_inputState = new InputState();
	
	camPosition = XMVectorSet(xpos, ypos, zpos, 0.0f); //Camposition도 잡아주고
	camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);


}

CameraClass::CameraClass(float xpos, float ypos, float zpos, float xrot, float yrot, float zrot)
	: m_positionX(xpos), m_positionY(ypos), m_positionZ(zpos),
	m_rotationX(xrot), m_rotationY(yrot), m_rotationZ(zrot)
{
	_inputState = new InputState();

	camPosition = XMVectorSet(xpos, ypos, zpos, 0.0f); //Camposition도 잡아주고
	camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);
}

CameraClass::~CameraClass()
{
	delete _inputState;
}

InputState* CameraClass::GetInputState() {
	return _inputState;
}


void CameraClass::SetPosition(float x, float y, float z)
{

	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;

	camPosition = XMVectorSet(x, y, z, 0.0f); //Setposition한 값으로 Camposition 넣음
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
}

void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	upVector = XMLoadFloat3(&up);

	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	positionVector = XMLoadFloat3(&position);

	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	lookAtVector = XMLoadFloat3(&lookAt);

	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
	return;



}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}

void CameraClass::GetCamView(XMMATRIX& camview) 
{
	camview = camView; //02C camview matrix 추가
}


void CameraClass::UpdateCamera()
{

	camRotationMatrix = XMMatrixRotationRollPitchYaw(_inputState->camPitch, _inputState->camYaw, 0);
	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camTarget = XMVector3Normalize(camTarget);

	//Free Look Camera
	camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camUp = XMVector3Cross(camForward, camRight);

	//Move
	camPosition += _inputState->moveLeftRight * camRight;
	camPosition += _inputState->moveBackForward * camForward;
	camPosition += _inputState->moveUpDown * camUp;

	_inputState->moveLeftRight = 0.0f;
	_inputState->moveBackForward = 0.0f;
	_inputState->moveUpDown = 0.0f;

	camTarget = camPosition + camTarget;

	camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);

}

