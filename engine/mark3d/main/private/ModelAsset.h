#ifndef __MODEL_ASSET_H__
#define __MODEL_ASSET_H__


class ModelAsset : public IModelAsset
{
public:
	ModelAsset(UINT64 ID);
	
	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IAsset interface
	virtual UINT64 GetID() const noexcept final;
	virtual ASSET_TYPE GetAssetType() const noexcept final;
	virtual LOAD_STAT GetLoadStat() const noexcept final;

	// IModelAsset interface
	virtual void INL_SetLoadStat(LOAD_STAT LoadStat) noexcept final;
};
