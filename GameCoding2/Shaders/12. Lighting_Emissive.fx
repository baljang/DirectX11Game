#include "00. Global.fx"

float4 MaterialEmissive; 

MeshOutput VS(VertexTextureNormal input)
{
    MeshOutput output;
    output.position = mul(input.position, W);
    output.worldPosition = input.position;
    output.position = mul(output.position, VP);
    output.uv = input.uv; 
    output.normal = mul(input.normal, (float3x3)W); 
    
    return output;     
}

// Emissive
// 외각선 구할 때 사용
// (림라이트)
float4 PS(MeshOutput input) : SV_TARGET
{
    float3 cameraPosition = -V._41_42_43; 
    float3 E = normalize(cameraPosition - input.worldPosition); 
    
    float value = saturate(dot(E, input.normal));   // Specular에서는 R고 E를 dot 해줬지만, 여기선 E와 normal을 해준다.
    float emissive = 1.0f - value;    // 1은 0, 0은 1로 바꿔준다.
    
    // min, max, x
    emissive = smoothstep(0.0f, 1.0f, emissive); // emissive가 0보다 작으면 0, 1보다 크면 1을 반환
    emissive = pow(emissive, 2); 
    float4 color = MaterialEmissive * emissive; 
    
    return color;
}

technique11 T0
{
    PASS_VP(P0, VS, PS)
};
