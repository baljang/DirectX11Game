
struct VS_INPUT
{ 
    float4 position : POSITION; 
    // float4 color : COLOR;
    float2 uv : TEXCOORD; 
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION; 
    // float4 color : COLOR;
    float2 uv : TEXCOORD;
};

cbuffer TransformData : register(b0) // 상수 버서 TransfromData를 받아 줄 건데, 버퍼의 약자인 b0를 받아 주도록 할거야. 
{ 
    float4 offset; 
    // 이걸 CPU가 세팅을 해서 TranformData를 넘겨주게 된다.
}


// IA-VS-RS-PS-OM
// 위치와 관련된 변화
VS_OUTPUT VS(VS_INPUT input)
{ 
    // 행렬을 이용해서 막 계산 해야 하지만 지금은 일단 그대로 토스해보자.
    VS_OUTPUT output; 
    output.position = input.position + offset; 
    output.uv = input.uv; 
    // 여기까지 VS
    // Rasterizer 단계로 넘어가는 건 보간하는 작업. 삼각형 안에 있는 애들만 걸러주는 거라고 보면 된다. 
    // 점점 각각의 색상이 다르다면 중간의 색깔 어떻게 보간할지 성정할 수 있어. 기본적으론 섞어서 보간. 
    // Vertex를 받아서 Raterizer에서 처리해서 Pixel shader로 넘기게 된다. 
    // Vertex Shader가 실행이 되면 알아서 RS가 실행이 되고 PS영역으로 넘어가게 되는데      
    
    return output; 
}

Texture2D texture0 : register(t0); 
// t가 Texture의 약자이고, t0 register에다가 teture0이라는 아이를 등록을 할 것이다 라고 예꼬를 하는 거. 
Texture2D texture1 : register(t1);

SamplerState sampler0 : register(s0); 
// 이렇게 texture와 sampler가 정해 지면 color를 정할 때 바로 input.을 하는게 아니라 textured0에다가 Sample이란 함수 이용 
// smapler0는 어떻게 갖고 올지에 대한 규약

// 모든 픽셀 단위에 실행되는 함수 
// 조명과 관련된 연산을 적어서 색상을 만들어주면 된다. 
// 색상과 관련된 변화
float4 PS(VS_OUTPUT input) : SV_Target
{      
    float4 color = texture0.Sample(sampler0, input.uv);// texture0에 uv좌표를 이용하여 해당하는 색상을 빼온다.
    
    return color; 
}