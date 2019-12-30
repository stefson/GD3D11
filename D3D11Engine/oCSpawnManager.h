#pragma once
#include "pch.h"
#include "HookedFunctions.h"
#include "Engine.h"
#include "GothicAPI.h"
#include "BaseGraphicsEngine.h"
#include "oCGame.h"
#include "zCVob.h"
#include "oCNPC.h"

class oCSpawnManager
{
public:

	/** Hooks the functions of this Class */
	static void Hook()
	{
		HookedFunctions::OriginalFunctions.original_oCSpawnManagerSpawnNpc = (oCSpawnManagerSpawnNpc)DetourFunction((BYTE*)GothicMemoryLocations::oCSpawnManager::SpawnNpc, (BYTE*)oCSpawnManager::hooked_oCSpawnManagerSpawnNpc);
		//HookedFunctions::OriginalFunctions.original_oCSpawnManagerCheckInsertNpc = (oCSpawnManagerCheckInsertNpc)DetourFunction((BYTE *)GothicMemoryLocations::oCSpawnManager::CheckInsertNpc, (BYTE *)oCSpawnManager::hooked_oCSpawnManagerCheckInsertNpc);
		
		// TODO: #8
		//HookedFunctions::OriginalFunctions.original_oCSpawnManagerCheckRemoveNpc = (oCSpawnManagerCheckRemoveNpc)DetourFunction((BYTE*)GothicMemoryLocations::oCSpawnManager::CheckRemoveNpc, (BYTE*)oCSpawnManager::hooked_oCSpawnManagerCheckRemoveNpc);
	}

	/** Reads config stuff */
	static void __fastcall hooked_oCSpawnManagerSpawnNpc(void * thisptr, void * unknwn, oCNPC* npc, const DirectX::SimpleMath::Vector3 & position, float f)
	{
		hook_infunc
		HookedFunctions::OriginalFunctions.original_oCSpawnManagerSpawnNpc(thisptr, npc, position, f);

		if (npc->GetSleepingMode() != 0 || npc->IsAPlayer())
		{
			Engine::GAPI->OnRemovedVob((zCVob *)npc, ((zCVob *)npc)->GetHomeWorld());	
			Engine::GAPI->OnAddVob((zCVob *)npc, ((zCVob *)npc)->GetHomeWorld());
		}
		hook_outfunc
	}

	static int __fastcall hooked_oCSpawnManagerCheckRemoveNpc(void* thisptr, void* unknwn, oCNPC* npc)
	{
		hook_infunc
		Engine::GAPI->SetCanClearVobsByVisual();
		auto res = HookedFunctions::OriginalFunctions.original_oCSpawnManagerCheckRemoveNpc(thisptr, npc);
		Engine::GAPI->SetCanClearVobsByVisual(false);
		return res;
		hook_outfunc
		
		return 0;
	}

	/** Reads config stuff */
	static void __fastcall hooked_oCSpawnManagerCheckInsertNpc(void* thisptr, void* unknwn)
	{
		hook_infunc
		HookedFunctions::OriginalFunctions.original_oCSpawnManagerCheckInsertNpc(thisptr);
		hook_outfunc
	}
};



