cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

struct VertexShaderInput
{
	float3 pos : POSITION;
};

struct GeometryShaderInput
{
	float4 pos : SV_POSITION;
};

GeometryShaderInput main(VertexShaderInput input)
{
	GeometryShaderInput output;
	float4 pos = float4(input.pos, 1.0f);
   
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;


	return output;
}
