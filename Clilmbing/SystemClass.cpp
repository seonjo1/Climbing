#include "systemclass.h"

SystemClass::SystemClass()
{
	m_Input = 0;
	m_Application = 0;
}

SystemClass::SystemClass(const SystemClass& other)
{
}


SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;


	// ȭ�� ũ�� �ʱ�ȭ
	screenWidth = 0;
	screenHeight = 0;

	// ������ â �ʱ�ȭ
	InitializeWindows(screenWidth, screenHeight);

	// �Է� ��ü �ʱ�ȭ
	m_Input = new InputClass;
	m_Input->Initialize();

	// ���ø����̼� ��ü �ʱ�ȭ
	m_Application = new ApplicationClass;
	result = m_Application->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

void SystemClass::Shutdown()
{
	// ���ø����̼� ��ü ����� ����
	if (m_Application)
	{
		m_Application->Shutdown();
		delete m_Application;
		m_Application = 0;
	}

	// ��ǲ ��ü ����� ����
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	// ������ â ����
	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;


	// �޽��� ����ü �ʱ�ȭ
	ZeroMemory(&msg, sizeof(MSG));

	// ���� ����
	done = false;
	while (!done)
	{
		// ������ �޽��� ó��
		// PeekMessage : ������ �޽��� ť���� �޽����� ������ 
		// PM_REMOVE   : ť���� �޽��� ������ ť���� ���� �޽��� ����
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// �޽����� Ű���� �Է��� ��� Ű���� �Է��� ���� �޽����� ��ȯ
			TranslateMessage(&msg);
			// �޽����� ������ ���ν����� ����
			DispatchMessage(&msg);
		}

		// ���� �޽��� ó�� (â ������, ���α׷� ����)
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// ������ ó��
			result = Frame();
			if (!result)
			{
				// ������ ó�� ���н� ����
				done = true;
			}
		}

	}

	return;
}

bool SystemClass::Frame()
{
	bool result;

	// ESC �Է½� ����
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// ���ø����̼� Ŭ������ ������ ó��
	result = m_Application->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	/*
	hwnd: ������ â�� �ڵ�
	umsg: �޽��� �ĺ���
	wparam: �޽����� ���� �߰� ���� (Ű �ڵ� or ���콺 ��ư ���� ��)
	lparam: �޽����� ���� �߰����� ������ (Ű�� ���� Ÿ�̹�, �ݺ� ����, ���콺�� x/y��ǥ ��)

	*/
	switch (umsg)
	{
		// key ���� ��� �̺�Ʈ
		case WM_KEYDOWN:
		{
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}

		// key �� ��� �̺�Ʈ
		case WM_KEYUP:
		{
			// If a key is released then send it to the input object so it can unset the state for that key.
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}

		// key down �� key up ������ ó�� (������ ũ�� ����, ���콺 Ŭ�� ��)
		default:
		{
			// DefWindowProc : ó������ ���� �޽����� �⺻������ ó�����ִ� �Լ� (ex: ������ ũ�� ����, �ý��� �޴�, ���콺 Ŭ�� �� �������� �⺻ ����)
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	int posX, posY;


	// ���ø����̼� �ڵ� ���� (SystemClass pointer)
	ApplicationHandle = this;

	// ���� ���ø����̼��� �ν��Ͻ� �ڵ� ����
	m_hinstance = GetModuleHandle(NULL);

	// ���ø����̼� �̸� ����
	m_applicationName = L"Engine";

	// ������ Ŭ���� ���� (WNDCLASSEX ����ü �ʱ�ȭ)
	/*
		������ ��Ÿ�� ����
		CS_HREDRAW: ���� ũ�� ����� ������ �ٽ� �׸���
		CS_VREDRAW: ���� ũ�� ����� ������ �ٽ� �׸���
		CS_OWNDC:   ���� Device Context ���
	*/
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	// ������ ���ν���(WndProc)�� �����Ͽ� �޽��� ó��
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	// ������ ������ ����
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	// ������ Ŀ�� ����
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	// ������ ���� ����
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	// ������ Ŭ���� �̸� ����
	wc.lpszClassName = m_applicationName;
	// ����ü ũ��
	wc.cbSize = sizeof(WNDCLASSEX);

	// ������ Ŭ���� ���
	RegisterClassEx(&wc);

	// GetSystemMetrics()�� ���� ����� �ý����� ȭ�� �ػ� ũ�� ��������
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	DEVMODE dmScreenSettings;

	if (FULL_SCREEN)
	{
		// ��ü ȭ�� ����
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// ��üȭ�� ����
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// ȭ�� ���� ���� ��ġ
		posX = posY = 0;
	}
	else
	{
		// â ��� ����
		screenWidth = 1920;
		screenHeight = 1080;

		// ȭ���� �߾ӿ� â ��ġ
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// ������ â ����
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// ShowWindow(): �����츦 ȭ�鿡 ǥ��
	ShowWindow(m_hwnd, SW_SHOW);

	// �����츦 ���� �տ� ��ġ
	SetForegroundWindow(m_hwnd);

	// �����쿡 ��Ŀ���� �����Ͽ� Ű���� �Է� ���� �ް� ��
	SetFocus(m_hwnd);

	// ���콺 Ŀ���� ȭ�鿡�� ����
	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	// ���콺 Ŀ�� �ٽ� ǥ��
	ShowCursor(true);

	// ���÷��� ���� ����
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// ������ ����
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// ������ Ŭ���� ��� ����
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// �����찡 ������ �����ϴ� ��� (���ø����̼� ���� �� ���ҽ� �����ϰų� ������ �۾���)
		case WM_DESTROY:
		{
			// WM_QUIT ���� �޽����� �޽��� ť�� �߰��ϴ� �Լ�
			PostQuitMessage(0);
			return 0;
		}

		// ������ x ��ư Ŭ�� or Alt + F4
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		default:
		{
			// �޽��� �ڵ鷯�� ����
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}