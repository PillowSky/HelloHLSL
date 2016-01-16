#include "Header.hlsli"

VS_OUTPUT main(float4 Pos : POSITION, float4 Color : COLOR) {
	VS_OUTPUT output = { mul(Pos, ModelViewProj), Color };
	return output;
}