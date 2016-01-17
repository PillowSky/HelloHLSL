#include "Header.hlsli"

float4 main(PS_INPUT input) : SV_Target {
	float4 finalColor =
		saturate(dot(LightDir[0], input.Normal) * LightColor[0]) * txDiffuse.Sample(samLinear, input.TexCoord) * input.Color +
		saturate(dot(LightDir[1], input.Normal) * LightColor[1]) * txDiffuse.Sample(samLinear, input.TexCoord) * input.Color;

	return finalColor;
}