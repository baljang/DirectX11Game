matrix World; 
matrix View; 
matrix Projection; 
Texture2D Texture0;
uint Address;

struct VertexInput
{
    float4 position : POSITION; // POSITION을 찾아서 연결해 줄 것이다.
    float2 uv : TEXCOORD;
};

struct VertexOutput
{
    float4 position : SV_POSITION;  // System Value 라고 예약된 이름이라 명시
    float2 uv : TEXCOORD;
};

VertexOutput VS( VertexInput input)
{
    VertexOutput output;
    output.position = mul(input.position, World);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);
    
    output.uv = input.uv; 
    
    return output;     
}

SamplerState Sampler0; 

SamplerState SamplerAddressWrap
{
// U가 넘어 갔을 때, V가 넘어갔을 때 무엇을 할 것이냐
// 대부분은 둘 다 똑같은 경우로 두는 경우가 많다. 
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState SamplerAddressMirror
{
    AddressU = Mirror;
    AddressV = Mirror;
};

SamplerState SamplerAddressClamp
{
// Clamp는 뭔가 자르는 느낌
    AddressU = Clamp;
    AddressV = Clamp;
};

SamplerState SamplerAddressBorder
{
// 2D 때 실습해 본 적이 있다.
    AddressU = Border;
    AddressV = Border;
    BorderColor = float4(0, 1, 0, 1); 
};


float4 PS(VertexOutput input) : SV_TARGET
{
    if (Address == 0)
        return Texture0.Sample(SamplerAddressWrap, input.uv); 
    
    if (Address == 1)
        return Texture0.Sample(SamplerAddressMirror, input.uv);
    
    if (Address == 2)
        return Texture0.Sample(SamplerAddressClamp, input.uv);
    
    if (Address == 3)
        return Texture0.Sample(SamplerAddressBorder, input.uv);
    
        return Texture0.Sample(Sampler0, input.uv);
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS())); // 버전은 5.0, main 함수는 VS다는 뜻
        SetPixelShader(CompileShader(ps_5_0, PS())); 
    }
};
