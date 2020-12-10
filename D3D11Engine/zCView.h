#pragma once
#include "pch.h"
#include "zTypes.h"
#include "HookedFunctions.h"
#include "oCGame.h"

#if BUILD_GOTHIC_2_6_fix
#include "zViewTypes.h"
#endif

class zCView {
public:

	/** Hooks the functions of this Class */
	static void Hook() {
		DWORD dwProtect;

		HookedFunctions::OriginalFunctions.original_zCViewSetMode = (zCViewSetMode)GothicMemoryLocations::zCView::SetMode;

		VirtualProtect( (void*)GothicMemoryLocations::zCView::SetMode, 0x1B9, PAGE_EXECUTE_READWRITE, &dwProtect ); // zCView::SetMode

		// Replace the actual mode-change in zCView::SetMode. Only do the UI-Changes.
		REPLACE_RANGE( GothicMemoryLocations::zCView::REPL_SetMode_ModechangeStart, GothicMemoryLocations::zCView::REPL_SetMode_ModechangeEnd - 1, INST_NOP );

#if BUILD_GOTHIC_2_6_fix
		// .text:007A9A10                             ; int __thiscall zCView::FontSize(zCView *this, struct zSTRING *)
		//XHook( HookedFunctions::OriginalFunctions.original_zCViewFontSize, GothicMemoryLocations::zCView::FontSize, hooked_FontSize );
		// .text:007A62A0; void __thiscall zCView::BlitText(zCView * __hidden this)
		XHook( HookedFunctions::OriginalFunctions.original_zCViewBlitText, GothicMemoryLocations::zCView::BlitText, hooked_BlitText );
		XHook( HookedFunctions::OriginalFunctions.original_zCViewPrint, GothicMemoryLocations::zCView::Print, hooked_Print );
#endif
	}

	static void SetMode( int x, int y, int bpp, HWND* window = nullptr ) {
		hook_infunc
			HookedFunctions::OriginalFunctions.original_zCViewSetMode( x, y, bpp, window );
		hook_outfunc
	}

#if BUILD_GOTHIC_2_6_fix
	static void __fastcall hooked_Print( _zCView* thisptr, void* unknwn, int x, int y, const zSTRING& s ) {
		if ( !Engine::GAPI->GetRendererState().RendererSettings.EnableCustomFontRendering ) {
			HookedFunctions::OriginalFunctions.original_zCViewPrint( thisptr, x, y, s );
			return;
		}
		if ( !thisptr->font ) return;
		thisptr->scrollTimer = 0;

		// Instantly blit Viewport/global-screen
		if ( (thisptr->viewID == 1)
			/*|| (thisptr == Engine::GAPI->GetScreen())*/
			|| (thisptr->viewID == 0) ) {
			Engine::GraphicsEngine->DrawString(
				s.ToChar(),
				thisptr->pposx + thisptr->nax(x), thisptr->pposy + thisptr->nay(y),
				thisptr);
		} else {
			// call zCView::Print to create the Text for later blit
			HookedFunctions::OriginalFunctions.original_zCViewPrint( thisptr, x, y, s );
		}
	}

	static void __fastcall hooked_BlitText( _zCView* thisptr, void* unknwn ) {
		if ( !Engine::GAPI->GetRendererState().RendererSettings.EnableCustomFontRendering ) {
			HookedFunctions::OriginalFunctions.original_zCViewBlitText( thisptr );
			return;
		}

		thisptr->CheckAutoScroll();
		thisptr->CheckTimedText();

		if ( !thisptr->isOpen || !thisptr->maxTextLength ) return;

		if ( thisptr->owner ) {
			if ( thisptr->owner->vtbl == 0x830F84 ) {
				auto owner = (zCMenuItemText*)thisptr->owner;
				const std::string& ownerId = owner->id.ToChar();

				if ( owner->GetIsDisabled() ) return;
				if ( owner->m_bDontRender ) return;
				if ( !owner->m_bVisible ) return;
			}
		}

		int x, y, pposx, pposy;

		pposx = thisptr->pposx;
		pposy = thisptr->pposy;
		zCList <zCViewText>* textNode = thisptr->textLines.next;
		zCViewText* text = nullptr;


		while ( textNode ) {

			text = textNode->data;
			textNode = textNode->next;

			x = pposx + thisptr->nax( text->posx );
			// TODO: Remove additional addition if we get the correct char positioning
			y = pposy + thisptr->nay( text->posy ) - 2;

			// text->font auswerten!

			if ( !thisptr->font ) continue;

			const std::string& fontName = thisptr->font->name.ToChar();

			Engine::GraphicsEngine->DrawString( text->text.ToChar(), x, y, thisptr );
		}
	}
	static int __fastcall hooked_FontSize( _zCView* thisptr, void* unknwn, const zSTRING& str ) {
		return thisptr->anx(Engine::GraphicsEngine->MeasureString(str.ToChar()));
	}

#endif

	/** Prints a message to the screen */
	void PrintTimed( int posX, int posY, const zSTRING& strMessage, float time = 3000.0f, DWORD* col = nullptr ) {
		XCALL( GothicMemoryLocations::zCView::PrintTimed );
	}
};