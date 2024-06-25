#include "CommonInclude.hlsli"

struct AppData
{
    float3 position : POSITION;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
};

cbuffer PerObject_Model : register(b0)
{
    float4x4 ModelTransform;
}

cbuffer PerView_CameraMatrix : register(b1)
{
	float4x4 ViewTransform;
	float4x4 InverseViewTransform;
	float4x4 ProjectionTransform;
	float4x4 InverseProjectionTransform;
}

VertexShaderOutput MainVS(AppData IN)
{
    VertexShaderOutput OUT;

	// I'm using glm matricies on the c++ side which are stored in a column major order.
	// To work with these matricies I need do to post-multiplication with the point (The point will be on the right)
	// Check out this cool blog post on scratch a pixel (https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/geometry/row-major-vs-column-major-vector.html)
	
	float4x4 ModelTransformT = transpose(ModelTransform);
	float4x4 ViewTransformT = transpose(ViewTransform);
	float4x4 InverseViewTransformT = transpose(InverseViewTransform);
	float4x4 ProjectionTransformT = transpose(ProjectionTransform);
	float4x4 InverseProjectionTransformT = transpose(InverseProjectionTransform);
	
	float4x4 ModelView = mul(ModelTransformT, ViewTransformT);
	float4x4 ModelViewProjection = mul(ModelView, ProjectionTransformT);
	
    //OUT.position = mul(ModelViewProjection, float4(IN.position, 1.0f));
    //OUT.positionVS = mul(ModelView, float4(IN.position, 1.0f)).xyz;
    //OUT.tangentVS = mul((float3x3) ModelView, IN.tangent);
    //OUT.binormalVS = mul((float3x3) ModelView, IN.binormal);
    //OUT.normalVS = mul((float3x3) ModelView, IN.normal);
	
	OUT.position = mul(float4(IN.position, 1.0f), ModelViewProjection);
    OUT.positionVS = mul(float4(IN.position, 1.0f), ModelView).xyz;
    OUT.tangentVS = mul(IN.tangent, (float3x3) ModelView);
    OUT.binormalVS = mul(IN.binormal, (float3x3) ModelView);
    OUT.normalVS = mul(IN.normal, (float3x3) ModelView);

    OUT.texCoord = IN.texCoord;

    return OUT;
}