#include "pch.h"
#include "RenderDef.h"


//--------------------------------------------------------------------------
static constexpr RS_SAMPLER_STATE CreateLinearWrap()
{
	return RS_SAMPLER_STATE();
}

static constexpr RS_SAMPLER_STATE CreatePointWrap()
{
	RS_SAMPLER_STATE state;
	state.MinFilter = SAMPLER_FILTER::NEAREST;
	state.MagFilter = SAMPLER_FILTER::NEAREST;
	state.MipFilter = SAMPLER_FILTER::NEAREST;
	return state;
}

static constexpr RS_SAMPLER_STATE CreateAnisotropicWrap(uint8_t MaxAnisotropy = 16)
{
	RS_SAMPLER_STATE state;
	state.MinFilter = SAMPLER_FILTER::LINEAR;
	state.MagFilter = SAMPLER_FILTER::LINEAR;
	state.MipFilter = SAMPLER_FILTER::LINEAR;
	state.MaxAnisotropy = MaxAnisotropy;
	return state;
}

static constexpr RS_SAMPLER_STATE CreateLinearClamp()
{
	RS_SAMPLER_STATE state;
	state.MinFilter = SAMPLER_FILTER::LINEAR;
	state.MagFilter = SAMPLER_FILTER::LINEAR;
	state.MipFilter = SAMPLER_FILTER::NONE;
	state.AddressU = TEXTURE_ADDRESS_MODE::CLAMP;
	state.AddressV = TEXTURE_ADDRESS_MODE::CLAMP;
	state.AddressW = TEXTURE_ADDRESS_MODE::CLAMP;
	state.ComparisonFunc = COMPARISON_FUNC::LESS_EQUAL;
	state.BorderColor = BORDER_COLOR::OPAQUE_WHITE;
	return state;
}

static constexpr RS_SAMPLER_STATE CreateShadowMap()
{
	RS_SAMPLER_STATE state;
	state.MinFilter = SAMPLER_FILTER::LINEAR;
	state.MagFilter = SAMPLER_FILTER::LINEAR;
	state.MipFilter = SAMPLER_FILTER::NONE;
	state.AddressU = state.AddressV = state.AddressW = TEXTURE_ADDRESS_MODE::CLAMP;
	state.ComparisonFunc = COMPARISON_FUNC::LESS_EQUAL;
	state.BorderColor = BORDER_COLOR::OPAQUE_WHITE;
	return state;
}

RS_SAMPLER_STATE RS_SAMPLER_STATE::LINEAR_WRAP = CreateLinearWrap();
RS_SAMPLER_STATE RS_SAMPLER_STATE::POINT_WRAP = CreatePointWrap();
RS_SAMPLER_STATE RS_SAMPLER_STATE::ANISOTROPIC_WRAP = CreateAnisotropicWrap(16);
RS_SAMPLER_STATE RS_SAMPLER_STATE::LINEAR_CLAMP = CreateLinearClamp();
RS_SAMPLER_STATE RS_SAMPLER_STATE::SHADOW_MAP = CreateShadowMap();


//--------------------------------------------------------------------------
static constexpr RS_BLEND_TARGET CreateOpaque()
{
	RS_BLEND_TARGET target;
	target.BlendEnable = FALSE;
	return target;
}

static constexpr RS_BLEND_TARGET CreateAlphaBlend()
{
	RS_BLEND_TARGET target;
	target.BlendEnable = TRUE;
	target.SrcBlend = BLEND_FACTOR::SRC_ALPHA;
	target.DestBlend = BLEND_FACTOR::INV_SRC_ALPHA;
	return target;
}

static constexpr RS_BLEND_TARGET CreatePremultipliedAlpha()
{
	RS_BLEND_TARGET target;
	target.BlendEnable = TRUE;
	target.SrcBlend = BLEND_FACTOR::ONE;
	target.DestBlend = BLEND_FACTOR::INV_SRC_ALPHA;
	return target;
}

static constexpr RS_BLEND_TARGET CreateAdditive()
{
	RS_BLEND_TARGET target;
	target.SrcBlend = BLEND_FACTOR::ONE;
	target.DestBlend = BLEND_FACTOR::ONE;
	return target;
}

