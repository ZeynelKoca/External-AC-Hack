#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

class Paint
{
private:
	IDirect3D9Ex* object = NULL;
	IDirect3DDevice9Ex* device = NULL;
	D3DPRESENT_PARAMETERS params;
	ID3DXFont* font = 0;
	HWND TargetHWND;
	int width, height;

	int init(HWND hWnd);
	void drawText(char* string, int x, int y, int a, int r, int g, int b);
	void drawRectangle(int x, int y, int width, int height, D3DCOLOR color);
	
public:
	int render();
	void drawLine(int x1, int y1, int x2, int y2, D3DCOLOR color);

	Paint();
	Paint(HWND overlayHWND, HWND targetHWND, int width, int height);
};

