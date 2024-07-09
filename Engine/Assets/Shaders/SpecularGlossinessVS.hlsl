#include "CommonInclude.hlsli"

struct AppData
{
    float3 position : POSITION;
	float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float2 texCoord : TEXCOORD0;
};

cbuffer PerObject_Model : register(b0)
{
    float4x4 ModelTransform;
};

VertexShaderOutput MainVS(AppData IN)
{
    VertexShaderOutput OUT;

	// I'm using glm matricies on the c++ side which are stored in a column major order.
	// To work with these matricies I need do to post-multiplication with the point (The point will be on the right)
	// Check out this cool blog post on scratch a pixel (https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/geometry/row-major-vs-column-major-vector.html)
	
	// We have to take note that the matricies sent over from the CPU are in column major order.
	// This will also impact the way we concatenate the transformation matricies since (A * B) != (B * A)
	
	float4x4 ViewPojection = mul(ProjectionTransform, ViewTransform);
	float4x4 ModelViewProjection = mul(ViewPojection, ModelTransform);
	float4x4 ModelView = mul(ViewTransform, ModelTransform);
	
    OUT.position = mul(ModelViewProjection, float4(IN.position, 1.0f));
    OUT.positionVS = mul(ModelView, float4(IN.position, 1.0f)).xyz;
    OUT.tangentVS = mul((float3x3) ModelView, IN.tangent);
    OUT.binormalVS = mul((float3x3) ModelView, IN.binormal);
    OUT.normalVS = mul((float3x3) ModelView, IN.normal);
    OUT.texCoord = IN.texCoord;

    return OUT;
}