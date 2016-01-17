#include "Header.hlsli"

VS_OUTPUT main(float4 Position : POSITION, float4 Normal : NORMAL, float4 Color : COLOR) {
	VS_OUTPUT output = { mul(Position, ModelViewProjection), mul(Normal, Model), Color };
	return output;
}