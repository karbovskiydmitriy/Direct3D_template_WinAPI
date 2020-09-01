#include "Template.hpp"

const DWORD windowStyle = WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP | WS_MAXIMIZE;
RECT clientRect;
HWND hMainWindow;
HDC hdcBack;
HBITMAP hbmBack;
int time;
float angle = 0.0f;

IDirect3D9 *d3d;
IDirect3DDevice9 *d3dDevice;
IDirect3DVertexBuffer9 *d3dVertexBuffer;

int trianglesCount = sizeof(cubeIndices) / sizeof(Triangle);
Vertex *vertices;

float fov = 45.0f;
float zNear = 0.01f;
float zFar = 1024.0f;

WNDCLASSEX wndClass =
{
	sizeof(WNDCLASSEX), 0, (WNDPROC)WindowProc, 0, 0, (HINSTANCE)0,
		(HICON)0, (HCURSOR)0, (HBRUSH)0, (TCHAR *)null, (TCHAR *)MAIN_CLASS_NAME, 0
};

D3DPRESENT_PARAMETERS d3dPresentParameters =
{
	0, 0, D3DFMT_UNKNOWN, 0, D3DMULTISAMPLE_NONE,
		0, D3DSWAPEFFECT_DISCARD, 0, true, false,
		D3DFMT_UNKNOWN, 0, 0, 0
};

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	RegisterClassEx(&wndClass);
	hMainWindow = CreateWindowEx(0, MAIN_CLASS_NAME, CAPTION, windowStyle, 0, 0, 0, 0, 0, 0, 0, 0);

	Init();

	while (true)
	{
		MSG msg;
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
		{
			break;
		}
		Draw();
	}

	Deinit();

	ExitProcess(0);
}

LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_SIZE:
		GetClientRect(hWnd, &clientRect);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		Draw();

		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		BitBlt(ps.hdc, 0, 0, clientRect.right, clientRect.bottom, hdcBack, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
			break;
		}
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

void Init()
{
	time = GetTickCount();

	HDC hdc = GetDC((HWND)0);
	hdcBack = CreateCompatibleDC(hdc);
	hbmBack = CreateCompatibleBitmap(hdc, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	SelectObject(hdcBack, hbmBack);
	ReleaseDC((HWND)0, hdc);

	ShowCursor(false);

	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hMainWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dPresentParameters, &d3dDevice);

	int veticesCount = trianglesCount * 3;

	d3dDevice->CreateVertexBuffer(veticesCount * sizeof(Vertex), 0, CUSTOMFVF_VERTEX, D3DPOOL_MANAGED, &d3dVertexBuffer, 0);
	vertices = new Vertex[veticesCount];

	for (int i = 0, index = 0; i < trianglesCount; i++)
	{
		Triangle *t = &cubeIndices[i];
		for (int j = 0; j < 3; j++)
		{
			int vertexIndex = t->vertices[j];
			vertices[index++] = cubeVertices[vertexIndex];
		}
	}

	Pointer p;
	d3dVertexBuffer->Lock(0, 0, &p, 0);
	memcpy(p, vertices, sizeof(Vertex) * veticesCount);
	d3dVertexBuffer->Unlock();
	
	d3dDevice->SetFVF(CUSTOMFVF_VERTEX);
	d3dDevice->SetRenderState(D3DRS_LIGHTING, false);
}

void Draw()
{	
	int currentTime = GetTickCount();
	
	if (currentTime >= time + 10)
	{
		time = currentTime;
		angle += 3.14f / 180;
	}

	d3dDevice->Clear(0, 0, D3DCLEAR_TARGET, COLOR, 1.0f, 0);

	d3dDevice->BeginScene();

	d3dDevice->SetStreamSource(0, d3dVertexBuffer, 0, sizeof(Vertex));

	D3DXMATRIX position;
	D3DXMATRIX rotation;
	D3DXMATRIX scale;
	D3DXMatrixTranslation(&position, 0.0f, 0.0f, 0.0f);
	D3DXMatrixRotationY(&rotation, angle);
	D3DXMatrixScaling(&scale, 1.0f, 1.0f, 1.0f);
	d3dDevice->SetTransform(D3DTS_WORLD, &(scale * rotation * position));

	D3DXVECTOR3 cameraPosition = D3DXVECTOR3(-200.0f, 200.0f, -200.0f);
	D3DXVECTOR3 cameraLookAt = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 cameraUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXMATRIX viewMatrix;
	D3DXMatrixLookAtLH(&viewMatrix, &cameraPosition, &cameraLookAt, &cameraUp);
	d3dDevice->SetTransform(D3DTS_VIEW, &viewMatrix);

	D3DXMATRIX projectionMatrix;
	float aspect = (float)clientRect.right / clientRect.bottom;
	D3DXMatrixPerspectiveFovLH(&projectionMatrix, 1.0f, aspect, zNear, zFar);
	d3dDevice->SetTransform(D3DTS_PROJECTION, &projectionMatrix);

	d3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, trianglesCount);

	d3dDevice->EndScene();

	d3dDevice->Present(0, 0, 0, 0);
}

void Deinit()
{
	d3dVertexBuffer->Release();
	d3dDevice->Release();
	d3d->Release();
}
