#ifndef __RENDER_STATE_PRESET_H__
#define __RENDER_STATE_PRESET_H__


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


#endif // __RENDER_STATE_PRESET_H__