static constexpr RS_BLEND_TARGET CreateMultiply()
{
	RS_BLEND_TARGET target;
	target.SrcBlend = BLEND_FACTOR::DST_COLOR;
	target.DestBlend = BLEND_FACTOR::ZERO;
	return target;
}

static constexpr RS_BLEND_TARGET CreateMultiply2x()
{
	RS_BLEND_TARGET target;
	target.SrcBlend = BLEND_FACTOR::DST_COLOR;
	target.DestBlend = BLEND_FACTOR::SRC_COLOR;
	return target;
}

static constexpr RS_BLEND_TARGET CreateScreen()
{
	RS_BLEND_TARGET target;
	target.SrcBlend = BLEND_FACTOR::ONE;
	target.DestBlend = BLEND_FACTOR::INV_SRC_COLOR;
	return target;
}

static constexpr RS_BLEND_TARGET CreateMin()
{
	RS_BLEND_TARGET target;
	target.SrcBlend = BLEND_FACTOR::ONE;
	target.DestBlend = BLEND_FACTOR::ONE;
	target.BlendOp = BLEND_OP::MIN;
	return target;
}

static constexpr RS_BLEND_TARGET CreateMax()
{
	RS_BLEND_TARGET target;
	target.SrcBlend = BLEND_FACTOR::ONE;
	target.DestBlend = BLEND_FACTOR::ONE;
	target.BlendOp = BLEND_OP::MAX;
	return target;
}

static constexpr RS_BLEND_TARGET CreateAlphaOnly()
{
	RS_BLEND_TARGET target;
	target.BlendEnable = FALSE;
	target.RenderTargetWriteMask = COLOR_WRITE_MASK::ALPHA;
	return target;
}

static constexpr RS_BLEND_TARGET CreateDepthOnly()
{
	RS_BLEND_TARGET target;
	target.BlendEnable = FALSE;
	target.RenderTargetWriteMask = COLOR_WRITE_MASK::DISABLE;
	return target;
}

static constexpr RS_BLEND_TARGET CreateColorOnly()
{
	RS_BLEND_TARGET target;
	target.BlendEnable = FALSE;
	target.RenderTargetWriteMask = COLOR_WRITE_MASK::RED | COLOR_WRITE_MASK::GREEN | COLOR_WRITE_MASK::BLUE;
	return target;
}

static constexpr RS_BLEND_TARGET CreateAlphaBlendAddAlpha()
{
	RS_BLEND_TARGET target;
	target.BlendEnable = TRUE;
	target.SrcBlend = BLEND_FACTOR::SRC_ALPHA;
	target.DestBlend = BLEND_FACTOR::INV_SRC_ALPHA;
	target.BlendOp = BLEND_OP::ADD;
	target.SrcBlendAlpha = BLEND_FACTOR::ONE;
	target.DestBlendAlpha = BLEND_FACTOR::ONE;
	target.BlendOpAlpha = BLEND_OP::ADD;

	return target;
}

RS_BLEND_TARGET RS_BLEND_TARGET::NO_BLEND = CreateOpaque();
RS_BLEND_TARGET RS_BLEND_TARGET::ALPHA_BLEND = CreateAlphaBlend();
RS_BLEND_TARGET RS_BLEND_TARGET::PREMUL_ALPHA_BLEND = CreatePremultipliedAlpha();
RS_BLEND_TARGET RS_BLEND_TARGET::ADDITIVE_BLEND = CreateAdditive();
RS_BLEND_TARGET RS_BLEND_TARGET::MULTIPLY_BLEND = CreateMultiply();
RS_BLEND_TARGET RS_BLEND_TARGET::MULTIPLY2X_BLEND = CreateMultiply2x();
RS_BLEND_TARGET RS_BLEND_TARGET::SCREEN_BLEND = CreateScreen();
RS_BLEND_TARGET RS_BLEND_TARGET::MIN_BLEND = CreateMin();
RS_BLEND_TARGET RS_BLEND_TARGET::MAX_BLEND = CreateMax();
RS_BLEND_TARGET RS_BLEND_TARGET::ALPHA_ONLY = CreateAlphaOnly();
RS_BLEND_TARGET RS_BLEND_TARGET::COLOR_ONLY = CreateColorOnly();
RS_BLEND_TARGET RS_BLEND_TARGET::DEPTH_ONLY = CreateDepthOnly();
RS_BLEND_TARGET RS_BLEND_TARGET::ALPHA_BLEND_ADD = CreateAlphaBlendAddAlpha();


