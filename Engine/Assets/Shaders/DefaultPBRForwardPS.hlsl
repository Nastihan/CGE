
#include "CommonInclude.hlsli"
#include "SamplersCommon.hlsli"

float4 MainPS( VertexShaderOutput IN ) : SV_TARGET
{
    // Everything is in view space.
    float4 eyePos = { 0, 0, 0, 1 };
    Material mat = Mat;

    float4 diffuse = mat.DiffuseColor;
    if ( mat.HasDiffuseTexture )
    {
        float4 diffuseTex = PerMaterial_DiffuseTexture.Sample( PerScene_LinearRepeatSampler, IN.texCoord );
        if ( any( diffuse.rgb ) )
        {
            diffuse *= diffuseTex;
        }
        else
        {
            diffuse = diffuseTex;
        }
    }

    // By default, use the alpha from the diffuse component.
    float alpha = diffuse.a;
    if ( mat.HasOpacityTexture )
    {
        // If the material has an opacity texture, use that to override the diffuse alpha.
        alpha = PerMaterial_OpacityTexture.Sample( PerScene_LinearRepeatSampler, IN.texCoord ).r;
    }

    float4 ambient = mat.AmbientColor;
    if ( mat.HasAmbientTexture )
    {
        float4 ambientTex = PerMaterial_AmbientTexture.Sample( PerScene_LinearRepeatSampler, IN.texCoord );
        if ( any( ambient.rgb ) )
        {
            ambient *= ambientTex;
        }
        else
        {
            ambient = ambientTex;
        }
    }
    // Combine the global ambient term.
    ambient *= mat.GlobalAmbient;

    float4 emissive = mat.EmissiveColor;
    if ( mat.HasEmissiveTexture )
    {
        float4 emissiveTex = PerMaterial_EmissiveTexture.Sample( PerScene_LinearRepeatSampler, IN.texCoord );
        if ( any( emissive.rgb ) )
        {
            emissive *= emissiveTex;
        }
        else
        {
            emissive = emissiveTex;
        }
    }

    if ( mat.HasSpecularPowerTexture )
    {
        mat.SpecularPower = PerMaterial_SpecularPowerTexture.Sample( PerScene_LinearRepeatSampler, IN.texCoord ).r * mat.SpecularScale;
    }

    float4 N;

    // Normal mapping
    if ( mat.HasNormalTexture )
    {
        // For scense with normal mapping, I don't have to invert the binormal.
        float3x3 TBN = float3x3( normalize( IN.tangentVS ),
                                 normalize( IN.binormalVS ),
                                 normalize( IN.normalVS ) );

        N = DoNormalMapping( TBN, PerMaterial_NormalTexture, PerScene_LinearRepeatSampler, IN.texCoord );
    }
    // Bump mapping
    else if ( mat.HasBumpTexture )
    {
        // For most scenes using bump mapping, I have to invert the binormal.
        float3x3 TBN = float3x3( normalize( IN.tangentVS ),
                                 normalize( -IN.binormalVS ), 
                                 normalize( IN.normalVS ) );

        N = DoBumpMapping( TBN, PerMaterial_BumpTexture, PerScene_LinearRepeatSampler, IN.texCoord, mat.BumpIntensity );
    }
    // Just use the normal from the model.
    else
    {
        N = normalize( float4( IN.normalVS, 0 ) );
    }

    float4 P = float4( IN.positionVS, 1 );

    LightingResult lit = DoLighting( PerScene_Lights, mat, eyePos, P, N );

    diffuse *= float4( lit.Diffuse.rgb, 1.0f ); // Discard the alpha value from the lighting calculations.

    float4 specular = 0;
    if ( mat.SpecularPower > 1.0f ) // If specular power is too low, don't use it.
    {
        specular = mat.SpecularColor;
        if ( mat.HasSpecularTexture )
        {
            float4 specularTex = PerMaterial_SpecularTexture.Sample( PerScene_LinearRepeatSampler, IN.texCoord );
            if ( any( specular.rgb ) )
            {
                specular *= specularTex;
            }
            else
            {
                specular = specularTex;
            }
        }
        specular *= lit.Specular;
    }
	
	// return float4( 0.0, 0.0, 0.0, 0.0 );
    return float4( ( ambient + emissive + diffuse + specular ).rgb, alpha * mat.Opacity );

}