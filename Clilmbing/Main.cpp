#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <iostream>

// ��ũ�� ���̺귯��
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "user32.lib")

// ���� ������
IDXGISwapChain* swapChain = nullptr;
ID3D11Device* device = nullptr;
ID3D11DeviceContext* deviceContext = nullptr;
ID3D11RenderTargetView* renderTargetView = nullptr;

// ������ ���ν��� �Լ�
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

// DirectX 11 �ʱ�ȭ �Լ�
bool InitDirectX(HWND hWnd)
{
	// Swap Chain �� Device, Device Context �ʱ�ȭ
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;                                // ���� �ϳ� ���
	swapChainDesc.BufferDesc.Width = 800;                         // â �ʺ�
	swapChainDesc.BufferDesc.Height = 600;                        // â ����
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // ���� ����
	swapChainDesc.SampleDesc.Count = 1;                           // ���ø� 1ȸ
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;                             // �������� â
	swapChainDesc.Windowed = TRUE;                                 // ������ ���
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
		&swapChainDesc, &swapChain, &device, nullptr, &deviceContext);

	if (FAILED(hr))
	{
		MessageBox(hWnd, L"DirectX 11 Device and SwapChain Creation Failed", L"Error", MB_OK);
		return false;
	}

	// ���� Ÿ�� �� ����
	ID3D11Texture2D* backBuffer = nullptr;
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if (FAILED(hr))
	{
		MessageBox(hWnd, L"GetBuffer failed", L"Error", MB_OK);
		return false;
	}

	hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
	backBuffer->Release(); // �� �̻� �ʿ����� ������ ����
	if (FAILED(hr))
	{
		MessageBox(hWnd, L"CreateRenderTargetView failed", L"Error", MB_OK);
		return false;
	}

	deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);

	// ����Ʈ ����
	D3D11_VIEWPORT viewport = {};
	viewport.Width = (float)800;
	viewport.Height = (float)600;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	deviceContext->RSSetViewports(1, &viewport);

	return true;
}

// WinMain �Լ�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	// ������ Ŭ���� ����
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"DX11WindowClass";
	RegisterClass(&wc);

	// ������ â ����
	HWND hWnd = CreateWindowEx(
		0, wc.lpszClassName, L"DirectX 11 Example", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
		return 0;

	ShowWindow(hWnd, nCmdShow);

	// DirectX �ʱ�ȭ
	if (!InitDirectX(hWnd))
	{
		return 0; // �ʱ�ȭ ����
	}

	// �޽��� ����
	MSG msg = {};
	while (true)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				return (int)msg.wParam;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// ����� ���������� ä���
		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		deviceContext->ClearRenderTargetView(renderTargetView, clearColor);

		// ����ü�� ǥ��
		swapChain->Present(1, 0);
	}

	return 0;
}
