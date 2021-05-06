#pragma once
#include "pch.h"
#include "Engine.h"
#include "GothicAPI.h"
#include "D3D11GraphicsEngineBase.h"
#include <d3dcompiler.h>
#include <filesystem>
#include <fstream>

/// <summary>
/// Provides shader locations in a way that is compatible with multi-threading by not using the CWD
/// </summary>
class CShaderInclude : public ID3DInclude
{
public:
    // shaderDir: Directory of current shader file, used by #include "FILE"
    // systemDir: Default shader includes directory, used by #include <FILE>
    CShaderInclude( const wchar_t* shaderDir, const wchar_t* systemDir ) :
        m_ShaderDir( shaderDir ),
        m_SystemDir( systemDir )
    {
    }
    CShaderInclude( const std::wstring& shaderDir, const std::wstring& systemDir ) :
        m_ShaderDir( shaderDir ),
        m_SystemDir( systemDir )
    {
    }

    HRESULT __stdcall Open(
        D3D_INCLUDE_TYPE IncludeType,
        LPCSTR pFileName,
        LPCVOID pParentData,
        LPCVOID* ppData,
        UINT* pBytes ) {
        /*
    If pFileName is absolute: finalPath = pFileName.
    If pFileName is relative: finalPath = dir + "\\" + pFileName
    */
        std::wstring finalPath;
        switch ( IncludeType ) {
        case D3D_INCLUDE_LOCAL: // #include "FILE"
            finalPath = std::filesystem::path( m_ShaderDir ) / pFileName;
            break;
        case D3D_INCLUDE_SYSTEM: // #include <FILE>
            finalPath = std::filesystem::path( m_SystemDir ) / pFileName;
            break;
        default:
            assert( 0 );
        }

        std::error_code ec;
        uint32_t fileSize = std::filesystem::file_size( finalPath, ec );
        if ( ec.value() != 0 ) {
            return E_FAIL;
        }

        std::ifstream fileStream( finalPath, std::ios::binary | std::ios::in );
        m_data = std::string( (std::istreambuf_iterator<char>( fileStream )), std::istreambuf_iterator<char>() );

        *ppData = m_data.c_str();
        *pBytes = fileSize;

        return S_OK;
    }

    HRESULT __stdcall Close( LPCVOID pData ) {
        m_data.clear();
        return S_OK;
    }

private:
    std::wstring m_ShaderDir;
    std::wstring m_SystemDir;
    std::string m_data;
};

class CShaderCompiler {
public:
    static HRESULT CShaderCompiler::CompileFromFile( const wchar_t* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, const std::vector<D3D_SHADER_MACRO>& makros, ID3DBlob** ppBlobOut ) {
        HRESULT hr = S_OK;

        DWORD dwShaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
        // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
        // Setting this flag improves the shader debugging experience, but still allows 
        // the shaders to be optimized and to run exactly the way they will run in 
        // the release configuration of this program.
        //dwShaderFlags |= D3DCOMPILE_DEBUG;
#else
        dwShaderFlags |= D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
        CShaderInclude include(
            // Directory where Shader lives
            (std::filesystem::path( Engine::GAPI->GetStartDirectoryW() ) / szFileName).parent_path(),
            // Directory where we assume lie most of our shaders
            Engine::GAPI->GetStartDirectoryW() + L"\\system\\GD3D11\\shaders" );

        Microsoft::WRL::ComPtr<ID3DBlob> pErrorBlob;

        std::vector<D3D_SHADER_MACRO> m;
        // Construct makros
        D3D11GraphicsEngineBase::ConstructShaderMakroList( m );
        if ( !makros.empty() ) {
            // Push these to the front
            m.insert( m.begin(), makros.begin(), makros.end() );
        }
        hr = D3DCompileFromFile( szFileName, &m[0], &include, szEntryPoint, szShaderModel, dwShaderFlags, 0, ppBlobOut, pErrorBlob.GetAddressOf() );
        if ( FAILED( hr ) ) {
            LogInfo() << "Shader compilation failed";
            if ( pErrorBlob.Get() ) {
                LogErrorBox() << (char*)pErrorBlob->GetBufferPointer() << "\n\n (You can ignore the next error from Gothic about too small video memory!)";
            }
            return hr;
        }
        return S_OK;
    }

    static HRESULT CShaderCompiler::CompileFromFile( const wchar_t* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut ) {
        static std::vector<D3D_SHADER_MACRO> empty;
        return CompileFromFile( szFileName, szEntryPoint, szShaderModel, empty, ppBlobOut );
    }
};