//--------------------------------------------------------------------------
static constexpr RS_DEPTH_STENCIL_STATE CreateDefault()
{
	return RS_DEPTH_STENCIL_STATE();
}

static constexpr RS_DEPTH_STENCIL_STATE CreateDepthReadOnly()
{
	RS_DEPTH_STENCIL_STATE state;
	state.DepthWriteEnable = FALSE;
	return state;
}

static constexpr RS_DEPTH_STENCIL_STATE CreateDepthWriteOnly()
{
	RS_DEPTH_STENCIL_STATE state;
	state.DepthEnable = FALSE;
	return state;
}

static constexpr RS_DEPTH_STENCIL_STATE CreateDepthDisabled()
{
	RS_DEPTH_STENCIL_STATE state;
	state.DepthEnable = FALSE;
	state.DepthWriteEnable = FALSE;
	return state;
}

static constexpr RS_DEPTH_STENCIL_STATE CreateDepthEqual()
{
	RS_DEPTH_STENCIL_STATE state;
	state.DepthFunc = COMPARISON_FUNC::EQUAL;
	return state;
}

static constexpr RS_DEPTH_STENCIL_STATE CreateDepthReverseZ()
{
	RS_DEPTH_STENCIL_STATE state;
	state.DepthFunc = COMPARISON_FUNC::GREATER_EQUAL;
	return state;
}

static constexpr RS_DEPTH_STENCIL_STATE CreateStencilWrite()
{
	RS_DEPTH_STENCIL_STATE state;
	state.DepthEnable = FALSE;
	state.DepthWriteEnable = FALSE;
	state.StencilEnable = TRUE;
	state.FrontFace.StencilFunc = STENCIL_FUNC::ALWAYS;
	state.FrontFace.StencilPassOp = STENCIL_OP::REPLACE;
	state.BackFace = state.FrontFace;
	return state;
}

static constexpr RS_DEPTH_STENCIL_STATE CreateStencilTest()
{
	RS_DEPTH_STENCIL_STATE state;
	state.DepthEnable = TRUE;
	state.DepthWriteEnable = TRUE;
	state.StencilEnable = TRUE;
	state.FrontFace.StencilFunc = STENCIL_FUNC::EQUAL;
	state.BackFace = state.FrontFace;
	return state;
}

static constexpr RS_DEPTH_STENCIL_STATE CreateStencilIncr()
{
	RS_DEPTH_STENCIL_STATE state;
	state.DepthEnable = TRUE;
	state.DepthWriteEnable = FALSE;
	state.StencilEnable = TRUE;
	state.FrontFace.StencilFunc = STENCIL_FUNC::ALWAYS;
	state.FrontFace.StencilPassOp = STENCIL_OP::INCR_WRAP;
	state.BackFace = state.FrontFace;
	return state;
}

static constexpr RS_DEPTH_STENCIL_STATE CreateStencilDecr()
{
	RS_DEPTH_STENCIL_STATE state;
	state.DepthEnable = TRUE;
	state.DepthWriteEnable = FALSE;
	state.StencilEnable = TRUE;
	state.FrontFace.StencilFunc = STENCIL_FUNC::ALWAYS;
	state.FrontFace.StencilPassOp = STENCIL_OP::DECR_WRAP;
	state.BackFace = state.FrontFace;
	return state;
}

static constexpr RS_DEPTH_STENCIL_STATE CreateStencilTwoSided()
{
	RS_DEPTH_STENCIL_STATE state;
	state.DepthEnable = TRUE;
	state.DepthWriteEnable = TRUE;
	state.StencilEnable = TRUE;

	// 앞면: 증가
	state.FrontFace = RS_STENCIL_OP(
		STENCIL_OP::KEEP,
		STENCIL_OP::INCR_WRAP,
		STENCIL_OP::KEEP,
		STENCIL_FUNC::ALWAYS
	);

	// 뒷면: 감소
	state.BackFace = RS_STENCIL_OP(
		STENCIL_OP::KEEP,
		STENCIL_OP::DECR_WRAP,
		STENCIL_OP::KEEP,
		STENCIL_FUNC::ALWAYS
	);

	return state;
}

