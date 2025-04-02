#pragma once

// ���̺귯�� ��ŷ
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

// Direct3D 11 API�� �� �� �ְ� ���ִ� ���
#include <d3d11.h>
// DirectX ���� ���̺귯��
#include <directxmath.h>

using namespace DirectX;

class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass& other);
	~D3DClass();

	bool Initialize(int , int, bool, HWND, bool, float, float);
	void Shutdown();

	// ������ ����
	void BeginScene(float, float, float, float);
	
	// ������ ���� �� ���÷���
	void EndScene();

	// Direct3D ����̽� ��ȯ
	ID3D11Device* GetDevice();

	// ����̽� ���ؽ�Ʈ ��ȯ
	ID3D11DeviceContext* GetDeviceContext();

	// ���� ��� ��ȯ
	void GetProjectionMatrix(XMMATRIX&);

	// ���� ��� ��ȯ
	void GetWorldMatrix(XMMATRIX&);
	
	// ���� ��� ��ȯ
	void GetOrthoMatrix(XMMATRIX&);

	// GPU ���� ��ȯ
	void GetVideoCardInfo(char*, int&);

	// ����� ������ Ÿ�� ����
	void SetBackBufferRenderTarget();

	// �� ��Ʈ �ʱ�ȭ
	void ResetViewport();

private:

	/*
		���� : ������ �����ϴ� �޸� ����
		���� : Ư�� �����̳� ���� ���
		��: ���۸� ����ϴ� ���
	*/

	// ���� ����ȭ Ȱ��ȭ ����
	bool m_vsync_enabled;

	// GPU �޸� ũ��(MB)
	int m_videoCardMemory;

	// GPU �̸�
	char m_videoCardDescription[128];

	// ���� ü�� ��ü
	IDXGISwapChain* m_swapChain;

	// Direct3D ����̽� ��ü
	ID3D11Device* m_device;

	// ����̽� ���ؽ�Ʈ ��ü
	ID3D11DeviceContext* m_deviceContext;

	// ���� Ÿ�� �� (������ �� �� ����)
	ID3D11RenderTargetView* m_renderTargetView;

	// ���� ���ٽ� ����, ����, �� ��ü
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;

	// ������ȭ ���� ��ü
	ID3D11RasterizerState* m_rasterState;

	// ���� ���
	XMMATRIX m_projectionMatrix;

	// ���� ���
	XMMATRIX m_worldMatrix;

	// ���� ���
	XMMATRIX m_orthoMatrix;

	// ����Ʈ ��ü
	D3D11_VIEWPORT m_viewport;
};
