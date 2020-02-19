#pragma once
#include "pch.h"
#include "zTypes.h"
#include "HookedFunctions.h"
#include "oCGame.h"

#if BUILD_GOTHIC_2_6_fix
	#include "zViewTypes.h"
#endif

class zCView
{
public:
	/** Hooks the functions of this Class */
	static void Hook()
	{
		DWORD dwProtect;

		HookedFunctions::OriginalFunctions.original_zCViewSetMode = (zCViewSetMode)GothicMemoryLocations::zCView::SetMode;

		VirtualProtect((void *)GothicMemoryLocations::zCView::SetMode, 0x1B9, PAGE_EXECUTE_READWRITE, &dwProtect); // zCView::SetMode

		// Replace the actual mode-change in zCView::SetMode. Only do the UI-Changes.
		REPLACE_RANGE(GothicMemoryLocations::zCView::REPL_SetMode_ModechangeStart, GothicMemoryLocations::zCView::REPL_SetMode_ModechangeEnd-1, INST_NOP);
		
#if BUILD_GOTHIC_2_6_fix
		XHook(HookedFunctions::OriginalFunctions.original_zCViewPrintChars, GothicMemoryLocations::zCView::PrintChars, hooked_PrintChars);
#endif
	}

	static void SetMode(int x, int y, int bpp, HWND* window = nullptr)
	{
		hook_infunc
		HookedFunctions::OriginalFunctions.original_zCViewSetMode(x,y,bpp, window);
		hook_outfunc
	}

#if BUILD_GOTHIC_2_6_fix

	static void __fastcall hooked_PrintChars(_zCView* thisptr, void* unknwn, int x, int y, const zSTRING& str) {
		// zColor (bgra) To RGBA
		//auto col = thisptr->GetFontColor();

        bool visible = thisptr->isOpen;

        if (thisptr->owner) {
            if (thisptr->owner->vtbl == 0x830F84)
            {
                auto owner = (zCMenuItemText*)thisptr->owner;
                visible = owner->m_bVisible && !owner->m_bDontRender;
            }
        }

		if (Engine::GAPI->GetRendererState()->RendererSettings.EnableCustomFontRendering)
		{
			if (visible) {
				auto blendFunc = thisptr->alphafunc;
                auto col = thisptr->GetTextColor();
				Engine::GraphicsEngine->DrawString(str.ToChar(), x, y, float4((float)col.bgra.r / 255.f, (float)col.bgra.g / 255.f, (float)col.bgra.b / 255.f, (float)col.bgra.alpha / 255.f), blendFunc);
			}
		}
		else
		{
			hook_infunc

			HookedFunctions::OriginalFunctions.original_zCViewPrintChars(thisptr, x, y, str);
		
			hook_outfunc
		}
	}
#endif

	/** Prints a message to the screen */
	void PrintTimed(int posX, int posY, const zSTRING& strMessage, float time = 3000.0f, DWORD* col = nullptr)
	{
		XCALL(GothicMemoryLocations::zCView::PrintTimed);
	}
};