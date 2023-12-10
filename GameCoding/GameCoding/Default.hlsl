
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

// IA-VS-RS-PS-OM
// 위치와 관련된 변화
VS_OUTPUT VS(VS_INPUT input)
{ 
    // 행렬을 이용해서 막 계산 해야 하지만 지금은 일단 그대로 토스해보자.
    VS_OUTPUT output; 
    output.position = input.position; 
    output.color = input.color; 
    // 여기까지 VS
    // Rasterizer 단계로 넘어가는 건 보간하는 작업. 삼각형 안에 있는 애들만 걸러주는 거라고 보면 된다. 
    // 점점 각각의 색상이 다르다면 중간의 색깔 어떻게 보간할지 성정할 수 있어. 기본적으론 섞어서 보간. 
    // Vertex를 받아서 Raterizer에서 처리해서 Pixel shader로 넘기게 된다. 
    // Vertex Shader가 실행이 되면 알아서 RS가 실행이 되고 PS영역으로 넘어가게 되는데      
    
    return output; 
}

// 모든 픽셀 단위에 실행되는 함수 
// 조명과 관련된 연산을 적어서 색상을 만들어주면 된다. 
// 색상과 관련된 변화
float4 PS(VS_OUTPUT input) : SV_Target
{        
    return input.color; // 빨간색으로 처리했다 가정
}