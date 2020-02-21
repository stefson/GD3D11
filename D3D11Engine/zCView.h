#pragma once
#include "pch.h"
#include "zTypes.h"
#include "HookedFunctions.h"
#include "oCGame.h"

#if BUILD_GOTHIC_2_6_fix
	#include "zViewTypes.h"
namespace zView {
	enum EMenuRenderState {
		None = 0,
		Content,
	};

	static std::string FONT_DEFAULT = "FONT_DEFAULT.TGA";
	static std::string FONT_OLD_10_WHITE = "FONT_OLD_10_WHITE.TGA";
	static std::string FONT_OLD_10_WHITE_HI = "FONT_OLD_10_WHITE_HI.TGA";
	static std::string MENU_ITEM_CONTENT_VIEWER = "MENU_ITEM_CONTENT_VIEWER";
	
	static zColor DefaultColor = zColor(158, 186, 203, 255); // BGRA
}
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
		// .text:007A9A10                             ; int __thiscall zCView::FontSize(zCView *this, struct zSTRING *)
		XHook(HookedFunctions::OriginalFunctions.original_zCViewFontSize, GothicMemoryLocations::zCView::FontSize, hooked_FontSize);
		// .text:007A62A0; void __thiscall zCView::BlitText(zCView * __hidden this)
		XHook(HookedFunctions::OriginalFunctions.original_zCViewBlitText, GothicMemoryLocations::zCView::BlitText, hooked_BlitText);
#endif
	}

	static void SetMode(int x, int y, int bpp, HWND* window = nullptr)
	{
		hook_infunc
		HookedFunctions::OriginalFunctions.original_zCViewSetMode(x,y,bpp, window);
		hook_outfunc
	}

#if BUILD_GOTHIC_2_6_fix
	static void __fastcall hooked_BlitText(_zCView* thisptr, void* unknwn) {
		if (!Engine::GAPI->GetRendererState()->RendererSettings.EnableCustomFontRendering) {
			HookedFunctions::OriginalFunctions.original_zCViewBlitText(thisptr);
			return;
		}

		thisptr->CheckAutoScroll();
		thisptr->CheckTimedText();

		if (!thisptr->isOpen || !thisptr->maxTextLength) return;

		if (thisptr->owner) {
			if (thisptr->owner->vtbl == 0x830F84)
			{
				auto owner = (zCMenuItemText*)thisptr->owner;
				std::string ownerId = owner->id.ToChar();

				if (owner->GetIsDisabled()) return;
				if (owner->m_bDontRender) return;
				if (!owner->m_bVisible) return;
			}
		}

		int x, y, pposx, pposy;

		pposx = thisptr->pposx;
		pposy = thisptr->pposy;
		zCList <zCViewText>* textNode = thisptr->textLines.next;
		zCViewText* text = nullptr;

		

		zColor color = zView::DefaultColor;
		while (textNode) {

			text = textNode->data;
			textNode = textNode->next;

			x = pposx + thisptr->nax(text->posx);
			y = pposy + thisptr->nay(text->posy);

			// text->font auswerten!

			if (text->colored && !text->color.IsWhite()) {
				// TODO: REMOVE color.IsWhite() if Alpha Blending works!
				color = text->color;
			}
			else {
				color = zView::DefaultColor;
			}
			if (!thisptr->font) continue;

			std::string fontName = thisptr->font->name.ToChar();

			if ((!fontName.compare(zView::FONT_DEFAULT) || !fontName.compare(zView::FONT_OLD_10_WHITE) || !fontName.compare(zView::FONT_OLD_10_WHITE_HI))) {
				auto blendFunc = thisptr->alphafunc;
				auto col = !fontName.compare(zView::FONT_OLD_10_WHITE_HI) ? zCOLOR_WHITE : color;
				Engine::GraphicsEngine->DrawString(text->text.ToChar(), x, y, float4((float)col.bgra.r / 255.f, (float)col.bgra.g / 255.f, (float)col.bgra.b / 255.f, (float)col.bgra.alpha / 255.f), blendFunc);
			}
			else {
				if(text->font)
					thisptr->font = text->font;
				if (text->colored) {
					thisptr->color = text->color;
				}
				thisptr->PrintChars(x, y, text->text);
			}
		}
	}
	static int __fastcall hooked_FontSize(_zCView* thisptr, void* unknwn, const zSTRING& str) {
		
		if (!thisptr->font) return 0;

		int result = 0;
		std::string fontName = thisptr->font->name.ToChar();


		if (!fontName.compare(zView::FONT_DEFAULT) || !fontName.compare(zView::FONT_OLD_10_WHITE) || !fontName.compare(zView::FONT_OLD_10_WHITE_HI)) {
			// return Engine::GraphicsEngine->MeasureString(str.ToChar(), thisptr->font);
		}

		hook_infunc

		result =  HookedFunctions::OriginalFunctions.original_zCViewFontSize(thisptr, str);

		hook_outfunc

		return result;
	}
	
#endif

	/** Prints a message to the screen */
	void PrintTimed(int posX, int posY, const zSTRING& strMessage, float time = 3000.0f, DWORD* col = nullptr)
	{
		XCALL(GothicMemoryLocations::zCView::PrintTimed);
	}
};