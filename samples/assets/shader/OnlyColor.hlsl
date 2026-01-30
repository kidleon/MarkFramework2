cbuffer CAMERA_DATA : register(b0)
{
    matrix View;
    matrix InvView;
    matrix Projection;
}

cbuffer OBJECT_DATA : register(b1)
{
    matrix World;
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
    
    //matrix ViewProjection = mul(View, Projection);
    //matrix WorldViewProjection = mul(World, ViewProjection);
    
    //output.Pos = mul(float4(input.Pos, 1.0f), WorldViewProjection);
	//output.Pos = float4(0.0, 0.0, 0.5, 1.0);
	output.Pos = float4(input.Pos.x, input.Pos.y, 0.5, 1.0);
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
