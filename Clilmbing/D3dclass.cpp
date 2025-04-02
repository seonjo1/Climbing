#include "d3dclass.h"

D3DClass::D3DClass()
{
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
}


D3DClass::D3DClass(const D3DClass& other)
{
}


D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	size_t stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	float fieldOfView, screenAspect;


	// ���� ����ȭ ���� (V-sync : ������� ������ ��°� GPU�� ������ ������ ����ȭ)
	m_vsync_enabled = vsync;

	// DXGI ���丮 ����
	// DXGI: GPU�� ���÷��� ����� Ž���ϰ� ���� ü���� ���� �� ����
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	// GPU �����: GPU�� ����� �ٸ� �ϵ��� ��� ��ġ(����� ��)�� ������ ��ü ��ġ�� ��Ī
	// �ý��ۿ� ��ġ�� GPU ����͸� ������ �� 0��° ����
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// GPU ����Ϳ� ����� ��� ��ġ(�����)�� ������ �� 0��° ����
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// Ư�� ����Ϳ� ���� �����Ǵ� ���÷��� ���(�ػ�, �ֻ��� ��) ��� �������� (����� �������� ����)
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// ���÷��� ��� ��� ������ŭ �迭 Ȯ��
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// ����Ͱ� �����ϴ� ���÷��� ��� ��� �������� (�̹��� �迭�� ��� ��)
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		delete[] displayModeList;
		displayModeList = 0;
		return false;
	}

	// ���÷��� ��� ��ϰ� ���Ͽ� �ػ󵵰� �´� ��� ã�� ����
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				// �ػ󵵰� ������ �ֻ��� ���� - [Numerator / denominator = �ֻ��� (ex: 60 / 1 = 60Hz)]
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// GPU ������� ��ũ����(GPU �̸�, �޸� �뷮 ��) ����
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		delete[] displayModeList;
		displayModeList = 0;
		return false;
	}

	// GPU �޸� ũ�� ���� (MB)
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// GPU �̸� ���ڿ� ����
	// adapterDesc.Decription(wchar_t*) -> m_videoCardDescription(char*) ���� 
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		delete[] displayModeList;
		displayModeList = 0;
		return false;
	}

	// displayModeList ����
	delete[] displayModeList;
	displayModeList = 0;

	// COM �������̽� ����
	adapterOutput->Release();
	adapterOutput = 0;

	adapter->Release();
	adapter = 0;

	factory->Release();
	factory = 0;

	// ���� ü�� ��ü 0���� �ʱ�ȭ
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// ����� 1�� (����Ʈ���۱��� �� 2��)
	swapChainDesc.BufferCount = 1;

	// �� ���� ũ�� ����
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// �� ���� ���� ����
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the refresh rate of the back buffer.
	if (m_vsync_enabled)
	{
		// ���� ����ȭ�� �ֻ��� ���� (GPU FPS�� ������� FPS�� ����)
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		// GPU�� FPS ���� ����
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// ���� ü���� �� ������ �뵵�� ���� Ÿ������ ����
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// ��� â �ڵ� ����
	swapChainDesc.OutputWindow = hwnd;

	// ��Ƽ ���ø� ��Ȱ��ȭ
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	if (fullscreen)
	{
		// â ��� false
		swapChainDesc.Windowed = false;
	}
	else
	{
		// â ��� true
		swapChainDesc.Windowed = true;
	}

	// ���÷��� ��� ����
	// ��ĵ���� ���� ���� (UNSPECIFIED: ��ĵ���� ���� �ڵ� ����)
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	// ȭ�� �ػ󵵰� ���÷��� ��ġ�� �ػ󵵿� ���� ������ ȭ���� ũ�⸦ ��� Ȯ�� Ȥ�� ������� ���� (UNSPECIFIED: �����ϸ� ��� �ڵ� ����)
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// ���� ��� ���� (DXGI_SWAP_EFFECT_DISCARD : �� ���� ���� �� ������ ���)
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// ���� ü�ο� ���� ��� ���� (0: ��� ���� x)
	swapChainDesc.Flags = 0;

	// Direct3D ��� ���� ���� (D3D_FEATURE_LEVEL_11_0: Direct3D 11�� ���� �⺻���� ��� ����)
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// ���� ü��, ����̽�, ����̽� ���ؽ�Ʈ ����
	// ����̽�: GPU�� �����ϴ� ��ü (GPU���� ��ȣ�ۿ��� �����ϰ� ���ҽ� ����)
	// ����̽� ���ؽ�Ʈ: ����̽��� ���� ������ �۾��� �����ϴ� ��ü (GPU���� ���� �۾��� �����ϴ� ȯ��)
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	// ����� ������ (backBufferPtr�� ����� ������ ����)
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// ���� Ÿ�� �� ���� (����۸� ���� Ÿ�� ��� ����)
	// ���� Ÿ�� ��: GPU�� �������� ����� ������ �޸� ����
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// ����� ������ ���� ����
	backBufferPtr->Release();
	backBufferPtr = 0;

	// ���� ���� �ʱ�ȭ
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// ���� ���� ����
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	// 24bit ����, 8bit ���ٽ�
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// ��Ƽ ���ø� ��Ȱ��ȭ (Count = 1 && Quality = 0)
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// ���� ���ٽ� 2D�ؽ�ó ���� ����
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// ���� ���ٽ� ���� �ʱ�ȭ
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// ���� �׽�Ʈ Ȱ��ȭ
	depthStencilDesc.DepthEnable = true;
	// ���� �� ��� �ȼ��� ���� ���� ���� ����
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	// ���� �� �Լ� ���� (���� ���� ���� �� ���� ��쿡�� �ȼ��� �׷������� ��(���� �켱))
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// ���ٽ� �׽�Ʈ Ȱ��ȭ
	depthStencilDesc.StencilEnable = true;
	// ���ٽ� �б�/���� ����ũ 0xFF ���� (��� ��Ʈ�� ���� �б�/���Ⱑ ����)
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// ���ٽ� ���� ����(�ȼ��� �ո��� ���)
	// ���ٽ� ���� �׽�Ʈ�� ������ �� � ������ ���� ���� (KEEP: ���ٽ� �� ����)
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	// ���� �׽�Ʈ�� ������ �� � ������ ���� ���� (INCR: ���ٽ� �� 1 ����)
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	// ���ٽ� �׽�Ʈ�� ������ �� � ������ ���� ���� (KEEP: ���ٽ� �� ����)
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	// ���ٽ� ���� ���ϴ� ��� ���� (COMPARISON_ALWAYS: �׻� �� ���� ����)
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// ���ٽ� ���� ����(�ȼ��� �ո��� �޸��� ���)
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// ���� ���ٽ� ���� ����
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	// ������ ���������ο��� ���� ���ٽ� ���� ����
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// ���� ���ٽ� �� �ʱ�ȭ
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// ���� ���ٽ� �� ����
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// ���� ���ٽ� �� ����
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	// ���������ο� ���� Ÿ�� ��� ���� ���ٽ� �� ����
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// ������ȭ ����
	// ��Ƽ �ٸ���� ����
	rasterDesc.AntialiasedLineEnable = false;
	// Cull ���� (CULL_BACK: ���� �� ����)
	rasterDesc.CullMode = D3D11_CULL_BACK;
	// ���� ���� ���� (���� �߰��ϸ� ������ ���̸� ���������� Ȯ����� ��ħ ���� �ذ�)
	rasterDesc.DepthBias = 0;
	// ���� ������ ����� �� �� �ִ밪 ����
	rasterDesc.DepthBiasClamp = 0.0f;
	// ���� Ŭ���� Ȱ��ȭ ���� (���̰� ȭ���� ���� �ۿ� �����ϸ� �� ���ؽ����� Ŭ���εǾ� ������ x)
	rasterDesc.DepthClipEnable = true;
	// �������� ä��� ��� ���� (SOLID: ä���� �׸�, WIREFRAME: ���̾� ������ ���)
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	// �ո� ���� (false: CCW�� �޸�)
	rasterDesc.FrontCounterClockwise = false;
	// ��Ƽ ���ø� ����
	rasterDesc.MultisampleEnable = false;
	// ȭ�� �Ϻθ� �������ϴ� ���� ��� ����
	rasterDesc.ScissorEnable = false;
	// ��翡 ���� ���� ���� ���� (��翡 ����Ͽ� ���� ������ �������� ����)
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// �����Ͷ����� ���� ����
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	// �����Ͷ����� ���� ���� (���������������ο� ����)
	m_deviceContext->RSSetState(m_rasterState);

	// ����Ʈ ����
	m_viewport.Width = (float)screenWidth;
	m_viewport.Height = (float)screenHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	// ����Ʈ ���������ο� ����
	m_deviceContext->RSSetViewports(1, &m_viewport);

	// FOV : pi / 4 = 45��
	fieldOfView = 3.141592654f / 4.0f;
	// aspect: width / height
	screenAspect = (float)screenWidth / (float)screenHeight;

	// ���� ���� ��� ����
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// ���� ��� (���� ��� ����)
	m_worldMatrix = XMMatrixIdentity();

	// ���� ���� ��� ����
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}

