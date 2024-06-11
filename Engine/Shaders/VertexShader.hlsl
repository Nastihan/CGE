#include "CommonInclude.hlsli"

VertexShaderOutput main(AppData IN)
{
    VertexShaderOutput OUT;

    OUT.position = mul(ModelViewProjection, float4(IN.position, 1.0f));

    OUT.positionVS = mul(ModelView, float4(IN.position, 1.0f)).xyz;
    OUT.tangentVS = mul((float3x3) ModelView, IN.tangent);
    OUT.binormalVS = mul((float3x3) ModelView, IN.binormal);
    OUT.normalVS = mul((float3x3) ModelView, IN.normal);

    OUT.texCoord = IN.texCoord;

    return OUT;
}