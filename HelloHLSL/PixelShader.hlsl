#include "Header.hlsli"

float4 main(PS_INPUT input) : SV_Target {
	float4 finalColor = 0;

	finalColor += saturate(dot(LightDir[0], input.Normal) * LightColor[0] * input.Color);
	finalColor += saturate(dot(LightDir[1], input.Normal) * LightColor[1] * input.Color);

	return finalColor;
}