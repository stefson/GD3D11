#include "MyDirectDrawSurface7.h"
#include "../Engine.h"
#include "../GothicAPI.h"
#include "../BaseGraphicsEngine.h"
#include "../D3D11Texture.h"
#include "../zCTexture.h"

#define DebugWriteTex(x)  DebugWrite(x)

const std::string LEAF_SUBSTR[] = { "Treetop", "Bush", "Leaf" };

MyDirectDrawSurface7::MyDirectDrawSurface7() {
    refCount = 1;
    EngineTexture = nullptr;
    Normalmap = nullptr;
    FxMap = nullptr;
    LockedData = nullptr;
    GothicTexture = nullptr;
    IsReady = false;
    TextureType = ETextureType::TX_UNDEF;
    LockType = 0;

    // Check for test-bind mode to figure out what zCTexture-Object we are associated with
    std::string bound;
    if ( Engine::GAPI->IsInTextureTestBindMode( bound ) ) {
        Engine::GAPI->SetTextureTestBindMode( false, "" );
        return;
    }
}

MyDirectDrawSurface7::~MyDirectDrawSurface7() {
    Engine::GAPI->RemoveSurface( this );

    // Release mip-map chain first
    for ( LPDIRECTDRAWSURFACE7 mipmap : attachedSurfaces ) {
        mipmap->Release();
    }

    // Sometimes gothic doesn't unlock a surface or this is a movie-buffer
    delete[] LockedData;

    delete EngineTexture;
    delete Normalmap;
    delete FxMap;
}

/** Returns the engine texture of this surface */
D3D11Texture* MyDirectDrawSurface7::GetEngineTexture() {
    return EngineTexture;
}

/** Returns the engine texture of this surface */
D3D11Texture* MyDirectDrawSurface7::GetNormalmap() {
    return Normalmap;
}

/** Returns the fx-map for this surface */
D3D11Texture* MyDirectDrawSurface7::GetFxMap() {
    return FxMap;
}

/** Binds this texture */
void MyDirectDrawSurface7::BindToSlot( int slot ) {
    if ( !IsReady ) {
        Engine::GraphicsEngine->UnbindTexture( 0 );
        return; // Don't bind half-loaded textures!
    }

    if ( EngineTexture ) // Needed sometimes
        EngineTexture->BindToPixelShader( slot );

    if ( Normalmap ) {
        Normalmap->BindToPixelShader( slot + 1 );
        Normalmap->BindToVertexShader( 0 );
    } else {
        //EngineTexture->BindToPixelShader(slot + 1);
        Engine::GraphicsEngine->UnbindTexture( slot + 1 );
    }
}

