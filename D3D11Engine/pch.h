#pragma once

#pragma warning(disable: 4731) // Change of ebp from inline assembly
#pragma warning(disable: 4244) // Loss of data during conversion
#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <future>
#include <list>
#include <map>
#include <mmsystem.h>
#include <set>
#include <signal.h>
#include <unordered_map>
#include <unordered_set>

#include "Logger.h"
#include "Types.h"
#include "VertexTypes.h"

#if _MSC_VER < 1900
;
#else
#define stdext std
#endif

#define VERSION_NUMBER "17.6"
__declspec(selectany) const char* VERSION_NUMBER_STR = VERSION_NUMBER;

extern bool GMPModeActive;

/** D3D7-Call logging */
#define DebugWriteValue(value, check) if (value == check) { LogInfo() << " - " << #check; }
#define DebugWriteFlag(value, check) if ((value & check) == check) { LogInfo() << " - " << #check; }
#define DebugWrite(debugMessage) DebugWrite_i(debugMessage, (void *) this);

/** Debugging */
#define SAFE_RELEASE(x) if (x) { x->Release(); x = nullptr; }
#define SAFE_DELETE(x) delete x; x = nullptr;
//#define V(x) x

/** Writes a string of the D3D7-Call log */
void DebugWrite_i(LPCSTR lpDebugMessage, void * thisptr);

/** Computes the size in bytes of the given FVF */
int ComputeFVFSize(DWORD fvf);

static DirectX::XMFLOAT4X4 D3DXMatToDX(const D3DXMATRIX& r) {
	return DirectX::XMFLOAT4X4(
		r._11, r._12, r._13, r._14, 
		r._21, r._22, r._23, r._24,
		r._31, r._32, r._33, r._34,
		r._41, r._42, r._43, r._44);
}

static D3DXMATRIX DX4x4ToD3DXMat(const DirectX::XMFLOAT4X4& r) {

	return D3DXMATRIX(
		r._11, r._12, r._13, r._14,
		r._21, r._22, r._23, r._24,
		r._31, r._32, r._33, r._34,
		r._41, r._42, r._43, r._44);
}
