struct GeometryShaderInput
{
	float4 pos : SV_POSITION;
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
};

cbuffer Thickness: register(b1) 
{
    float4 fThickness;
}


[maxvertexcount(4)]
void main(line PixelShaderInput input[2], inout TriangleStream<PixelShaderInput> triangleStream)
{
    PixelShaderInput output[4];

    float2 dir = normalize(input[1].pos.xy / input[1].pos.w - input[0].pos.xy / input[0].pos.w);
    float2 normal = float2(-dir.y, dir.x);

    float4 offset1 = float4(normal * input[0].pos.w * fThickness / 2.0f, 0, 0);
    float4 offset2 = float4(normal * input[1].pos.w * fThickness / 2.0f, 0, 0);

    float4 p1 = input[0].pos += offset1;
    float4 p2 = input[0].pos -= offset1;
    float4 p3 = input[1].pos += offset2;
    float4 p4 = input[1].pos -= offset2;

    output[0].pos = p1;
    output[1].pos = p2;
    output[2].pos = p3;
    output[3].pos = p4;

    triangleStream.Append(output[0]);
    triangleStream.Append(output[1]);
    triangleStream.Append(output[2]);
    triangleStream.Append(output[3]);

}