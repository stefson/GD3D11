#include "pch.h"
#include "HookedFunctions.h"

#include "zCBspTree.h"
#include "zCWorld.h"
#include "oCGame.h"
#include "zCMaterial.h"
#include "zFILE.h"
#include "zCOption.h"
#include "zCRndD3D.h"
#include "zCParticleFX.h"
#include "zCView.h"
#include "CGameManager.h"
#include "zCVisual.h"
#include "zCTimer.h"
#include "zCModel.h"
#include "oCSpawnManager.h"
#include "zCVob.h"
#include "zCTexture.h"
#include "zCThread.h"
#include "zCResourceManager.h"
#include "zCQuadMark.h"
#include "oCNPC.h"
#include "zCSkyController_Outdoor.h"

#include "zQuat.h"
#include "zMat4.h"

#if _MSC_VER >= 1300
#include <Tlhelp32.h>
#endif

#include "StackWalker.h"

/** Init all hooks here */
void HookedFunctionInfo::InitHooks() {
    LogInfo() << "Initializing hooks";

    oCGame::Hook();
    zCBspTree::Hook();
    zCWorld::Hook();
    zCMaterial::Hook();
    zCBspNode::Hook();
    zFILE::Hook();
    zCOption::Hook();
    zCRndD3D::Hook();
    zCParticleFX::Hook();
    zCView::Hook();
    CGameManager::Hook();
    zCVisual::Hook();
    zCTimer::Hook();
    zCModel::Hook();
    zCModelPrototype::Hook();
    oCSpawnManager::Hook();
    zCVob::Hook();
    zCTexture::Hook();
    zCThread::Hook();
    //zCResourceManager::Hook();
    zCQuadMark::Hook();
    oCNPC::Hook();
    zCSkyController_Outdoor::Hook();

    //XHook(original_zCExceptionHandler_UnhandledExceptionFilter, GothicMemoryLocations::Functions::zCExceptionHandler_UnhandledExceptionFilter, HookedFunctionInfo::hooked_zCExceptionHandlerUnhandledExceptionFilter);
    //XHook(original_HandledWinMain, GothicMemoryLocations::Functions::HandledWinMain, HookedFunctionInfo::hooked_HandledWinMain);
    //XHook(original_ExitGameFunc, GothicMemoryLocations::Functions::ExitGameFunc, HookedFunctionInfo::hooked_ExitGameFunc);

    // Kill the check for doing freelook only in fullscreen, since we force the game to run windowed internally
    //int flSize = GothicMemoryLocations::GlobalObjects::NOP_FreelookWindowedCheckEnd - GothicMemoryLocations::GlobalObjects::NOP_FreelookWindowedCheckStart;
    //VirtualProtect((void *)GothicMemoryLocations::GlobalObjects::NOP_FreelookWindowedCheckStart, flSize, PAGE_EXECUTE_READWRITE, &dwProtect);
    //REPLACE_RANGE(GothicMemoryLocations::GlobalObjects::NOP_FreelookWindowedCheckStart, GothicMemoryLocations::GlobalObjects::NOP_FreelookWindowedCheckEnd-1, INST_NOP);

    // Hook the single bink-function
    XHook( GothicMemoryLocations::zCBinkPlayer::GetPixelFormat, HookedFunctionInfo::hooked_zBinkPlayerGetPixelFormat );

#if defined(BUILD_GOTHIC_2_6_fix) || defined(BUILD_GOTHIC_1_08k)
    XHook( original_zCBinkPlayerOpenVideo, GothicMemoryLocations::zCBinkPlayer::OpenVideo, HookedFunctionInfo::hooked_zBinkPlayerOpenVideo );
#endif
    original_Alg_Rotation3DNRad = (Alg_Rotation3DNRad)GothicMemoryLocations::Functions::Alg_Rotation3DNRad;

//G1 patches
#ifdef BUILD_GOTHIC_1_08k
#ifdef BUILD_1_12F
    LogInfo() << "Patching: Fix integer overflow crash";
    PatchAddr( 0x00506B31, "\xEB" );

    LogInfo() << "Patching: Marking texture as cached-in after cache-out";
    PatchAddr( 0x005E90BE, "\x90\x90" );
#else
    LogInfo() << "Patching: BroadCast fix";
    {
        char* zSPYwnd[5];
        DWORD zSPY = reinterpret_cast<DWORD>(FindWindowA( nullptr, "[zSpy]" ));
        memcpy( zSPYwnd, &zSPY, 4 );
        PatchAddr( 0x00447F5A, zSPYwnd );
        PatchAddr( 0x00449280, zSPYwnd );
        PatchAddr( 0x004480AF, zSPYwnd );
    }

    LogInfo() << "Patching: LOW_FPS_NaN_check";
    PatchAddr( 0x007CF732, "\x81\x3B\x00\x00\xC0\xFF\x0F\x84\x3B\xFF\xD4\xFF\x81\x3B\x00\x00\xC0\x7F\x0F\x84\x2F\xFF\xD4\xFF\xD9\x03\x8D\x44\x8C\x1C\xE9\x33\xFF\xD4\xFF" );
    PatchAddr( 0x0051F682, "\xE9\xAB\x00\x2B\x00\x90" );
    PatchAddr( 0x007CF755, "\x81\x7C\xE4\x20\x00\x00\xC0\xFF\x0F\x84\x43\xF0\xD4\xFF\x81\x7C\xE4\x20\x00\x00\xC0\x7F\x0F\x84\x35\xF0\xD4\xFF\xE9\xDA\xEF\xD4\xFF" );
    PatchAddr( 0x005F0EAA, "\xE8\xA6\xE8\x1D\x00" );

    LogInfo() << "Patching: Fix Morph Mesh animation glitch at 60hz";
    PatchAddr( 0x0074086D, "\xD9\x05\xEC\xF1\x8C\x00\xD8\x05\x10\x11\x7D\x00\xE9\x4B\x66\xE4\xFF" );
    PatchAddr( 0x00586EC3, "\xE9\xA5\x99\x1B\x00\x90" );
    PatchAddr( 0x0074087E, "\xD9\x05\xEC\xF1\x8C\x00\xD8\x05\x10\x11\x7D\x00\xE9\x6A\x66\xE4\xFF" );
    PatchAddr( 0x00586EF3, "\xE9\x86\x99\x1B\x00\x90" );

    LogInfo() << "Patching: Fix integer overflow crash";
    PatchAddr( 0x004F4024, "\xEB" );
    PatchAddr( 0x004F43FC, "\xEB" );

    LogInfo() << "Patching: Marking texture as cached-in after cache-out";
    PatchAddr( 0x005CA683, "\x90\x90" );
#endif
#endif

//G2 patches
#ifdef BUILD_GOTHIC_2_6_fix
    zQuat::Hook();
    zMat4::Hook();

    LogInfo() << "Patching: BroadCast fix";
    {
        char* zSPYwnd[5];
        DWORD zSPY = reinterpret_cast<DWORD>(FindWindowA( nullptr, "[zSpy]" ));
        memcpy( zSPYwnd, &zSPY, 4 );
        PatchAddr( 0x0044C5DA, zSPYwnd );
        PatchAddr( 0x0044D9A0, zSPYwnd );
        PatchAddr( 0x0044C72F, zSPYwnd );
    }

    // Workaround to fix disappearing ui elements under certain circumstances
    // e.g. praying at Beliar statue, screen blend causing dialog boxes to disappear.
    LogInfo() << "Patching: Overriding zCVobScreenFX::OnTick() if (blend.visible) -> if (false)";
    PatchAddr( 0x0061808F, "\x90\xE9" );

    LogInfo() << "Patching: Interupt gamestart sound";
    PatchAddr( 0x004DB89F, "\x00" );

    LogInfo() << "Patching: Fix low framerate";
    PatchAddr( 0x004DDC6F, "\x08" );

    LogInfo() << "Patching: LOW_FPS_NaN_check";
    PatchAddr( 0x0066E59A, "\x81\x3A\x00\x00\xC0\xFF\x0F\x84\xF3\x3C\xEC\xFF\x81\x3A\x00\x00\xC0\x7F\x0F\x84\xE7\x3C\xEC\xFF\xD9\x45\x00\x8D\x44\x8C\x20\xE9\xEB\x3C\xEC\xFF" );
    PatchAddr( 0x005322A2, "\xE9\xF3\xC2\x13\x00\x90\x90" );
    PatchAddr( 0x0066E5BE, "\x81\x7C\xE4\x20\x00\x00\xC0\xFF\x0F\x84\x2A\x2B\xEC\xFF\x81\x7C\xE4\x20\x00\x00\xC0\x7F\x0F\x84\x1C\x2B\xEC\xFF\xE9\xC1\x2A\xEC\xFF" );
    PatchAddr( 0x0061E412, "\xE8\xA7\x01\x05\x00" );

    LogInfo() << "Patching: Fix integer overflow crash";
    PatchAddr( 0x00502F94, "\xEB" );
    PatchAddr( 0x00503343, "\xEB" );

    LogInfo() << "Patching: Texture size is lower than 32 - fix";
    PatchAddr( 0x005F4E20, "\xC7\x05\xBC\xB3\x99\x00\x00\x40\x00\x00\xEB\x4D\x90\x90" );

    LogInfo() << "Patching: Marking texture as cached-in after cache-out";
    PatchAddr( 0x005F5573, "\x90\x90" );

    // HACK Workaround to fix debuglines in godmode
    LogInfo() << "Patching: Godmode Debuglines";
    // oCMagFrontier::GetDistanceNewWorld
    PatchAddr( 0x00473f37, "\xBD\x00\x00\x00\x00" ); // replace MOV EBP, 0x1 with MOV EBP, 0x0
    // oCMagFrontier::GetDistanceDragonIsland
    PatchAddr( 0x004744c1, "\xBF\x00\x00\x00\x00" ); // replace MOV EDI, 0x1 with MOV EDI, 0x0
    // oCMagFrontier::GetDistanceAddonWorld
    PatchAddr( 0x00474681, "\xBF\x00\x00\x00\x00" ); // replace MOV EDI, 0x1 with MOV EDI, 0x0
#endif
}

