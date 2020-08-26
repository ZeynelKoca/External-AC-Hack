#pragma once

#include <vector>
#include <windows.h>
#include <TlHelp32.h>
#include "Vector.h"

class Entity
{
public:
	Vector3 HeadPosition; //0x0004
	char pad_0010[36]; //0x0010
	Vector3 Position; //0x0034
	Vector3 ViewAngle; //0x0040
	char pad_004C[172]; //0x004C
	int32_t Health; //0x00F8
	char pad_00FC[24]; //0x00FC
	int32_t MagAmmo2; //0x0114
	char pad_0118[16]; //0x0118
	int32_t MagAmmo; //0x0128
	char pad_012C[16]; //0x012C
	int32_t ClipAmmo2; //0x013C
	char pad_0140[16]; //0x0140
	int32_t ClipAmmo; //0x0150
	char pad_0154[36]; //0x0154
	int32_t GunCooldown; //0x0178
	char pad_017C[169]; //0x017C
	char Name[16]; //0x0225
}; //Size: 0x0850

class EntityObject {
public:
	char padding[4];
	Entity* ent;
};

class EntityList
{
public:
	EntityObject* entities[64];
};


class Hack
{
public:
	HANDLE hProc;

	uintptr_t moduleBaseAddr;
	uintptr_t viewMatrixAddr;
	uintptr_t entityListAddr;

	Entity* localEntity;
	EntityList* entityList;
	float viewMatrix[16];
	int windowWidth, windowHeight;

	uintptr_t localEntityAddr;

	void Init();
	void Update();
	bool CheckValidEnt(Entity* ent);
	bool WorldToScreen(Vector3 pos, Vector2& screen, float matrix[16]);


	template<typename T> T RPM(SIZE_T address) {

		T buffer;
		ReadProcessMemory(hProc, (BYTE*)address, &buffer, sizeof(buffer), nullptr);

		return buffer;
	}
};

