struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

Texture2D shaderTexture;
SamplerState SampleType;

float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
    
    float4 textureColor;
    
    textureColor = shaderTexture.Sample(SampleType, input.tex);
    
    return textureColor;
}