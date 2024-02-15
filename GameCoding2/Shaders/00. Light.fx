#ifndef _LIGHT_FX_
#define _LIGHT_FX_

#include "00. Global.fx"

////////////
// Struct //
////////////

struct LightDesc
{ 
    float4 ambient; 
    float4 diffuse; 
    float4 specular; 
    float4 emissive; 
    float3 direction; 
    float padding; 
};

struct MaterialDesc
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 emissive;
};

////////////////////
// ConstantBuffer //
////////////////////

cbuffer LightBuffer
{
    LightDesc GlobalLight;     
};

cbuffer MaterialBuffer
{ 
    MaterialDesc Material; 
};

/////////
// SRV //
///////// 

Texture2D DiffuseMap; 
Texture2D SpecularMap; 
Texture2D NormalMap; 

//////////////
// Function //
//////////////

float4 ComputeLight(float3 normal, float2 uv, float3 worldPosition)
{
    float4 ambientColor = 0; 
    float4 diffuseColor = 0; 
    float4 specularColor = 0; 
    float4 emissiveColor = 0;   
    
    // Ambient
    {
        float4 color = GlobalLight.ambient * Material.ambient;
        //return color; 
       ambientColor = DiffuseMap.Sample(LinearSampler, uv) * color;
    }
    
    // Diffuse
    {
        float4 color = DiffuseMap.Sample(LinearSampler, uv);    
        float value = dot(-GlobalLight.direction, normalize(normal));
        
        diffuseColor = color * value * GlobalLight.diffuse * Material.diffuse;
    }
    
    // Specular
    {
        // float3 R = reflect(GlobalLight.direction, normal); 
        float3 R = GlobalLight.direction - (2 * normal * dot(GlobalLight.direction, normal));
        R = normalize(R);
    
        float3 cameraPosition = CameraPosition(); // 아니면 View의 역행렬을 한 다음에 _41_42_43
        float3 E = normalize(cameraPosition - worldPosition);
    
        float value = saturate(dot(R, E)); // clamp(0~1)
        float specular = pow(value, 10); // 제곱을 안하면 조금 더 영역이 넓게 되고, 좁힐 수록 효과가 커진다. 
    
        specularColor = GlobalLight.specular * Material.specular * specular;
    }
    
    // Emissive
    {
        float3 cameraPosition = CameraPosition();
        float3 E = normalize(cameraPosition - worldPosition);
    
        float value = saturate(dot(E, normal)); // Specular에서는 R고 E를 dot 해줬지만, 여기선 E와 normal을 해준다.
        float emissive = 1.0f - value; // 1은 0, 0은 1로 바꿔준다.
    
    // min, max, x
        emissive = smoothstep(0.0f, 1.0f, emissive); // emissive가 0보다 작으면 0, 1보다 크면 1을 반환
        emissive = pow(emissive, 2);
        
        emissiveColor = GlobalLight.emissive * Material.emissive * emissive;
    }
    
    return ambientColor + diffuseColor + specularColor + emissiveColor; 
}

#endif


