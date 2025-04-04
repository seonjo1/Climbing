#include "camera.h"

Camera::Camera()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}


Camera::Camera(const Camera& other)
{
}


Camera::~Camera()
{
}

void Camera::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}


void Camera::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}

XMFLOAT3 Camera::GetPosition()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}


XMFLOAT3 Camera::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

void Camera::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;


	// upVector ����
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// upVector load
	upVector = XMLoadFloat3(&up);

	// position ����
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// position load
	positionVector = XMLoadFloat3(&position);

	// camera front ����
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// camera front load
	lookAtVector = XMLoadFloat3(&lookAt);

	// degree�� m_rotation�� Radian���� ���� (0.0174532925 == pi / 180)
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// Roll, Pitch, Yaw�� ���� ȸ�� ��� ����
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// camera front, up ���͸� world ��ǥ��� ����
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// lookAtVector: ī�޶� ���� �ٶ󺸴� ���� ��ġ
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// View Matrix ����
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}

void Camera::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}
