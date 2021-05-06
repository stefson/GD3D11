#include "pch.h"
#include "D3D11HDShader.h"
#include "D3D11GraphicsEngineBase.h"
#include "Engine.h"
#include "GothicAPI.h"
#include "D3D11ConstantBuffer.h"
#include <d3dcompiler.h>
#include "D3D11_Helpers.h"
#include "D3DShaderCompiler.h"

using namespace DirectX;

// Patch HLSL-Compiler for http://support.microsoft.com/kb/2448404
#if D3DX_VERSION == 0xa2b
#pragma ruledisable 0x0802405f
#endif

D3D11HDShader::D3D11HDShader() {
    ConstantBuffers = std::vector<D3D11ConstantBuffer*>();

    // Insert into state-map
    ID = D3D11ObjectIdManager::AddHDShader( this );
}


D3D11HDShader::~D3D11HDShader() {
    // Remove from state map
    D3D11ObjectIdManager::EraseHDShader( this );

    for ( unsigned int i = 0; i < ConstantBuffers.size(); i++ ) {
        delete ConstantBuffers[i];
    }
}


/** Loads both shaders at the same time */
XRESULT D3D11HDShader::LoadShader( const wchar_t* hullShader, const wchar_t* domainShader ) {
    HRESULT hr;
    D3D11GraphicsEngineBase* engine = (D3D11GraphicsEngineBase*)Engine::GraphicsEngine;

    Microsoft::WRL::ComPtr<ID3DBlob> hsBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> dsBlob;

    if ( Engine::GAPI->GetRendererState().RendererSettings.EnableDebugLog )
        LogInfo() << "Compilling hull shader: " << hullShader;
    File = hullShader;

    // Compile shaders

    if ( FAILED( CShaderCompiler::CompileFromFile( hullShader, "HSMain", "hs_5_0", hsBlob.GetAddressOf() ) ) ) {
        return XR_FAILED;
    }

    if ( FAILED( CShaderCompiler::CompileFromFile( domainShader, "DSMain", "ds_5_0", dsBlob.GetAddressOf() ) ) ) {
        return XR_FAILED;
    }

    // Create the shaders
    LE( engine->GetDevice()->CreateHullShader( hsBlob->GetBufferPointer(), hsBlob->GetBufferSize(), nullptr, HullShader.GetAddressOf() ) );
    LE( engine->GetDevice()->CreateDomainShader( dsBlob->GetBufferPointer(), dsBlob->GetBufferSize(), nullptr, DomainShader.GetAddressOf() ) );

    SetDebugName( HullShader.Get(), hullShader );
    SetDebugName( DomainShader.Get(), domainShader );

    return XR_SUCCESS;
}

/** Applys the shaders */
XRESULT D3D11HDShader::Apply() {
    D3D11GraphicsEngineBase* engine = (D3D11GraphicsEngineBase*)Engine::GraphicsEngine;

    engine->GetContext()->HSSetShader( HullShader.Get(), nullptr, 0 );
    engine->GetContext()->DSSetShader( DomainShader.Get(), nullptr, 0 );

    return XR_SUCCESS;
}

/** Unbinds the currently bound hull/domain shaders */
void D3D11HDShader::Unbind() {
    D3D11GraphicsEngineBase* engine = (D3D11GraphicsEngineBase*)Engine::GraphicsEngine;

    engine->GetContext()->HSSetShader( nullptr, nullptr, 0 );
    engine->GetContext()->DSSetShader( nullptr, nullptr, 0 );
}

/** Returns a reference to the constantBuffer vector*/
std::vector<D3D11ConstantBuffer*>& D3D11HDShader::GetConstantBuffer() {
    return ConstantBuffers;
}
