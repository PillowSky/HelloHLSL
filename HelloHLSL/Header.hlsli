//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0) {
	matrix ModelViewProj;
};

//--------------------------------------------------------------------------------------
// Input and Output Structures
//--------------------------------------------------------------------------------------
struct VS_OUTPUT {
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};

typedef VS_OUTPUT PS_INPUT;