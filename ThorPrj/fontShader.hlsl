cbuffer PerFrameBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer PixelBuffer
{
    float4 pixelColor;
};

Texture2D shaderTexture;
SamplerState SampleType;

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_Position;
    float2 tex : TEXCOORD0;
};


PixelInputType FontVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    input.position.w = 1.0f;
    
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.tex = input.tex;
    
    return output;
}

float4 FontPixelShader(PixelInputType input) : SV_TARGET
{
    float4 color;
    color = shaderTexture.Sample(SampleType, input.tex);
    
    if (color.r == 0.0f)
    {
        color.a = 0.0f;
    }
    else
    {
        color.rgb = pixelColor.rgb;
        color.a = 1.0f;
    }
    return color;
}