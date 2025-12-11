#include "pch.h"
#include "D3D11RenderResources.h"
#include "D3D11InputLayout.h"
#include "D3D11ShaderParams.h"


D3D11VertexShader::~D3D11VertexShader() noexcept
{
	// Release resources
	if (pVS)
	{
		pVS->Release();
		pVS = nullptr;
	}

	if (pShaderParams)
	{
		MARK_POOL_FREE(pShaderParams);
		pShaderParams = nullptr;
	}

	if (pInputLayout)
	{
		MARK_POOL_FREE(pInputLayout);
		pInputLayout = nullptr;
	}
}

void D3D11VertexShader::OnDestroy()
{
	MARK_POOL_DELETE(this, D3D11VertexShader);
}

//----------------------------------------------------------------
D3D11PixelShader::~D3D11PixelShader() noexcept
{
	// Release resources
	if (pPS)
	{
		pPS->Release();
		pPS = nullptr;
	}

	if (pShaderParams)
	{
		MARK_POOL_FREE(pShaderParams);
		pShaderParams = nullptr;
	}
}

void D3D11PixelShader::OnDestroy()
{
	MARK_POOL_DELETE(this, D3D11PixelShader);
}

//----------------------------------------------------------------
D3D11ComputeShader::~D3D11ComputeShader() noexcept
{
	// Release resources
	if (pCS)
	{
		pCS->Release();
		pCS = nullptr;
	}

	if (pShaderParams)
	{
		MARK_POOL_FREE(pShaderParams);
		pShaderParams = nullptr;
	}
}

void D3D11ComputeShader::OnDestroy()
{
	MARK_POOL_DELETE(this, D3D11ComputeShader);
}

//----------------------------------------------------------------
D3D11SamplerState::~D3D11SamplerState() noexcept
{
	// Release resources
	if (pSamplerState)
	{
		pSamplerState->Release();
		pSamplerState = nullptr;
	}
}

void D3D11SamplerState::OnDestroy()
{
	MARK_POOL_DELETE(this, D3D11SamplerState);
}

//----------------------------------------------------------------
D3D11BlendState::~D3D11BlendState() noexcept
{
	// Release resources
	if (pBlendState)
	{
		pBlendState->Release();
		pBlendState = nullptr;
	}
}

void D3D11BlendState::OnDestroy()
{
	MARK_POOL_DELETE(this, D3D11BlendState);
}

//----------------------------------------------------------------
D3D11RasterizerState::~D3D11RasterizerState() noexcept
{
	// Release resources
	if (pRasterizerState)
	{
		pRasterizerState->Release();
		pRasterizerState = nullptr;
	}
}

void D3D11RasterizerState::OnDestroy()
{
	MARK_POOL_DELETE(this, D3D11RasterizerState);
}

//----------------------------------------------------------------
D3D11DepthStencilState::~D3D11DepthStencilState() noexcept
{
	// Release resources
	if (pDepthStencilState)
	{
		pDepthStencilState->Release();
		pDepthStencilState = nullptr;
	}
}

void D3D11DepthStencilState::OnDestroy()
{
	MARK_POOL_DELETE(this, D3D11DepthStencilState);
}