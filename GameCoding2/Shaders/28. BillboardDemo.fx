#include "00. Global.fx"
#include "00. Light.fx"
#include "00. Render.fx"

struct VertexInput
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float2 scale : SCALE;
};

struct V_OUT // VertexOut은 global에서 만들어 놨어서 안겹치게 V_OUT으로 했다.
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

V_OUT VS(VertexInput input)
{
    V_OUT output; 
    
    float4 position = mul(input.position, W); 
    
    float3 up = float3(0, 1, 0); 
    //float3 forward = float3(0, 0, 1);  // TODO 카메라 바라보게 
    float3 forward = position.xyz - CameraPosition(); // BillBoard    
    float3 right = normalize(cross(up, forward)); 
    
    // 동일한 정점을 넣어 준 것을 직접 변환을 해준 것이다. 회전행렬 이용하는 공식이랑 같다.
    // 신기하게 위치가 정상적인 삼각형처럼 뜬다.
    position.xyz += (input.uv.x - 0.5f) * right * input.scale.x;
    position.xyz += (1.0f - input.uv.y - 0.5f) * up * input.scale.y;
    position.w = 1.0f;    
    
    output.position = mul(mul(position, V), P); 
    
    output.uv = input.uv; 
    
    return output; 
}

float4 PS(V_OUT input) : SV_Target
{ 
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.uv); 

    //clip(diffuse.a - 0.3f); 
    if (diffuse.a < 0.3f)
        discard; 
    
    return diffuse;
}

technique11 T0
{
    pass P0
    {
        //SetRasterizerState(FillModeWireFrame);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
};