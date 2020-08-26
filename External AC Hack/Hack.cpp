#include "Hack.h"
#include "Proc.h"

void Hack::Init() {
	DWORD procId = GetProcId(L"ac_client.exe");
	moduleBaseAddr = GetModuleBaseAddress(procId, L"ac_client.exe");
	hProc = 0;
	hProc = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

	uintptr_t localEntAddr = moduleBaseAddr + 0x10F4F4;
	uintptr_t windowWidthAddr = moduleBaseAddr + 0x110C94;
	uintptr_t windowHeightAddr = moduleBaseAddr + 0x110C98;

	viewMatrixAddr = moduleBaseAddr + 0x101AE8;
	for (unsigned int i = 0; i < 16; i++) {

		uintptr_t m = viewMatrixAddr + (i * 0x4);
		ReadProcessMemory(hProc, (BYTE*)m, &viewMatrix[i], sizeof(m), nullptr);
	}

	entityListAddr = moduleBaseAddr + 0x10F4F8;
	//ReadProcessMemory(hProc, (BYTE*)windowWidthAddr, &windowWidth, sizeof(windowWidth), nullptr);
	//ReadProcessMemory(hProc, (BYTE*)windowHeightAddr, &windowHeight, sizeof(windowHeight), nullptr);
	//entityList = (EntityList*)entityListAddr;
	//localEntity = (Entity*)localEntAddr;
	ReadProcessMemory(hProc, (BYTE*)localEntAddr, &localEntity, sizeof(localEntity), nullptr);
	ReadProcessMemory(hProc, (BYTE*)entityListAddr, &entityList, sizeof(entityList), nullptr);
	localEntityAddr = localEntAddr;

	windowWidth = RPM<int>(windowWidthAddr);
	windowHeight = RPM<int>(windowHeightAddr);
}

void Hack::Update() {
	memcpy(&viewMatrix, (PBYTE*)viewMatrixAddr, sizeof(viewMatrix));
}

bool Hack::CheckValidEnt(Entity* ent) {
	if (ent == nullptr) return false;
	if (ent == localEntity) return false;
	if (ent->Health <= 0) return false;
	return true;
}

bool Hack::WorldToScreen(Vector3 pos, Vector2& screen, float matrix[16]) {
	//Matrix-vector Product, multiplying world(eye) coordinates by projection matrix = clipCoords
	Vector4 clipCoords;
	clipCoords.x = pos.x * matrix[0] + pos.y * matrix[4] + pos.z * matrix[8] + matrix[12];
	clipCoords.y = pos.x * matrix[1] + pos.y * matrix[5] + pos.z * matrix[9] + matrix[13];
	clipCoords.z = pos.x * matrix[2] + pos.y * matrix[6] + pos.z * matrix[10] + matrix[14];
	clipCoords.w = pos.x * matrix[3] + pos.y * matrix[7] + pos.z * matrix[11] + matrix[15];

	if (clipCoords.w < 0.1f)
		return false;

	//perspective division, dividing by clip.W = Normalized Device Coordinates
	Vector3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
	return true;
}