/** Function hooks */
int __stdcall HookedFunctionInfo::hooked_HandledWinMain( HINSTANCE hInstance, HINSTANCE hPrev, LPSTR szCmdLine, int sw ) {
    int r = HookedFunctions::OriginalFunctions.original_HandledWinMain( hInstance, hPrev, szCmdLine, sw );

    return r;
}

void __fastcall HookedFunctionInfo::hooked_zCActiveSndAutoCalcObstruction( void* thisptr, void* unknwn, int i ) {
    // Just do nothing here. Something was inside zCBspTree::Render that managed this and thus voices get really quiet in indoor locations
    // This function is for calculating the automatic volume-changes when the camera goes in/out buildings
    // We keep everything on the same level by removing it
}

void __cdecl HookedFunctionInfo::hooked_ExitGameFunc() {
    Engine::OnShutDown();

    HookedFunctions::OriginalFunctions.hooked_ExitGameFunc();
}

long __stdcall HookedFunctionInfo::hooked_zCExceptionHandlerUnhandledExceptionFilter( EXCEPTION_POINTERS* exceptionPtrs ) {
    return HookedFunctions::OriginalFunctions.original_zCExceptionHandler_UnhandledExceptionFilter( exceptionPtrs );
}

/** Returns the pixelformat of a bink-surface */
long __fastcall HookedFunctionInfo::hooked_zBinkPlayerGetPixelFormat( void* thisptr, void* unknwn, zTRndSurfaceDesc& desc ) {
    int* cd = (int*)&desc;

    // Resolution is at pos [2] and [3]
    //cd[2] = Engine::GraphicsEngine->GetResolution().x;
    //cd[3] = Engine::GraphicsEngine->GetResolution().y;

    /*for(int i=0;i<0x7C;i++)
    {
    cd[i] = i;
    }*/

    return 4; // 4 satisfies gothic enough to play the video
    //Global::HookedFunctions.zBinkPlayerGetPixelFormat(thisptr, desc);
}

int __fastcall HookedFunctionInfo::hooked_zBinkPlayerOpenVideo( void* thisptr, void* unknwn, zSTRING str ) {
    int r = HookedFunctions::OriginalFunctions.original_zCBinkPlayerOpenVideo( thisptr, str );

    struct BinkInfo {
        unsigned int ResX;
        unsigned int ResY;
        // ... unimportant
    };

    // Grab the resolution
    // This structure stores width and height as first two parameters, as ints.
    BinkInfo* res = *(BinkInfo**)(((char*)thisptr) + (GothicMemoryLocations::zCBinkPlayer::Offset_VideoHandle));

    if ( res ) {
        Engine::GAPI->GetRendererState().RendererInfo.PlayingMovieResolution = INT2( res->ResX, res->ResY );
    }

    return r;
}