/** Loads additional resources if possible */
void MyDirectDrawSurface7::LoadAdditionalResources( zCTexture* ownedTexture ) {
    if ( !GothicTexture ) {
        GothicTexture = ownedTexture;
        TextureName = GothicTexture->GetNameWithoutExt();

        // Find texture type
        if ( Toolbox::StringContainsOneOf( TextureName, LEAF_SUBSTR, ARRAYSIZE( LEAF_SUBSTR ) ) ) {
            TextureType = ETextureType::TX_LEAF;
        }

        Engine::GAPI->AddSurface( TextureName, this );
    }

    if ( Normalmap ) {
        SAFE_DELETE( Normalmap );
    }

    if ( FxMap ) {
        SAFE_DELETE( FxMap );
    }

    if ( TextureName.empty() || Normalmap || FxMap || !Engine::GAPI->GetRendererState().RendererSettings.AllowNormalmaps ) {
        return;
    }

    D3D11Texture* fxMapTexture = nullptr;
    D3D11Texture* nrmmapTexture = nullptr;

    // Check for normalmap in our mods folder first, then in the original games
    int j = 0;
    std::string replacementsFolder = "system\\GD3D11\\textures\\replacements\\Normalmaps_" + std::to_string( j );
    while ( Toolbox::FolderExists( replacementsFolder ) ) {
        std::string normalmap = replacementsFolder + "\\" + TextureName + "_normal.dds";
        if ( Toolbox::FileExists( normalmap ) ) {
            // Create the texture object this is linked with
            Engine::GraphicsEngine->CreateTexture( &nrmmapTexture );
            if ( XR_SUCCESS != nrmmapTexture->Init( normalmap ) ) {
                SAFE_DELETE( nrmmapTexture );
                LogWarn() << "Failed to load normalmap!";
            }
            break; // No need to check the other folders
        }
        j++;
        replacementsFolder = "system\\GD3D11\\textures\\replacements\\Normalmaps_" + std::to_string( j );
    }
    std::string normalmap = "system\\GD3D11\\textures\\replacements\\Normalmaps_" + Engine::GAPI->GetGameName() + "\\" + TextureName + "_normal.dds";
    if ( !nrmmapTexture && Toolbox::FileExists( normalmap ) ) {
        // Create the texture object this is linked with
        Engine::GraphicsEngine->CreateTexture( &nrmmapTexture );
        if ( XR_SUCCESS != nrmmapTexture->Init( normalmap ) ) {
            SAFE_DELETE( nrmmapTexture );
            LogWarn() << "Failed to load normalmap!";
        }
    }

    j = 0;
    replacementsFolder = "system\\GD3D11\\textures\\replacements\\Normalmaps_" + std::to_string( j );
    while ( Toolbox::FolderExists( replacementsFolder ) ) {
        std::string fxMap = replacementsFolder + "\\" + TextureName + "_fx.dds";
        if ( Toolbox::FileExists( fxMap ) ) {
            // Create the texture object this is linked with
            Engine::GraphicsEngine->CreateTexture( &fxMapTexture );
            if ( XR_SUCCESS != fxMapTexture->Init( fxMap ) ) {
                SAFE_DELETE( fxMapTexture );
                LogWarn() << "Failed to load normalmap!";
            }
            break; // No need to check the other folders
        }
        j++;
        replacementsFolder = "system\\GD3D11\\textures\\replacements\\Normalmaps_" + std::to_string( j );
    }
    std::string fxMap = "system\\GD3D11\\textures\\replacements\\Normalmaps_" + Engine::GAPI->GetGameName() + "\\" + TextureName + "_fx.dds";
    if ( !fxMapTexture && Toolbox::FileExists( fxMap ) ) {
        // Create the texture object this is linked with
        Engine::GraphicsEngine->CreateTexture( &fxMapTexture );
        if ( XR_SUCCESS != fxMapTexture->Init( fxMap ) ) {
            SAFE_DELETE( fxMapTexture );
            LogWarn() << "Failed to load normalmap!";
        }
    }

    Normalmap = nrmmapTexture;
    FxMap = fxMapTexture;
}

HRESULT MyDirectDrawSurface7::QueryInterface( REFIID riid, LPVOID* ppvObj ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::QueryInterface(%s)" );
    return S_OK;
}

ULONG MyDirectDrawSurface7::AddRef() {
    DebugWriteTex( "IDirectDrawSurface7(%p)::AddRef(%i)" );

    refCount++;
    return refCount;
}

ULONG MyDirectDrawSurface7::Release() {
    refCount--;
    ULONG uRet = refCount;
    DebugWriteTex( "IDirectDrawSurface7(%p)::Release(%i)" );

    if ( uRet == 0 ) {
        delete this;
    }

    return uRet;
}

