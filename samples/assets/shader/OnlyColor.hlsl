cbuffer CAMERA_DATA : register(b0)
{
    matrix View;
    matrix InvView;
    matrix Projection;
    matrix ViewProjection;
}

cbuffer OBJECT_DATA : register(b1)
{
    matrix World;
    matrix WorldViewProjection;
}

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 Pos : POSITION;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

PS_INPUT VS_MAIN(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul(float4(input.Pos, 1.0f), WorldViewProjection);
    output.Color = float4(1.0f, 0.0f, 0.0f, 1.0f); // Solid red color
    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS_MAIN(PS_INPUT input) : SV_Target
{
    return input.Color;
}
