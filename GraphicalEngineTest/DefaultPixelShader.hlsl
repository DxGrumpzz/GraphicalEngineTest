Texture2D _texture : register(t0);
SamplerState _sampler : register(s0);

float4 main(float4 position : SV_POSITION, float2 textureCoordinate : TEXCOORD0) : SV_TARGET
{ 
	return _texture.Sample(_sampler,textureCoordinate);
}