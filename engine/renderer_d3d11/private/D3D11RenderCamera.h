#ifndef __D3D11_RENDER_CAMERA_H__
#define __D3D11_RENDER_CAMERA_H__


class D3D11RenderTarget;

class D3D11RenderCamera final : public IRenderCamera
{
public:
	static constexpr UINT32 HAS_RENDER_TARGET = 0x00000001u;
	static constexpr UINT32 HAS_RENDER_QUEUE = 0x00000002u;

	struct CLEAR_TARGET_DESC
	{
		UINT32 ClearBuffers;
		FLOAT4 ClearColor;
		FLOAT Depth;
		UINT32 Stencil;

		UINT32 PADDING;
	};

	struct PERSPECTIVE_DESC
	{
		FLOAT Fovy;
		FLOAT Aspect;
		FLOAT NearZ;
		FLOAT FarZ;
	};

	struct ORTHO_DESC
	{
		FLOAT ViewWidth;
		FLOAT ViewHeight;
		FLOAT NearZ;
		FLOAT FarZ;
	};

	struct VIEW_DESC
	{
		FLOAT3 EyePos;
		FLOAT3 EyeDir;
		FLOAT3 EyeUp;
		FLOAT3 EyeRight;
	};

public:
	explicit D3D11RenderCamera(
		CAMERA_MODE CameraMode,
		D3D11RenderTarget* pRenderTarget
	);

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IAsset interface
	virtual UINT64 GetID() const noexcept final;
	virtual ASSET_TYPE GetAssetType() const noexcept final;
	virtual LOAD_STAT GetLoadStat() const noexcept final;

	// IRenderCamera interface
	void SetClearTarget(
		UINT32 ClearBuffers,
		const FLOAT4& ClearColor,
		FLOAT Depth,
		UINT32 Stencil
	) noexcept final;

	void SetCameraMode(CAMERA_MODE CameraMode) noexcept final;

	void SetPerspective(
		FLOAT Fovy,
		FLOAT Aspect,
		FLOAT NearZ,
		FLOAT FarZ
	) noexcept final;

	void SetOrtho(
		FLOAT ViewWidth,
		FLOAT ViewHeight,
		FLOAT NearZ,
		FLOAT FarZ
	) noexcept final;

	void SetView(
		const FLOAT3& EyePos,
		const FLOAT3& EyeDir,
		const FLOAT3& EyeUp
	) final;

	void LookAt(const FLOAT3& EyePos, const FLOAT3& Target) noexcept final;

	void SetCameraOrder(INT8 Order) noexcept final;

	void ComputeCamera() noexcept;

	__INLINE INT32 INL_GetCameraOrder() const noexcept { return m_CameraOrder; }

	__INLINE const CLEAR_TARGET_DESC& INL_GetClearTargetDesc() const noexcept { return m_ClearTarget; }
	__INLINE const PERSPECTIVE_DESC& INL_GetPerspectiveDesc() const noexcept { return m_Perspective; }
	__INLINE const ORTHO_DESC& INL_GetOrthoDesc() const noexcept { return m_Ortho; }
	__INLINE const VIEW_DESC& INL_GetViewDesc() const noexcept { return m_View; }
	__INLINE CAMERA_MODE INL_GetCameraMode() const noexcept { return m_CameraMode; }
	__INLINE BOOL INL_HasRenderTarget() const noexcept { return m_CameraFlags & HAS_RENDER_TARGET; }
	__INLINE BOOL INL_HasRenderQueue() const noexcept { return m_CameraFlags & HAS_RENDER_QUEUE; }
	__INLINE const MATRIX4& INL_GetViewMatrix() const noexcept { return m_ViewMatrix; }
	__INLINE const MATRIX4& INL_GetProjectionMatrix() const noexcept { return m_ProjectionMatrix; }
	__INLINE const D3D11_VIEWPORT INL_GetViewport() const noexcept { return m_Viewport; }
	__INLINE D3D11RenderTarget* INL_GetRenderTarget() const noexcept { return m_pRenderTarget; }

	__FORCEINLINE FLOAT INL_GetDepthFarZ() const noexcept
	{
		return (m_CameraMode == CAMERA_MODE::PERSPECTIVE) ? m_Perspective.FarZ : m_Ortho.FarZ;
	}

	__FORCEINLINE FLOAT3 INL_GetEyePos() const noexcept
	{
		return m_View.EyePos;
	}

protected:
	D3D11RenderCamera() = delete;
	virtual ~D3D11RenderCamera() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	UINT64 m_ID = 0;
	LOAD_STAT m_LoadStat = LOAD_STAT::NOT_LOADED;
	INT32 m_CameraOrder = 0;

	CLEAR_TARGET_DESC m_ClearTarget;
	PERSPECTIVE_DESC m_Perspective;
	ORTHO_DESC m_Ortho;
	VIEW_DESC m_View;
	CAMERA_MODE m_CameraMode = CAMERA_MODE::PERSPECTIVE;
	UINT32 m_CameraFlags = 0;

	MATRIX4 m_ViewMatrix = MATRIX4_IDENT;
	MATRIX4 m_ProjectionMatrix = MATRIX4_IDENT;

	D3D11_VIEWPORT m_Viewport = {};

	D3D11RenderTarget* m_pRenderTarget = nullptr;


};



#endif // __D3D11_RENDER_CAMERA_H__