static constexpr RS_DEPTH_STENCIL_STATE CreateStencilMask()
{
	RS_DEPTH_STENCIL_STATE state;
	state.DepthEnable = TRUE;
	state.DepthWriteEnable = FALSE;
	state.DepthFunc = COMPARISON_FUNC::LESS;
	state.StencilEnable = TRUE;
	state.StencilReadMask = 0xFF;
	state.StencilWriteMask = 0xFF;

	// 앞면: 증가
	state.FrontFace = RS_STENCIL_OP(
		STENCIL_OP::KEEP,
		STENCIL_OP::INCR_WRAP,
		STENCIL_OP::KEEP,
		STENCIL_FUNC::ALWAYS
	);

	// 뒷면: 감소
	state.BackFace = RS_STENCIL_OP(
		STENCIL_OP::KEEP,
		STENCIL_OP::DECR_WRAP,
		STENCIL_OP::KEEP,
		STENCIL_FUNC::ALWAYS
	);

	return state;
}

RS_DEPTH_STENCIL_STATE RS_DEPTH_STENCIL_STATE::DEFAULT = CreateDefault();
RS_DEPTH_STENCIL_STATE RS_DEPTH_STENCIL_STATE::DEPTH_READ_ONLY = CreateDepthReadOnly();
RS_DEPTH_STENCIL_STATE RS_DEPTH_STENCIL_STATE::DEPTH_WRITE_ONLY = CreateDepthWriteOnly();
RS_DEPTH_STENCIL_STATE RS_DEPTH_STENCIL_STATE::DEPTH_DISABLED = CreateDepthDisabled();
RS_DEPTH_STENCIL_STATE RS_DEPTH_STENCIL_STATE::DEPTH_EQUAL = CreateDepthEqual();
RS_DEPTH_STENCIL_STATE RS_DEPTH_STENCIL_STATE::DEPTH_REVERSE_Z = CreateDepthReverseZ();
RS_DEPTH_STENCIL_STATE RS_DEPTH_STENCIL_STATE::STENCIL_WRITE = CreateStencilWrite();
RS_DEPTH_STENCIL_STATE RS_DEPTH_STENCIL_STATE::STENCIL_TEST = CreateStencilTest();
RS_DEPTH_STENCIL_STATE RS_DEPTH_STENCIL_STATE::STENCIL_INCR = CreateStencilIncr();
RS_DEPTH_STENCIL_STATE RS_DEPTH_STENCIL_STATE::STENCIL_DECR = CreateStencilDecr();
RS_DEPTH_STENCIL_STATE RS_DEPTH_STENCIL_STATE::STENCIL_TWO_SIDED = CreateStencilTwoSided();
RS_DEPTH_STENCIL_STATE RS_DEPTH_STENCIL_STATE::STENCIL_MASK = CreateStencilMask();

//--------------------------------------------------------------------------
static constexpr RS_RASTERIZER_STATE CreateDefaultRaster()
{
	return RS_RASTERIZER_STATE();
}

static constexpr RS_RASTERIZER_STATE CreateWireframe()
{
	RS_RASTERIZER_STATE state;
	state.FillMode = FILL_MODE::WIREFRAME;
	return state;
}

static constexpr RS_RASTERIZER_STATE CreateWireframeTwoSide()
{
	RS_RASTERIZER_STATE state;
	state.FillMode = FILL_MODE::WIREFRAME;
	state.CullMode = CULL_MODE::NONE;
	return state;
}

static constexpr RS_RASTERIZER_STATE CreateTwoSide()
{
	RS_RASTERIZER_STATE state;
	state.CullMode = CULL_MODE::NONE;
	return state;
}

RS_RASTERIZER_STATE RS_RASTERIZER_STATE::DEFAULT = CreateDefaultRaster();
RS_RASTERIZER_STATE RS_RASTERIZER_STATE::WIREFRAME = CreateWireframe();
RS_RASTERIZER_STATE RS_RASTERIZER_STATE::WIREFRAME_TWOSIDE = CreateWireframeTwoSide();
RS_RASTERIZER_STATE RS_RASTERIZER_STATE::TWO_SIDE = CreateTwoSide();


