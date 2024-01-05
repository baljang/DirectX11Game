
struct VS_INPUT
{ 
    float4 position : POSITION; 
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION; 
    float4 color : COLOR;
};

cbuffer TransformData : register(b0) // 상수 버서 TransfromData를 받아 줄 건데, 버퍼의 약자인 b0를 받아 주도록 할거야. 
{ 
    row_major matrix matWorld;
    row_major matrix matView;
    row_major matrix matProjection;
}


// IA-VS-RS-PS-OM
// 위치와 관련된 변화
VS_OUTPUT VS(VS_INPUT input)
{ 
    VS_OUTPUT output; 
    
    // WVP
    float4 position = mul(input.position, matWorld); // W
    position = mul(position, matView); // V
    position = mul(position, matProjection); // P 
    
//  output.position = input.position + offset; 
    output.position = position; 
    output.color = input.color;  
    
    return output; 
}

Texture2D texture0 : register(t0); 
// t가 Texture의 약자이고, t0 register에다가 teture0이라는 아이를 등록을 할 것이다 라고 예꼬를 하는 거. 

SamplerState sampler0 : register(s0); 
// 이렇게 texture와 sampler가 정해 지면 color를 정할 때 바로 input.을 하는게 아니라 textured0에다가 Sample이란 함수 이용 
// smapler0는 어떻게 갖고 올지에 대한 규약

// 모든 픽셀 단위에 실행되는 함수 
// 조명과 관련된 연산을 적어서 색상을 만들어주면 된다. 
// 색상과 관련된 변화
float4 PS(VS_OUTPUT input) : SV_Target
{      
    return input.color;
}