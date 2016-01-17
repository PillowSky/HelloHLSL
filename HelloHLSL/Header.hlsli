//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer ConstantBufferPerFrame : register(b0) {
	matrix ModelViewProjection;
	matrix Model;
	float4 LightDir[2];
};

cbuffer ConstantBufferPersist : register(b1) {
	float4 LightColor[2];
};

//--------------------------------------------------------------------------------------
// Input and Output Structures
//--------------------------------------------------------------------------------------
struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float4 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
	float4 Color : COLOR;
};

typedef VS_OUTPUT PS_INPUT;