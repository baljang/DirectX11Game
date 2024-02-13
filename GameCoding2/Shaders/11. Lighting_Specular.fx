#include "00. Global.fx"

float3 LightDir; 
float4 LightSpecular; // 색상
float4 MaterialSpecular; // 물체가 받아주는 Specular의 양

Texture2D DiffuseMap;

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

// Specular (반사광)
// 한방향으로 완전히 반사되는 빛 (Phong)
float4 PS(MeshOutput input) : SV_TARGET
{
    // float3 R = reflect(LightDir, input.normal); 
    float3 R = LightDir - (2 * input.normal * dot(LightDir, input.normal));
    R = normalize(R);
    
    float3 cameraPosition = -V._41_42_43; // 아니면 View의 역행렬을 한 다음에 _41_42_43
    float3 E = normalize(cameraPosition - input.worldPosition);
    
    float value = saturate(dot(R, E)); // clamp(0~1)
    float specular = pow(value, 10); // 제곱을 안하면 조금 더 영역이 넓게 되고, 좁힐 수록 효과가 커진다. 
    
    float4 color = LightSpecular * MaterialSpecular * specular; 
    
    return color;
}

technique11 T0
{
    PASS_VP(P0, VS, PS)
};
