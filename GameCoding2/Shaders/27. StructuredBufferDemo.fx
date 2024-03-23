
struct InputDesc
{
    matrix input; 
};

struct OutputDesc
{
    matrix result; 
};

StructuredBuffer<InputDesc> Input; // vector 처럼 같은 타임의 데이터가 여러개 있는 거 
RWStructuredBuffer<OutputDesc> Output;

[numthreads(500, 1, 1)] 
void CS(uint id : SV_GroupIndex)
{    
    matrix result = Input[id].input*2; // 가공을 2를 곱하는 걸로
    
    Output[id].result = result; 
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
