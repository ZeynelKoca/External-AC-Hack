#include "Paint.h"
#include "Hack.h"
#include "Proc.h"
#include <string>

Hack* hack;

int Paint::init(HWND hWnd) {
	hack = new Hack();
	hack->Init();
	

	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &object))) {
		exit(1);
	}

	ZeroMemory(&params, sizeof(params));

	params.BackBufferWidth = width;
	params.BackBufferHeight = height;
	params.Windowed = TRUE;
	params.hDeviceWindow = hWnd;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	params.BackBufferFormat = D3DFMT_A8R8G8B8;
	params.EnableAutoDepthStencil = TRUE;
	params.AutoDepthStencilFormat = D3DFMT_D16;

	object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &params, 0, &device);

	D3DXCreateFont(device, 50, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, OUT_DEVICE_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Comic Sans", &font);
}

int Paint::render() {
	if (device == nullptr)
		return 1;
	device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	device->BeginScene();

	if (TargetHWND == GetForegroundWindow())
	{

		for (unsigned int i = 1; i < 32; i++) {
			uintptr_t currEnt = hack->RPM<uintptr_t>(hack->RPM<uintptr_t>(hack->entityListAddr) + (i * 0x4));

			std::vector<unsigned int> teamOffsets{ i * 0x4, 0x32C };
			uintptr_t teamAddr = FindDMAAddy(hack->hProc, hack->entityListAddr, teamOffsets);
			int teamNr;
			ReadProcessMemory(hack->hProc, (BYTE*)teamAddr, &teamNr, sizeof(teamNr), nullptr);

			if (teamNr == 0)
				continue;


			std::vector<unsigned int> healthOffsets{ i * 0x4, 0xF8 };
			uintptr_t healthAddr = FindDMAAddy(hack->hProc, hack->entityListAddr, healthOffsets);
			int health;
			ReadProcessMemory(hack->hProc, (BYTE*)healthAddr, &health, sizeof(health), nullptr);

			if (health <= 0)
				continue;

			
			Vector3 currEntPos = hack->RPM<Vector3>(currEnt + 0x34);
			//if (!hack->CheckValidEnt(currEnt)) 
			  //  continue;


			float matr[16];
			uintptr_t viewMatrixAddr = 0x501ae8;
			for (unsigned int i = 0; i < 16; i++) {

				uintptr_t m = viewMatrixAddr + (i * 0x4);
				ReadProcessMemory(hack->hProc, (BYTE*)m, &matr[i], sizeof(matr[i]), nullptr);
			}

			D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 0, 0);

			Vector2 screen;
			if (hack->WorldToScreen(currEntPos, screen, matr)) {
				drawLine(screen.x, screen.y, hack->windowWidth / 2, hack->windowHeight, color);

				Vector3 entHeadPos = hack->RPM<Vector3>(currEnt + 0x4);
				Vector2 screenHead;
				hack->WorldToScreen(entHeadPos, screenHead, matr);

				float entHeight = abs(screen.y - screenHead.y);
				float entWidth = entHeight / 2;
				drawRectangle(screen.x - entWidth / 2, screen.y - entHeight, entWidth, entHeight, color);

				drawText((char*)"We here", width / 10, height / 10, 255, 171, 0, 182);
			}
		}
	}

	device->EndScene();
	device->PresentEx(0, 0, 0, 0, 0);

	return 0;
}

void Paint::drawText(char* String, int x, int y, int a, int r, int g, int b)
{
	RECT rect;
	rect.top = y;
	rect.left = x;
	font->DrawTextA(0, String, strlen(String), &rect, DT_NOCLIP, D3DCOLOR_ARGB(a, r, g, b));
}

void Paint::drawRectangle(int x, int y, int width, int height, D3DCOLOR color) {
	ID3DXLine* LineL;
	D3DXCreateLine(device, &LineL);

	D3DXVECTOR2 rect[5];
	rect[0] = D3DXVECTOR2(x, y);
	rect[1] = D3DXVECTOR2(x + width, y);
	rect[2] = D3DXVECTOR2(x + width, y + height);
	rect[3] = D3DXVECTOR2(x, y + height);
	rect[4] = D3DXVECTOR2(x, y);


	LineL->SetWidth(2);
	LineL->Draw(rect, 5, color);
	LineL->Release();
}

void Paint::drawLine(int x1, int y1, int x2, int y2, D3DCOLOR color) {
	ID3DXLine* LineL;
	D3DXCreateLine(device, &LineL);

	D3DXVECTOR2 Line[2];
	Line[0] = D3DXVECTOR2(x1, y1);
	Line[1] = D3DXVECTOR2(x2, y2);
	LineL->SetWidth(2);
	LineL->Draw(Line, 2, color);
	LineL->Release();
}

Paint::Paint() {}

Paint::Paint(HWND overlayHWND, HWND targetHWND, int width, int height) {
	this->width = width;
	this->height = height;
	this->TargetHWND = targetHWND;
	init(overlayHWND);
}