HRESULT MyDirectDrawSurface7::AddAttachedSurface( LPDIRECTDRAWSURFACE7 lpDDSAttachedSurface ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::AddAttachedSurface()" );
    attachedSurfaces.push_back( (MyDirectDrawSurface7*)lpDDSAttachedSurface );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::AddOverlayDirtyRect( LPRECT lpRect ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::AddOverlayDirtyRect()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::Blt( LPRECT lpDestRect, LPDIRECTDRAWSURFACE7 lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::Blt()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::BltBatch( LPDDBLTBATCH lpDDBltBatch, DWORD dwCount, DWORD dwFlags ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::BltBatch()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::BltFast( DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE7 lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwTrans ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::BltFast()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::DeleteAttachedSurface( DWORD dwFlags, LPDIRECTDRAWSURFACE7 lpDDSAttachedSurface ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::DeleteAttachedSurface()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::EnumAttachedSurfaces( LPVOID lpContext, LPDDENUMSURFACESCALLBACK7 lpEnumSurfacesCallback ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::EnumAttachedSurfaces()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::EnumOverlayZOrders( DWORD dwFlags, LPVOID lpContext, LPDDENUMSURFACESCALLBACK7 lpfnCallback ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::EnumOverlayZOrders()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::Flip( LPDIRECTDRAWSURFACE7 lpDDSurfaceTargetOverride, DWORD dwFlags ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::Flip() #####" );

    return S_OK;
}

HRESULT MyDirectDrawSurface7::GetAttachedSurface( LPDDSCAPS2 lpDDSCaps2, LPDIRECTDRAWSURFACE7* lplpDDAttachedSurface ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::GetAttachedSurface()" );

    if ( attachedSurfaces.empty() )
        return E_FAIL;

    *lplpDDAttachedSurface = attachedSurfaces[0];
    attachedSurfaces[0]->AddRef();

    return S_OK;
}

HRESULT MyDirectDrawSurface7::GetBltStatus( DWORD dwFlags ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::GetBltStatus()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::GetCaps( LPDDSCAPS2 lpDDSCaps2 ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::GetCaps()" );
    *lpDDSCaps2 = OriginalSurfaceDesc.ddsCaps;

    return S_OK;
}

HRESULT MyDirectDrawSurface7::GetClipper( LPDIRECTDRAWCLIPPER* lplpDDClipper ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::GetClipper()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::GetColorKey( DWORD dwFlags, LPDDCOLORKEY lpDDColorKey ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::GetColorKey()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::GetDC( HDC* lphDC ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::GetDC()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::GetFlipStatus( DWORD dwFlags ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::GetFlipStatus()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::GetOverlayPosition( LPLONG lplX, LPLONG lplY ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::GetOverlayPosition()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::GetPalette( LPDIRECTDRAWPALETTE* lplpDDPalette ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::GetPalette()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::GetPixelFormat( LPDDPIXELFORMAT lpDDPixelFormat ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::GetPixelFormat()" );

    *lpDDPixelFormat = OriginalSurfaceDesc.ddpfPixelFormat;

    return S_OK;
}

HRESULT MyDirectDrawSurface7::GetSurfaceDesc( LPDDSURFACEDESC2 lpDDSurfaceDesc ) {
    *lpDDSurfaceDesc = OriginalSurfaceDesc;
    return S_OK;
}

HRESULT MyDirectDrawSurface7::Initialize( LPDIRECTDRAW lpDD, LPDDSURFACEDESC2 lpDDSurfaceDesc ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::Initialize()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::IsLost() {
    DebugWriteTex( "IDirectDrawSurface7(%p)::IsLost()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::Lock( LPRECT lpDestRect, LPDDSURFACEDESC2 lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::Lock()" );

    LockType = dwFlags;

    *lpDDSurfaceDesc = OriginalSurfaceDesc;

    // This has to be a backbuffer-copy
    if ( (LockType & DDLOCK_READONLY) != 0 && LockType != DDLOCK_READONLY ) // Gothic uses DDLOCK_READONLY + some other flags for getting the framebuffer. DDLOCK_READONLY only is for movie playback. 
    {
        // Assume 32-bit
        byte* data;
        int pixelSize;
        Engine::GraphicsEngine->Present(); // Get the old frame out, since the draw-function still waits for "PresentPending"
        Engine::GraphicsEngine->OnStartWorldRendering(); // Render a frame without menus on the screen first
        Engine::GraphicsEngine->GetBackbufferData( &data, pixelSize );

        lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount = 32;
        lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask = 0x000000FF;
        lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask = 0x0000FF00;
        lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask = 0x00FF0000;
        lpDDSurfaceDesc->ddpfPixelFormat.dwRGBAlphaBitMask = 0x00000000;

        // Gothic transforms this into a 256x256 texture anyways
        lpDDSurfaceDesc->lPitch = 256 * pixelSize;
        lpDDSurfaceDesc->dwWidth = 256;
        lpDDSurfaceDesc->dwHeight = 256;

        /*lpDDSurfaceDesc->lPitch = Engine::GraphicsEngine->GetBackbufferResolution().x * pixelSize;
        lpDDSurfaceDesc->dwWidth = Engine::GraphicsEngine->GetBackbufferResolution().x;
        lpDDSurfaceDesc->dwHeight = Engine::GraphicsEngine->GetBackbufferResolution().y;*/
        lpDDSurfaceDesc->lpSurface = data;

        LockedData = data;

        return S_OK;
    }

    if ( !EngineTexture )
        return S_OK;

    // Check for 16-bit surface. We allocate the texture as 32-bit, so we need to divide the size by two for that
    int redBits = Toolbox::GetNumberOfBits( OriginalSurfaceDesc.ddpfPixelFormat.dwRBitMask );
    int greenBits = Toolbox::GetNumberOfBits( OriginalSurfaceDesc.ddpfPixelFormat.dwGBitMask );
    int blueBits = Toolbox::GetNumberOfBits( OriginalSurfaceDesc.ddpfPixelFormat.dwBBitMask );
    int alphaBits = Toolbox::GetNumberOfBits( OriginalSurfaceDesc.ddpfPixelFormat.dwRGBAlphaBitMask );

    int bpp = redBits + greenBits + blueBits + alphaBits;
    int divisor = 1;

    if ( bpp == 16 )
        divisor = 2;

    if ( bpp == 24 ) {
        // Handle movie frame,
        // don't deallocate the memory after unlock, since only the changing parts in videos will get updated
        if ( !LockedData )
            LockedData = new unsigned char[EngineTexture->GetSizeInBytes( 0 )];

        // First movie frame - reset data
        if ( Engine::GAPI->GetRendererState().RendererInfo.FirstVideoFrame ) {
            memset( LockedData, 0, EngineTexture->GetSizeInBytes( 0 ) );
        }
    } else {
        // Allocate some temporary data
        delete[] LockedData;
        LockedData = new unsigned char[EngineTexture->GetSizeInBytes( 0 ) / divisor];
    }

    lpDDSurfaceDesc->lpSurface = LockedData;
    lpDDSurfaceDesc->lPitch = EngineTexture->GetRowPitchBytes( 0 ) / divisor;

    return S_OK;
}

#pragma warning(push)
#pragma warning(disable: 6386)
HRESULT MyDirectDrawSurface7::Unlock( LPRECT lpRect ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::Unlock()" );

    // This has to be a backbuffer-copy
    if ( (LockType & DDLOCK_READONLY) != 0 && LockType != DDLOCK_READONLY ) {
        // Clean up
        delete[] LockedData;
        LockedData = nullptr;

        return S_OK;
    }

    // Textureslot 7 is filled only on load-time. This is used to get the zCTexture from this Surface.
    if ( Engine::GAPI->GetBoundTexture( 7 ) != nullptr ) {
        // Comming from LoadResourceData
        LoadAdditionalResources( Engine::GAPI->GetBoundTexture( 7 ) );
    }

    // If this is a 16-bit surface, we need to convert it to 32-bit first
    int redBits = Toolbox::GetNumberOfBits( OriginalSurfaceDesc.ddpfPixelFormat.dwRBitMask );
    int greenBits = Toolbox::GetNumberOfBits( OriginalSurfaceDesc.ddpfPixelFormat.dwGBitMask );
    int blueBits = Toolbox::GetNumberOfBits( OriginalSurfaceDesc.ddpfPixelFormat.dwBBitMask );
    int alphaBits = Toolbox::GetNumberOfBits( OriginalSurfaceDesc.ddpfPixelFormat.dwRGBAlphaBitMask );

    int bpp = redBits + greenBits + blueBits + alphaBits;

    if ( bpp == 16 ) {
        // Convert
        unsigned char* dst = new unsigned char[EngineTexture->GetSizeInBytes( 0 )];
        for ( unsigned int i = 0; i < EngineTexture->GetSizeInBytes( 0 ) / 4; i++ ) {
            unsigned char temp0 = LockedData[i * 2 + 0];
            unsigned char temp1 = LockedData[i * 2 + 1];
            unsigned pixel_data = temp1 << 8 | temp0;

            unsigned char blueComponent = (pixel_data & 0x1F);
            unsigned char greenComponent = (pixel_data >> 6) & 0x1F;
            unsigned char redComponent = (pixel_data >> 11) & 0x1F;

            // Extract red, green and blue components from the 16 bits
            dst[4 * i + 0] = (unsigned char)((redComponent / 32.0) * 255.0f);
            dst[4 * i + 1] = (unsigned char)((greenComponent / 32.0) * 255.0f);
            dst[4 * i + 2] = (unsigned char)((blueComponent / 32.0) * 255.0f);
            dst[4 * i + 3] = 255;
        }

        if ( Engine::GAPI->GetMainThreadID() != GetCurrentThreadId() ) {
            EngineTexture->UpdateDataDeferred( dst, 0 );
            EngineTexture->GenerateMipMapsDeferred();
            Engine::GAPI->AddFrameLoadedTexture( this );
        } else {
            EngineTexture->UpdateData( dst, 0 );
            EngineTexture->GenerateMipMaps();
            SetReady( true ); // No need to load other stuff to get this ready
        }

        delete[] dst;
    } else {
        if ( bpp == 24 ) {
            // First movie frame - clear backbuffers
            if ( Engine::GAPI->GetRendererState().RendererInfo.FirstVideoFrame ) {
                Engine::GraphicsEngine->Clear( float4( 0.0f, 0.0f, 0.0f, 0.0f ) );
                Engine::GAPI->GetRendererState().RendererInfo.FirstVideoFrame = 0;
            }

            if ( Engine::GAPI->GetRendererState().RendererInfo.FixBink ) {
                // SSE2 BGRA -> RGBA conversion
                __m128i mask = _mm_setr_epi8( -1, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0 );
                int32_t textureDataSize = static_cast<int32_t>(EngineTexture->GetSizeInBytes( 0 )) - 32;
                int32_t i = 0;
                for ( ; i <= textureDataSize; i += 32 ) {
                    __m128i data0 = _mm_loadu_si128( reinterpret_cast<const __m128i*>(&LockedData[i]) );
                    __m128i data1 = _mm_loadu_si128( reinterpret_cast<const __m128i*>(&LockedData[i + 16]) );
                    __m128i gaComponents0 = _mm_andnot_si128( mask, data0 );
                    __m128i brComponents0 = _mm_and_si128( data0, mask );
                    __m128i gaComponents1 = _mm_andnot_si128( mask, data1 );
                    __m128i brComponents1 = _mm_and_si128( data1, mask );
                    __m128i brSwapped0 = _mm_shufflehi_epi16( _mm_shufflelo_epi16( brComponents0, _MM_SHUFFLE( 2, 3, 0, 1 ) ), _MM_SHUFFLE( 2, 3, 0, 1 ) );
                    __m128i brSwapped1 = _mm_shufflehi_epi16( _mm_shufflelo_epi16( brComponents1, _MM_SHUFFLE( 2, 3, 0, 1 ) ), _MM_SHUFFLE( 2, 3, 0, 1 ) );
                    _mm_storeu_si128( reinterpret_cast<__m128i*>(&LockedData[i]), _mm_or_si128( gaComponents0, brSwapped0 ) );
                    _mm_storeu_si128( reinterpret_cast<__m128i*>(&LockedData[i + 16]), _mm_or_si128( gaComponents1, brSwapped1 ) );
                }
                textureDataSize += 32;
                for ( ; i < textureDataSize; i += 4 ) {
                    unsigned char R = LockedData[i + 0];
                    unsigned char G = LockedData[i + 2];
                    LockedData[i + 0] = G;
                    LockedData[i + 2] = R;
                }
            }

            // This is a movie frame, draw it to the sceen
            EngineTexture->UpdateData( LockedData, 0 );
            EngineTexture->BindToPixelShader( 0 );

            Engine::GAPI->GetRendererState().BlendState.SetDefault();
            Engine::GAPI->GetRendererState().BlendState.SetDirty();

            if ( Engine::GAPI->GetRendererState().RendererInfo.FixBink ) {
                Engine::GraphicsEngine->DrawQuad( INT2( 0, 0 ), Engine::GraphicsEngine->GetResolution() );
            } else {
                INT2 vidRes = Engine::GAPI->GetRendererState().RendererInfo.PlayingMovieResolution;
                if ( vidRes.x == 0 || vidRes.y == 0 )
                    vidRes = Engine::GraphicsEngine->GetResolution();

                const INT2 engineRes = Engine::GraphicsEngine->GetResolution();

                // Compute how much we would have to scale the video on both axis
                float scaleX = engineRes.x / (float)vidRes.x;
                float scaleY = engineRes.y / (float)vidRes.y;

                // select the smaller one
                float scale = std::min( scaleX, scaleY ) * 0.75f;

                // I am honestly not sure how this is correct, but after an hour of fiddeling this works fine. You probably don't want to touch it.
                float tlx = -engineRes.x * scale + (float)engineRes.x;
                float tly = -engineRes.y * scale + (float)engineRes.y;

                float brx = engineRes.x * scale;
                float bry = engineRes.y * scale;

                Engine::GraphicsEngine->DrawQuad( INT2( tlx, tly ), INT2( brx - tlx, bry - tly ) );
            }
        } else {
            // No conversion needed
            if ( Engine::GAPI->GetMainThreadID() != GetCurrentThreadId() ) {
                EngineTexture->UpdateDataDeferred( LockedData, 0 );
                Engine::GAPI->AddFrameLoadedTexture( this );
            } else {
                EngineTexture->UpdateData( LockedData, 0 );
                SetReady( true ); // No need to load other stuff to get this ready
            }
        }
    }

    if ( bpp != 24 ) {
        // Clean up if not a movie frame
        delete[] LockedData;
        LockedData = nullptr;
    }

    return S_OK;
}
#pragma warning(pop)

HRESULT MyDirectDrawSurface7::ReleaseDC( HDC hDC ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::ReleaseDC()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::Restore() {
    DebugWriteTex( "IDirectDrawSurface7(%p)::Restore()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::SetClipper( LPDIRECTDRAWCLIPPER lpDDClipper ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::SetClipper()" );
    hook_infunc


        HWND hWnd;
    lpDDClipper->GetHWnd( &hWnd );
    Engine::GAPI->OnSetWindow( hWnd );

    hook_outfunc

        return S_OK;
}

HRESULT MyDirectDrawSurface7::SetColorKey( DWORD dwFlags, LPDDCOLORKEY lpDDColorKey ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::SetColorKey(%s, %s)" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::SetOverlayPosition( LONG lX, LONG lY ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::SetOverlayPosition()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::SetPalette( LPDIRECTDRAWPALETTE lpDDPalette ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::SetPalette()" );
    return S_OK;
}


HRESULT MyDirectDrawSurface7::UpdateOverlay( LPRECT lpSrcRect, LPDIRECTDRAWSURFACE7 lpDDDestSurface, LPRECT lpDestRect, DWORD dwFlags, LPDDOVERLAYFX lpDDOverlayFx ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::UpdateOverlay()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::UpdateOverlayDisplay( DWORD dwFlags ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::UpdateOverlayDisplay()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::UpdateOverlayZOrder( DWORD dwFlags, LPDIRECTDRAWSURFACE7 lpDDSReference ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::UpdateOverlayZOrder()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::GetDDInterface( LPVOID* lplpDD ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::GetDDInterface()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::PageLock( DWORD dwFlags ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::PageLock()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::PageUnlock( DWORD dwFlags ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::PageUnlock()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::SetSurfaceDesc( LPDDSURFACEDESC2 lpDDSurfaceDesc, DWORD dwFlags ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::SetSurfaceDesc()" );

    OriginalSurfaceDesc = *lpDDSurfaceDesc;

    // Check if this is the rendertarget or something else we dont need
    if ( lpDDSurfaceDesc->dwWidth == 0 ) {
        return S_OK;
    }

    // Create the texture object this is linked with
    Engine::GraphicsEngine->CreateTexture( &EngineTexture );


    int redBits = Toolbox::GetNumberOfBits( lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask );
    int greenBits = Toolbox::GetNumberOfBits( lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask );
    int blueBits = Toolbox::GetNumberOfBits( lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask );
    int alphaBits = Toolbox::GetNumberOfBits( lpDDSurfaceDesc->ddpfPixelFormat.dwRGBAlphaBitMask );

    int bpp = redBits + greenBits + blueBits + alphaBits;

    // Find out format
    D3D11Texture::ETextureFormat format = D3D11Texture::ETextureFormat::TF_R8G8B8A8;
    switch ( bpp ) {
    case 16:
        format = D3D11Texture::ETextureFormat::TF_R8G8B8A8;
        break;

    case 24:
    case 32:
        format = D3D11Texture::ETextureFormat::TF_R8G8B8A8;

    case 0:
    {
        // DDS-Texture
        if ( (lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_FOURCC) == DDPF_FOURCC ) {
            switch ( lpDDSurfaceDesc->ddpfPixelFormat.dwFourCC ) {
            case FOURCC_DXT1:
                format = D3D11Texture::ETextureFormat::TF_DXT1;
                break;

            case FOURCC_DXT2:
            case FOURCC_DXT3:
                format = D3D11Texture::ETextureFormat::TF_DXT3;
                break;

            case FOURCC_DXT4:
            case FOURCC_DXT5:
                format = D3D11Texture::ETextureFormat::TF_DXT5;
                break;
            }
        }
    }
    break;
    }

    // Find out mip-level count
    unsigned int mipMapCount = 1;
    if ( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_MIPMAP ) {
        mipMapCount = lpDDSurfaceDesc->dwMipMapCount;
    }

    // Create the texture
    EngineTexture->Init( INT2( lpDDSurfaceDesc->dwWidth, lpDDSurfaceDesc->dwHeight ), format, mipMapCount, nullptr, "DirectDrawSurface7" );

    return S_OK;
}

HRESULT MyDirectDrawSurface7::SetPrivateData( REFGUID guidTag, LPVOID lpData, DWORD cbSize, DWORD dwFlags ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::SetPrivateData()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::GetPrivateData( REFGUID guidTag, LPVOID lpBuffer, LPDWORD lpcbBufferSize ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::GetPrivateData()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::FreePrivateData( REFGUID guidTag ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::FreePrivateData()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::GetUniquenessValue( LPDWORD lpValue ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::GetUniquenessValue()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::ChangeUniquenessValue() {
    DebugWriteTex( "IDirectDrawSurface7(%p)::ChangeUniquenessValue()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::SetPriority( DWORD dwPriority ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::SetPriority()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::GetPriority( LPDWORD dwPriority ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::GetPriority()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::SetLOD( DWORD dwLOD ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::SetLOD()" );
    return S_OK;
}

HRESULT MyDirectDrawSurface7::GetLOD( LPDWORD dwLOD ) {
    DebugWriteTex( "IDirectDrawSurface7(%p)::GetLOD()" );
    return S_OK;
}

/** Returns the name of this surface */
const std::string& MyDirectDrawSurface7::GetTextureName() {
    return TextureName;
}
