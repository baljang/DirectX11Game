struct VertexInput
{
    float4 position : POSITION; // POSITION을 찾아서 연결해 줄 것이다.
};

struct VertexOutput
{
    float4 position : SV_POSITION;  // System Value 라고 예약된 이름이라 명시
};

VertexOutput VS( VertexInput input)
{
    VertexOutput output; 
    output.position = input.position;
    
    return output;     
}

float4 PS(VertexOutput input) : SV_TARGET   // 지정한 RenderTarget에 색상을 그려줄 것이다.
{ 
    return float4(1, 0, 0, 1); // 빨강색을 그려준다.
}

float4 PS2(VertexOutput input) : SV_TARGET // 지정한 RenderTarget에 색상을 그려줄 것이다.
{
    return float4(0, 1, 0, 1); // 초록
}

float4 PS3(VertexOutput input) : SV_TARGET // 지정한 RenderTarget에 색상을 그려줄 것이다.
{
    return float4(0, 0, 1, 1); // 파랑
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS())); // 버전은 5.0, main 함수는 VS다는 뜻
        SetPixelShader(CompileShader(ps_5_0, PS())); // 버전은 5.0, main 함수는 VS다는 뜻
    }

    pass P1
    { 
        SetVertexShader(CompileShader(vs_5_0, VS())); // 버전은 5.0, main 함수는 VS다는 뜻
        SetPixelShader(CompileShader(ps_5_0, PS2())); // 버전은 5.0, main 함수는 VS다는 뜻
    }
};

technique11 T1
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));

        SetPixelShader(CompileShader(ps_5_0, PS3()));
    }
};