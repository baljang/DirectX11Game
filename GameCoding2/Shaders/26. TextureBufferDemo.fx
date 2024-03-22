
Texture2DArray<float4> Input; 
RWTexture2DArray<float4> Output; 

[numthreads(32, 32, 1)] // 2차원 정보
void CS(uint3 id : SV_DispatchThreadID)
{
    float4 color = Input.Load(int4(id, 0)); 
    
    // Output[id] = color;  // 원래 색깔 유지
    Output[id] = 1.0f - color; // 색 반전
    //Output[id] = 1.0f - (color.r + color.g + color.b) / 3.0f; // 회색
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(NULL); 
        SetPixelShader(NULL); 
        SetComputeShader(CompileShader(cs_5_0, CS())); 
    }
};
