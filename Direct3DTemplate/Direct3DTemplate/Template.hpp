#ifndef __TEMPLATE_HPP__
#define __TEMPLATE_HPP__

#pragma once

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <math.h>
#include "Types.h"
#include "Cube.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#define CAPTION "WinAPI program template"
#define MAIN_CLASS_NAME "TemplateClass"

#define CUSTOMFVF_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)
#define COLOR D3DCOLOR_XRGB(127, 127, 255)

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void Init();
void Draw();
void Deinit();

#endif // __TEMPLATE_HPP__
