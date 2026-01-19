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
    float4 Pos : POSITION;
    float4 Color : COLOR;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

PS_INPUT VS_MAIN(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    /*
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    */
    output.Pos = mul(input.Pos, WorldViewProjection);
    output.Color = input.Color;
    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS_MAIN(PS_INPUT input) : SV_Target
{
    return input.Color;
}
