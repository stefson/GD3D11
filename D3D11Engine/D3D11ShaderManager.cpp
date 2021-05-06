#include "pch.h"
#include "D3D11ShaderManager.h"
#include "D3D11Vshader.h"
#include "D3D11PShader.h"
#include "D3D11HDShader.h"
#include "D3D11GShader.h"
#include "D3D11ConstantBuffer.h"
#include "GothicGraphicsState.h"
#include "ConstantBufferStructs.h"
#include "GothicAPI.h"
#include "Engine.h"
#include "Threadpool.h"

const int NUM_MAX_BONES = 96;

D3D11ShaderManager::D3D11ShaderManager() {
    ReloadShadersNextFrame = false;
}

D3D11ShaderManager::~D3D11ShaderManager() {
    DeleteShaders();
}

/** Creates list with ShaderInfos */
XRESULT D3D11ShaderManager::Init() {
    Shaders = std::vector<ShaderInfo>();
    VShaders = std::unordered_map<std::wstring, std::shared_ptr<D3D11VShader>>();
    PShaders = std::unordered_map<std::wstring, std::shared_ptr<D3D11PShader>>();

    Shaders.push_back( ShaderInfo( L"VS_Ex", L"VS_Ex.hlsl", L"v", 1 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerFrame ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerInstance ) );

    Shaders.push_back( ShaderInfo( L"VS_ExCube", L"VS_ExCube.hlsl", L"v", 1 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerFrame ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerInstance ) );

    Shaders.push_back( ShaderInfo( L"VS_PNAEN", L"VS_PNAEN.hlsl", L"v", 1 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerFrame ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerInstance ) );

    Shaders.push_back( ShaderInfo( L"VS_PNAEN_Instanced", L"VS_PNAEN_Instanced.hlsl", L"v", 10 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerFrame ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerInstance ) );

    Shaders.push_back( ShaderInfo( L"VS_Decal", L"VS_Decal.hlsl", L"v", 1 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerFrame ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerInstance ) );

    Shaders.push_back( ShaderInfo( L"VS_ExWater", L"VS_ExWater.hlsl", L"v", 1 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerFrame ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerInstance ) );

    Shaders.push_back( ShaderInfo( L"VS_ParticlePoint", L"VS_ParticlePoint.hlsl", L"v", 11 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerFrame ) );

    Shaders.push_back( ShaderInfo( L"VS_ParticlePointShaded", L"VS_ParticlePointShaded.hlsl", L"v", 11 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerFrame ) );
    Shaders.back().cBufferSizes.push_back( sizeof( ParticlePointShadingConstantBuffer ) );


    Shaders.push_back( ShaderInfo( L"VS_AdvanceRain", L"VS_AdvanceRain.hlsl", L"v", 11 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( AdvanceRainConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"VS_Ocean", L"VS_Ocean.hlsl", L"v", 1 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerFrame ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerInstance ) );

    Shaders.push_back( ShaderInfo( L"VS_ExWS", L"VS_ExWS.hlsl", L"v", 1 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerFrame ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerInstance ) );

    Shaders.push_back( ShaderInfo( L"VS_ExDisplace", L"VS_ExDisplace.hlsl", L"v", 1 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerFrame ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerInstance ) );

    Shaders.push_back( ShaderInfo( L"VS_Obj", L"VS_Obj.hlsl", L"v", 8 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerFrame ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerInstance ) );

    Shaders.push_back( ShaderInfo( L"VS_ExSkeletal", L"VS_ExSkeletal.hlsl", L"v", 3 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerFrame ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerInstanceSkeletal ) );
    Shaders.back().cBufferSizes.push_back( NUM_MAX_BONES * sizeof( DirectX::XMFLOAT4X4 ) );

    Shaders.push_back( ShaderInfo( L"VS_ExSkeletalCube", L"VS_ExSkeletalCube.hlsl", L"v", 3 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerFrame ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerInstanceSkeletal ) );
    Shaders.back().cBufferSizes.push_back( NUM_MAX_BONES * sizeof( DirectX::XMFLOAT4X4 ) );


    Shaders.push_back( ShaderInfo( L"VS_PNAEN_Skeletal", L"VS_PNAEN_Skeletal.hlsl", L"v", 3 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerFrame ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerInstanceSkeletal ) );
    Shaders.back().cBufferSizes.push_back( NUM_MAX_BONES * sizeof( DirectX::XMFLOAT4X4 ) );

    Shaders.push_back( ShaderInfo( L"VS_TransformedEx", L"VS_TransformedEx.hlsl", L"v", 1 ) );
    Shaders.back().cBufferSizes.push_back( 2 * sizeof( float2 ) );

    Shaders.push_back( ShaderInfo( L"VS_ExPointLight", L"VS_ExPointLight.hlsl", L"v", 1 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerFrame ) );
    Shaders.back().cBufferSizes.push_back( sizeof( DS_PointLightConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"VS_XYZRHW_DIF_T1", L"VS_XYZRHW_DIF_T1.hlsl", L"v", 7 ) );
    Shaders.back().cBufferSizes.push_back( 2 * sizeof( float2 ) );

    Shaders.push_back( ShaderInfo( L"VS_ExInstancedObj", L"VS_ExInstancedObj.hlsl", L"v", 10 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerFrame ) );

    Shaders.push_back( ShaderInfo( L"VS_ExRemapInstancedObj", L"VS_ExRemapInstancedObj.hlsl", L"v", 12 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerFrame ) );


    Shaders.push_back( ShaderInfo( L"VS_ExInstanced", L"VS_ExInstanced.hlsl", L"v", 4 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerFrame ) );
    Shaders.back().cBufferSizes.push_back( sizeof( GrassConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"VS_GrassInstanced", L"VS_GrassInstanced.hlsl", L"v", 9 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerFrame ) );

    Shaders.push_back( ShaderInfo( L"VS_Lines", L"VS_Lines.hlsl", L"v", 6 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerFrame ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VS_ExConstantBuffer_PerInstance ) );

    Shaders.push_back( ShaderInfo( L"PS_Lines", L"PS_Lines.hlsl", L"p" ) );
    Shaders.push_back( ShaderInfo( L"PS_LinesSel", L"PS_LinesSel.hlsl", L"p" ) );

    //Shaders.push_back(ShaderInfo("FixedFunctionPipelineEmulationPS", "FixedFunctionPipelineEmulationPS.hlsl", L"p", 1));
    Shaders.push_back( ShaderInfo( L"PS_Simple", L"PS_Simple.hlsl", L"p" ) );
    Shaders.push_back( ShaderInfo( L"PS_SimpleAlphaTest", L"PS_SimpleAlphaTest.hlsl", L"p" ) );

    Shaders.push_back( ShaderInfo( L"PS_Rain", L"PS_Rain.hlsl", L"p" ) );



    Shaders.push_back( ShaderInfo( L"PS_World", L"PS_World.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( GothicGraphicsState ) );
    Shaders.back().cBufferSizes.push_back( sizeof( AtmosphereConstantBuffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( MaterialInfo::Buffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( PerObjectState ) );

    Shaders.push_back( ShaderInfo( L"PS_Ocean", L"PS_Ocean.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( GothicGraphicsState ) );
    Shaders.back().cBufferSizes.push_back( sizeof( AtmosphereConstantBuffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( OceanSettingsConstantBuffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( OceanPerPatchConstantBuffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( RefractionInfoConstantBuffer ) );


    Shaders.push_back( ShaderInfo( L"PS_Water", L"PS_Water.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( GothicGraphicsState ) );
    Shaders.back().cBufferSizes.push_back( sizeof( AtmosphereConstantBuffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( RefractionInfoConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"PS_ParticleDistortion", L"PS_ParticleDistortion.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( RefractionInfoConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"PS_PFX_ApplyParticleDistortion", L"PS_PFX_ApplyParticleDistortion.hlsl", L"p" ) );

    Shaders.push_back( ShaderInfo( L"PS_WorldTriplanar", L"PS_WorldTriplanar.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( GothicGraphicsState ) );
    Shaders.back().cBufferSizes.push_back( sizeof( AtmosphereConstantBuffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( MaterialInfo::Buffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( PerObjectState ) );

    Shaders.push_back( ShaderInfo( L"PS_Grass", L"PS_Grass.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( MaterialInfo::Buffer ) );

    Shaders.push_back( ShaderInfo( L"PS_Sky", L"PS_Sky.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( SkyConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"VS_PFX", L"VS_PFX.hlsl", L"v", 5 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( PFXVS_ConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"PS_PFX_Simple", L"PS_PFX_Simple.hlsl", L"p" ) );


    Shaders.push_back( ShaderInfo( L"PS_PFX_GaussBlur", L"PS_PFX_GaussBlur.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( BlurConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"PS_PFX_Heightfog", L"PS_PFX_Heightfog.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( HeightfogConstantBuffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( AtmosphereConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"PS_PFX_UnderwaterFinal", L"PS_PFX_UnderwaterFinal.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( RefractionInfoConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"PS_Cloud", L"PS_Cloud.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( CloudConstantBuffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( AtmosphereConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"PS_PFX_Copy_NoAlpha", L"PS_PFX_Copy_NoAlpha.hlsl", L"p" ) );
    Shaders.push_back( ShaderInfo( L"PS_PFX_Blend", L"PS_PFX_Blend.hlsl", L"p" ) );
    Shaders.push_back( ShaderInfo( L"PS_PFX_DistanceBlur", L"PS_PFX_DistanceBlur.hlsl", L"p" ) );
    Shaders.push_back( ShaderInfo( L"PS_PFX_LumConvert", L"PS_PFX_LumConvert.hlsl", L"p" ) );
    Shaders.push_back( ShaderInfo( L"PS_PFX_LumAdapt", L"PS_PFX_LumAdapt.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( LumAdaptConstantBuffer ) );

    D3D_SHADER_MACRO m;
    std::vector<D3D_SHADER_MACRO> makros;

    m.Name = "USE_TONEMAP";
    m.Definition = "4";
    makros.push_back( m );

    Shaders.push_back( ShaderInfo( L"PS_PFX_HDR", L"PS_PFX_HDR.hlsl", L"p", makros ) );
    Shaders.back().cBufferSizes.push_back( sizeof( HDRSettingsConstantBuffer ) );
    makros.clear();

    Shaders.push_back( ShaderInfo( L"PS_PFX_GodRayMask", L"PS_PFX_GodRayMask.hlsl", L"p" ) );
    Shaders.push_back( ShaderInfo( L"PS_PFX_GodRayZoom", L"PS_PFX_GodRayZoom.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( GodRayZoomConstantBuffer ) );

    m.Name = "USE_TONEMAP";
    m.Definition = "4";
    makros.push_back( m );

    Shaders.push_back( ShaderInfo( L"PS_PFX_Tonemap", L"PS_PFX_Tonemap.hlsl", L"p", makros ) );
    Shaders.back().cBufferSizes.push_back( sizeof( HDRSettingsConstantBuffer ) );
    makros.clear();

    Shaders.push_back( ShaderInfo( L"PS_SkyPlane", L"PS_SkyPlane.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( ViewportInfoConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"PS_AtmosphereGround", L"PS_AtmosphereGround.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( GothicGraphicsState ) );
    Shaders.back().cBufferSizes.push_back( sizeof( AtmosphereConstantBuffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( MaterialInfo::Buffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( PerObjectState ) );

    Shaders.push_back( ShaderInfo( L"PS_Atmosphere", L"PS_Atmosphere.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( AtmosphereConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"PS_AtmosphereOuter", L"PS_AtmosphereOuter.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( AtmosphereConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"PS_WorldLightmapped", L"PS_WorldLightmapped.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( GothicGraphicsState ) );

    Shaders.push_back( ShaderInfo( L"PS_FixedFunctionPipe", L"PS_FixedFunctionPipe.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( GothicGraphicsState ) );

    Shaders.push_back( ShaderInfo( L"PS_DS_PointLight", L"PS_DS_PointLight.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( DS_PointLightConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"PS_DS_PointLightDynShadow", L"PS_DS_PointLightDynShadow.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( DS_PointLightConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"PS_DS_AtmosphericScattering", L"PS_DS_AtmosphericScattering.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( DS_ScreenQuadConstantBuffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( AtmosphereConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"PS_DS_SimpleSunlight", L"PS_DS_SimpleSunlight.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( DS_ScreenQuadConstantBuffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( AtmosphereConstantBuffer ) );

    // UNUSED
    //Shaders.push_back( ShaderInfo( "DefaultTess", "DefaultTess.hlsl", "hd" ) );
    //Shaders.back().cBufferSizes.push_back( sizeof( DefaultHullShaderConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"OceanTess", L"OceanTess.hlsl", L"hd" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( DefaultHullShaderConstantBuffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( OceanSettingsConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"PNAEN_Tesselation", L"PNAEN_Tesselation.hlsl", L"hd" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( PNAENConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"Water_Tesselation", L"Water_Tesselation.hlsl", L"hd" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( VisualTesselationSettings::Buffer ) );

    Shaders.push_back( ShaderInfo( L"GS_Billboard", L"GS_Billboard.hlsl", L"g" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( ParticleGSInfoConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"GS_Raindrops", L"GS_Raindrops.hlsl", L"g" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( ParticleGSInfoConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"GS_Cubemap", L"GS_Cubemap.hlsl", L"g" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( CubemapGSConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"GS_ParticleStreamOut", L"VS_AdvanceRain.hlsl", L"g", 11 ) );
    Shaders.back().cBufferSizes.push_back( sizeof( ParticleGSInfoConstantBuffer ) );

    m.Name = "NORMALMAPPING";
    m.Definition = "0";
    makros.push_back( m );

    m.Name = "ALPHATEST";
    m.Definition = "0";
    makros.push_back( m );

    Shaders.push_back( ShaderInfo( L"PS_Diffuse", L"PS_Diffuse.hlsl", L"p", makros ) );
    Shaders.back().cBufferSizes.push_back( sizeof( GothicGraphicsState ) );
    Shaders.back().cBufferSizes.push_back( sizeof( AtmosphereConstantBuffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( MaterialInfo::Buffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( PerObjectState ) );

    makros.clear();

    m.Name = "APPLY_RAIN_EFFECTS";
    m.Definition = "1";
    makros.push_back( m );

    m.Name = "SHD_ENABLE";
    m.Definition = "1";
    makros.push_back( m );

    Shaders.push_back( ShaderInfo( L"PS_DS_AtmosphericScattering_Rain", L"PS_DS_AtmosphericScattering.hlsl", L"p", makros ) );
    Shaders.back().cBufferSizes.push_back( sizeof( DS_ScreenQuadConstantBuffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( AtmosphereConstantBuffer ) );

    makros.clear();

    Shaders.push_back( ShaderInfo( L"PS_LinDepth", L"PS_LinDepth.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( GothicGraphicsState ) );
    Shaders.back().cBufferSizes.push_back( sizeof( AtmosphereConstantBuffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( MaterialInfo::Buffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( PerObjectState ) );


    m.Name = "NORMALMAPPING";
    m.Definition = "1";
    makros.push_back( m );

    m.Name = "ALPHATEST";
    m.Definition = "0";
    makros.push_back( m );

    Shaders.push_back( ShaderInfo( L"PS_DiffuseNormalmapped", L"PS_Diffuse.hlsl", L"p", makros ) );
    Shaders.back().cBufferSizes.push_back( sizeof( GothicGraphicsState ) );
    Shaders.back().cBufferSizes.push_back( sizeof( AtmosphereConstantBuffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( MaterialInfo::Buffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( PerObjectState ) );

    makros.clear();
    m.Name = "NORMALMAPPING";
    m.Definition = "1";
    makros.push_back( m );

    m.Name = "ALPHATEST";
    m.Definition = "0";
    makros.push_back( m );

    m.Name = "FXMAP";
    m.Definition = "1";
    makros.push_back( m );

    Shaders.push_back( ShaderInfo( L"PS_DiffuseNormalmappedFxMap", L"PS_Diffuse.hlsl", L"p", makros ) );
    Shaders.back().cBufferSizes.push_back( sizeof( GothicGraphicsState ) );
    Shaders.back().cBufferSizes.push_back( sizeof( AtmosphereConstantBuffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( MaterialInfo::Buffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( PerObjectState ) );

    makros.clear();
    m.Name = "NORMALMAPPING";
    m.Definition = "0";
    makros.push_back( m );

    m.Name = "ALPHATEST";
    m.Definition = "1";
    makros.push_back( m );

    Shaders.push_back( ShaderInfo( L"PS_DiffuseAlphaTest", L"PS_Diffuse.hlsl", L"p", makros ) );
    Shaders.back().cBufferSizes.push_back( sizeof( GothicGraphicsState ) );
    Shaders.back().cBufferSizes.push_back( sizeof( AtmosphereConstantBuffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( MaterialInfo::Buffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( PerObjectState ) );

    makros.clear();
    m.Name = "NORMALMAPPING";
    m.Definition = "1";
    makros.push_back( m );

    m.Name = "ALPHATEST";
    m.Definition = "1";
    makros.push_back( m );

    Shaders.push_back( ShaderInfo( L"PS_DiffuseNormalmappedAlphaTest", L"PS_Diffuse.hlsl", L"p", makros ) );
    Shaders.back().cBufferSizes.push_back( sizeof( GothicGraphicsState ) );
    Shaders.back().cBufferSizes.push_back( sizeof( AtmosphereConstantBuffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( MaterialInfo::Buffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( PerObjectState ) );

    makros.clear();
    m.Name = "NORMALMAPPING";
    m.Definition = "1";
    makros.push_back( m );

    m.Name = "ALPHATEST";
    m.Definition = "1";
    makros.push_back( m );

    m.Name = "FXMAP";
    m.Definition = "1";
    makros.push_back( m );

    Shaders.push_back( ShaderInfo( L"PS_DiffuseNormalmappedAlphaTestFxMap", L"PS_Diffuse.hlsl", L"p", makros ) );
    Shaders.back().cBufferSizes.push_back( sizeof( GothicGraphicsState ) );
    Shaders.back().cBufferSizes.push_back( sizeof( AtmosphereConstantBuffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( MaterialInfo::Buffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( PerObjectState ) );




    makros.clear();
    m.Name = "RENDERMODE";
    m.Definition = "0";
    makros.push_back( m );
    Shaders.push_back( ShaderInfo( L"PS_Preview_White", L"PS_Preview.hlsl", L"p", makros ) );

    makros.clear();
    m.Name = "RENDERMODE";
    m.Definition = "1";
    makros.push_back( m );
    Shaders.push_back( ShaderInfo( L"PS_Preview_Textured", L"PS_Preview.hlsl", L"p", makros ) );

    makros.clear();
    m.Name = "RENDERMODE";
    m.Definition = "2";
    makros.push_back( m );
    Shaders.push_back( ShaderInfo( L"PS_Preview_TexturedLit", L"PS_Preview.hlsl", L"p", makros ) );

    makros.clear();

    Shaders.push_back( ShaderInfo( L"PS_PFX_Sharpen", L"PS_PFX_Sharpen.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( GammaCorrectConstantBuffer ) );

    Shaders.push_back( ShaderInfo( L"PS_PFX_GammaCorrectInv", L"PS_PFX_GammaCorrectInv.hlsl", L"p" ) );
    Shaders.back().cBufferSizes.push_back( sizeof( GammaCorrectConstantBuffer ) );


    // --- LPP
    makros.clear();
    m.Name = "NORMALMAPPING";
    m.Definition = "1";
    makros.push_back( m );

    m.Name = "ALPHATEST";
    m.Definition = "1";
    makros.push_back( m );

    Shaders.push_back( ShaderInfo( L"PS_LPPNormalmappedAlphaTest", L"PS_LPP.hlsl", L"p", makros ) );
    Shaders.back().cBufferSizes.push_back( sizeof( GothicGraphicsState ) );
    Shaders.back().cBufferSizes.push_back( sizeof( AtmosphereConstantBuffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( MaterialInfo::Buffer ) );
    Shaders.back().cBufferSizes.push_back( sizeof( PerObjectState ) );




    return XR_SUCCESS;
}

XRESULT D3D11ShaderManager::CompileShader( const ShaderInfo& si ) {
    const auto SHADERS_PATH = L"system\\GD3D11\\shaders\\";
    //Check if shader src-file exists
    std::wstring fileName = Engine::GAPI->GetStartDirectoryW() + L"\\" + SHADERS_PATH + si.fileName;
    if ( FILE* f = _wfopen( fileName.c_str(), L"r" ) ) {
        //Check shader's type
        if ( si.type == L"v" ) {
            // See if this is a reload
            D3D11VShader* vs = new D3D11VShader();
            if ( IsVShaderKnown( si.name ) ) {
                if ( Engine::GAPI->GetRendererState().RendererSettings.EnableDebugLog )
                    LogInfo() << "Reloading shader: " << si.name;

                if ( XR_SUCCESS != vs->LoadShader( (SHADERS_PATH + si.fileName).c_str(), si.layout, si.shaderMakros ) ) {
                    LogError() << "Failed to reload shader: " << si.fileName;

                    delete vs;
                } else {
                    // Compilation succeeded, switch the shader

                    for ( unsigned int j = 0; j < si.cBufferSizes.size(); j++ ) {
                        vs->GetConstantBuffer().push_back( new D3D11ConstantBuffer( si.cBufferSizes[j], nullptr ) );
                    }
                    UpdateVShader( si.name, vs );
                }
            } else {
                if ( Engine::GAPI->GetRendererState().RendererSettings.EnableDebugLog )
                    LogInfo() << "Reloading shader: " << si.name;

                XLE( vs->LoadShader( (SHADERS_PATH + si.fileName).c_str(), si.layout, si.shaderMakros ) );
                for ( unsigned int j = 0; j < si.cBufferSizes.size(); j++ ) {
                    vs->GetConstantBuffer().push_back( new D3D11ConstantBuffer( si.cBufferSizes[j], nullptr ) );
                }
                UpdateVShader( si.name, vs );
            }
        } else if ( si.type == L"p" ) {
            // See if this is a reload
            D3D11PShader* ps = new D3D11PShader();
            if ( IsPShaderKnown( si.name ) ) {
                if ( XR_SUCCESS != ps->LoadShader( (SHADERS_PATH + si.fileName).c_str(), si.shaderMakros ) ) {
                    LogError() << "Failed to reload shader: " << si.fileName;

                    delete ps;
                } else {
                    // Compilation succeeded, switch the shader

                    for ( unsigned int j = 0; j < si.cBufferSizes.size(); j++ ) {
                        ps->GetConstantBuffer().push_back( new D3D11ConstantBuffer( si.cBufferSizes[j], nullptr ) );
                    }
                    UpdatePShader( si.name, ps );
                }
            } else {
                XLE( ps->LoadShader( (SHADERS_PATH + si.fileName).c_str(), si.shaderMakros ) );
                for ( unsigned int j = 0; j < si.cBufferSizes.size(); j++ ) {
                    ps->GetConstantBuffer().push_back( new D3D11ConstantBuffer( si.cBufferSizes[j], nullptr ) );
                }
                UpdatePShader( si.name, ps );
            }
        } else if ( si.type == L"g" ) {
            // See if this is a reload
            D3D11GShader* gs = new D3D11GShader();
            if ( IsGShaderKnown( si.name ) ) {
                if ( XR_SUCCESS != gs->LoadShader( (SHADERS_PATH + si.fileName).c_str(), si.shaderMakros, si.layout != 0, si.layout ) ) {
                    LogError() << "Failed to reload shader: " << si.fileName;

                    delete gs;
                } else {
                    // Compilation succeeded, switch the shader
                    for ( unsigned int j = 0; j < si.cBufferSizes.size(); j++ ) {
                        gs->GetConstantBuffer().push_back( new D3D11ConstantBuffer( si.cBufferSizes[j], nullptr ) );
                    }
                    UpdateGShader( si.name, gs );
                }
            } else {
                XLE( gs->LoadShader( (SHADERS_PATH + si.fileName).c_str(), si.shaderMakros, si.layout != 0, si.layout ) );
                for ( unsigned int j = 0; j < si.cBufferSizes.size(); j++ ) {
                    gs->GetConstantBuffer().push_back( new D3D11ConstantBuffer( si.cBufferSizes[j], nullptr ) );
                }
                UpdateGShader( si.name, gs );
            }
        }

        fclose( f );
    }

    // Hull/Domain shaders are handled differently, they check inside for missing file
    if ( si.type == std::wstring( L"hd" ) ) {
        // See if this is a reload
        D3D11HDShader* hds = new D3D11HDShader();
        if ( IsHDShaderKnown( si.name ) ) {
            if ( XR_SUCCESS != hds->LoadShader( (SHADERS_PATH + si.fileName).c_str(),
                (SHADERS_PATH + si.fileName).c_str() ) ) {
                LogError() << "Failed to reload shader: " << si.fileName;

                delete hds;
            } else {
                // Compilation succeeded, switch the shader
                for ( unsigned int j = 0; j < si.cBufferSizes.size(); j++ ) {
                    hds->GetConstantBuffer().push_back( new D3D11ConstantBuffer( si.cBufferSizes[j], nullptr ) );
                }
                UpdateHDShader( si.name, hds );
            }
        } else {
            XLE( hds->LoadShader( (SHADERS_PATH + si.fileName).c_str(),
                (SHADERS_PATH + si.fileName).c_str() ) );
            for ( unsigned int j = 0; j < si.cBufferSizes.size(); j++ ) {
                hds->GetConstantBuffer().push_back( new D3D11ConstantBuffer( si.cBufferSizes[j], nullptr ) );
            }
            UpdateHDShader( si.name, hds );
        }
    }
    return XR_SUCCESS;
}

/** Loads/Compiles Shaderes from list */
XRESULT D3D11ShaderManager::LoadShaders() {
    size_t numThreads = std::thread::hardware_concurrency();
    if ( numThreads > 1 ) {
        numThreads = numThreads - 1;
    }
#if defined(DEBUG) || defined(_DEBUG) || !defined(PUBLIC_RELEASE)
    numThreads = 1;
#endif
    auto compilationTP = std::make_unique<ThreadPool>( numThreads );
    LogInfo() << "Compiling/Reloading shaders with " << compilationTP->getNumThreads() << " threads";
    for ( const ShaderInfo& si : Shaders ) {
        compilationTP->enqueue( [this, si]() { CompileShader( si ); } );
    }

    // Join all threads (call Threadpool destructor)
    compilationTP.reset();

    return XR_SUCCESS;
}

/** Deletes all shaders and loads them again */
XRESULT D3D11ShaderManager::ReloadShaders() {
    ReloadShadersNextFrame = true;

    return XR_SUCCESS;
}

/** Called on frame start */
XRESULT D3D11ShaderManager::OnFrameStart() {
    if ( ReloadShadersNextFrame ) {
        LoadShaders();
        ReloadShadersNextFrame = false;
    }

    return XR_SUCCESS;
}

/** Deletes all shaders */
XRESULT D3D11ShaderManager::DeleteShaders() {
    for ( auto vIter = VShaders.begin(); vIter != VShaders.end(); vIter++ ) {
        vIter->second.reset();
    }
    VShaders.clear();

    for ( auto pIter = PShaders.begin(); pIter != PShaders.end(); pIter++ ) {
        pIter->second.reset();
    }
    PShaders.clear();

    for ( auto hdIter = HDShaders.begin(); hdIter != HDShaders.end(); hdIter++ ) {
        hdIter->second.reset();
    }
    HDShaders.clear();

    return XR_SUCCESS;
}

ShaderInfo D3D11ShaderManager::GetShaderInfo( const std::wstring& shader, bool& ok ) {
    for ( size_t i = 0; i < Shaders.size(); i++ ) {
        if ( Shaders[i].name == shader ) {
            ok = true;
            return Shaders[i];
        }
    }
    ok = false;
    return ShaderInfo( L"", L"", L"" );
}
void D3D11ShaderManager::UpdateShaderInfo( ShaderInfo& shader ) {
    for ( size_t i = 0; i < Shaders.size(); i++ ) {
        if ( Shaders[i].name == shader.name ) {
            Shaders[i] = shader;
            CompileShader( shader );
            return;
        }
    }
    Shaders.push_back( shader );
    CompileShader( shader );
}

/** Return a specific shader */
std::shared_ptr<D3D11VShader> D3D11ShaderManager::GetVShader( const std::wstring& shader ) {
    return VShaders[shader];
}
std::shared_ptr<D3D11PShader> D3D11ShaderManager::GetPShader( const std::wstring& shader ) {
    return PShaders[shader];
}
std::shared_ptr<D3D11HDShader> D3D11ShaderManager::GetHDShader( const std::wstring& shader ) {
    return HDShaders[shader];
}
std::shared_ptr<D3D11GShader> D3D11ShaderManager::GetGShader( const std::wstring& shader ) {
    return GShaders[shader];
}
