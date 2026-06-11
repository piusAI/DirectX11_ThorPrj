cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    // matrix scaleMatrix; //01T Cbuffer Scaling Test
};

Texture2D shaderTexture;
SamplerState SampleType;


struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 instancePosition : TEXCOORD1;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

PixelInputType TextureVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    
    
    // float4 pos = float4(input.position, 1.0f);
    input.position.w = 1.0f; //동차 좌표 추가
    //float4 pos = input.position;

    output.position = mul(input.position, worldMatrix);
    output.position.xyz += input.instancePosition;
    // output.position = mul(input.position, scaleMatrix); //01T Cbuffer Scaling Test
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.tex = input.tex;

    
    return output;

}