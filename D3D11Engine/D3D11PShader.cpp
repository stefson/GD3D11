#include "pch.h"
#include "D3D11PShader.h"
#include "D3D11GraphicsEngineBase.h"
#include "Engine.h"
#include "GothicAPI.h"
#include "D3D11ConstantBuffer.h"
#include <d3dcompiler.h>
#include "D3D11_Helpers.h"
#include "D3DShaderCompiler.h"

using namespace DirectX;

D3D11PShader::D3D11PShader() {

    // Insert into state-map
    ID = D3D11ObjectIdManager::AddPShader( this );
}

D3D11PShader::~D3D11PShader() {
    // Remove from state map
    D3D11ObjectIdManager::ErasePShader( this );

    for ( unsigned int i = 0; i < ConstantBuffers.size(); i++ ) {
        delete ConstantBuffers[i];
    }
}

/** Loads both shaders at the same time */
XRESULT D3D11PShader::LoadShader( const char* pixelShader, const std::vector<D3D_SHADER_MACRO>& makros ) {
    HRESULT hr;
    D3D11GraphicsEngineBase* engine = (D3D11GraphicsEngineBase*)Engine::GraphicsEngine;

    Microsoft::WRL::ComPtr<ID3DBlob> psBlob;

    if ( Engine::GAPI->GetRendererState().RendererSettings.EnableDebugLog )
        LogInfo() << "Compilling pixel shader: " << pixelShader;
    File = pixelShader;

    // Compile shaders
    if ( FAILED( CShaderCompiler::CompileFromFile( Toolbox::ToWideChar( pixelShader ).c_str(), "PSMain", "ps_5_0", makros, psBlob.GetAddressOf() ) ) ) {
        return XR_FAILED;
    }

    // Create the shader
    LE( engine->GetDevice()->CreatePixelShader( psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, PixelShader.GetAddressOf() ) );

    SetDebugName( PixelShader.Get(), pixelShader );

    return XR_SUCCESS;
}

/** Applys the shaders */
XRESULT D3D11PShader::Apply() {
    D3D11GraphicsEngineBase* engine = (D3D11GraphicsEngineBase*)Engine::GraphicsEngine;

    engine->GetContext()->PSSetShader( PixelShader.Get(), nullptr, 0 );

    return XR_SUCCESS;
}

/** Returns a reference to the constantBuffer vector*/
std::vector<D3D11ConstantBuffer*>& D3D11PShader::GetConstantBuffer() {
    return ConstantBuffers;
}
