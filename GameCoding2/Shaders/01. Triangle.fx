struct VertexInput
{
    float4 position : POSITION; // POSITION�� ã�Ƽ� ������ �� ���̴�.
};

struct VertexOutput
{
    float4 position : SV_POSITION;  // System Value ��� ����� �̸��̶� ���
};

VertexOutput VS( VertexInput input)
{
    VertexOutput output; 
    output.position = input.position;
    
    return output;     
}

float4 PS(VertexOutput input) : SV_TARGET   // ������ RenderTarget�� ������ �׷��� ���̴�.
{ 
    return float4(1, 0, 0, 1); // �������� �׷��ش�.
}

float4 PS2(VertexOutput input) : SV_TARGET // ������ RenderTarget�� ������ �׷��� ���̴�.
{
    return float4(0, 1, 0, 1); // �ʷ�
}

float4 PS3(VertexOutput input) : SV_TARGET // ������ RenderTarget�� ������ �׷��� ���̴�.
{
    return float4(0, 0, 1, 1); // �Ķ�
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS())); // ������ 5.0, main �Լ��� VS�ٴ� ��
        SetPixelShader(CompileShader(ps_5_0, PS())); // ������ 5.0, main �Լ��� VS�ٴ� ��
    }

    pass P1
    { 
        SetVertexShader(CompileShader(vs_5_0, VS())); // ������ 5.0, main �Լ��� VS�ٴ� ��
        SetPixelShader(CompileShader(ps_5_0, PS2())); // ������ 5.0, main �Լ��� VS�ٴ� ��
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