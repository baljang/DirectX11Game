﻿struct VertexInput
{
    float4 position : POSITION; // POSITION을 찾아서 연결해 줄 것이다.
    float4 color : COLOR;
};

struct VertexOutput
{
    float4 position : SV_POSITION;  // System Value 라고 예약된 이름이라 명시
    float4 color : COLOR; 
};

VertexOutput VS( VertexInput input)
{
    VertexOutput output; 
    output.position = input.position;
    output.color = input.color; 
    
    return output;     
}

float4 PS(VertexOutput input) : SV_TARGET   // 지정한 RenderTarget에 색상을 그려줄 것이다.
{ 
    return input.color; 
}

RasterizerState FillModeWireFrame
{
    FillMode = Wireframe;
};

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS())); // 버전은 5.0, main 함수는 VS다는 뜻
        SetPixelShader(CompileShader(ps_5_0, PS())); 
    }

    pass P1
    { 
        SetRasterizerState(FillModeWireFrame); 
        SetVertexShader(CompileShader(vs_5_0, VS())); 
        SetPixelShader(CompileShader(ps_5_0, PS())); 
    }
};