void D3DClass::Shutdown()
{
	// ���� ü�� ��ü ȭ�� ����
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	// ������ ���� ����
	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	// ���� ���ٽ� �� ����
	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	// ���� ���ٽ� ���� ����
	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	// ���� ���ٽ� ���� ����
	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	// ���� Ÿ�� �� ����
	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	// ����̽� ���ؽ�Ʈ ����
	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	// ����̽� ����
	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	// ���� ü�� ����
	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// ����� �ش� ������ Ŭ����
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// ���� ���� 1.0(�ִ밪)���� Ŭ����
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3DClass::EndScene()
{
	if (m_vsync_enabled)
	{
		// ���� ����ȭ ON
		// Present(UINT SyncInterval, UINT Flags): ����Ʈ ���ۿ� �� ���� ��ȯ
		// SyncInterval = 1: ���� ����ȭ ON -> 1������ ��ٸ� �� ���� ��ȯ (���� ü�ο� ������ �Ϳ� ����)
		m_swapChain->Present(1, 0);
	}
	else
	{
		// ���� ����ȭ OFF
		// SyncInterval = 0: ���� ����ȭ OFF
		m_swapChain->Present(0, 0);
	}

	return;
}

ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}


ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}


void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}


void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}

void D3DClass::SetBackBufferRenderTarget()
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	return;
}


void D3DClass::ResetViewport()
{
	// Set the viewport.
	m_deviceContext->RSSetViewports(1, &m_viewport);

	return;
}