Texture2D tex : register(t0);
SamplerState smp : register(s0);

cbuffer VertexBuffer
{
    matrix wp;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
    float2 uv : TEXCOORD0;
};

VS_OUTPUT VS(float2 pos : POSITION, float4 col : COLOR, float2 uv : TEXCOORD)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.pos = mul(wp, float4(pos, 0.f, 1.f));
    output.col = col;
    output.uv = uv;

    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 col = input.col * tex.Sample(smp, input.uv);
    return col;
}