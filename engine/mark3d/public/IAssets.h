#ifndef __IASSETS_H__
#define __IASSETS_H__


interface ITextAsset;
interface IBinaryAsset;
interface ITexture1D;
interface ITexture2D;

interface IAssets : public IUNKNOWN
{
	/**
	* @brief 텍스트 자산 로드
	* @param szRelativePath 자산의 상대 경로
	* @param ppOut 로드된 ITextAsset 포인터를 받을 변수의 주소
	* @return 로드 성공 시 TRUE, 실패 시 FALSE
	*/
	virtual BOOL Load(const char* szRelativePath, ITextAsset** ppOut) = 0;

	/**
	* @brief 비동기 텍스트 자산 로드
	* @param szRelativePath 자산의 상대 경로
	* @param ppOut 로드된 ITextAsset 포인터를 받을 변수의 주소
	* @return 로드 성공 시 TRUE, 실패 시 FALSE
	*/
	virtual BOOL LoadAsync(const char* szRelativePath, ITextAsset** ppOut) = 0;

	/**
	* @brief 바이너리 자산 로드
	* @param szRelativePath 자산의 상대 경로
	* @param ppOut 로드된 IBinaryAsset 포인터를 받을 변수의 주소
	* @return 로드 성공 시 TRUE, 실패 시 FALSE
	*/
	virtual BOOL Load(const char* szRelativePath, IBinaryAsset** ppOut) = 0;

	/**
	* @brief 비동기 바이너리 자산 로드
	* @param szRelativePath 자산의 상대 경로
	* @param ppOut 로드된 IBinaryAsset 포인터를 받을 변수의 주소
	* @return 로드 성공 시 TRUE, 실패 시 FALSE
	*/
	virtual BOOL LoadAsync(const char* szRelativePath, IBinaryAsset** ppOut) = 0;

	/**
	* @brief 1D 텍스처 자산 로드
	* @param szRelativePath 자산의 상대 경로
	* @param ppOut 로드된 ITexture1D 포인터를 받을 변수의 주소
	* @return 로드 성공 시 TRUE, 실패 시 FALSE
	*/
	virtual BOOL Load(const char* szRelativePath, ITexture1D** ppOut) = 0;

	/**
	* @brief 비동기 1D 텍스처 자산 로드
	* @param szRelativePath 자산의 상대 경로
	* @param ppOut 로드된 ITexture1D 포인터를 받을 변수의 주소
	* @return 로드 성공 시 TRUE, 실패 시 FALSE
	*/
	virtual BOOL LoadAsync(const char* szRelativePath, ITexture1D** ppOut) = 0;

	/**
	* @brief 2D 텍스처 자산 로드
	* @param szRelativePath 자산의 상대 경로
	* @param ppOut 로드된 ITexture1D 포인터를 받을 변수의 주소
	* @return 로드 성공 시 TRUE, 실패 시 FALSE
	*/
	virtual BOOL Load(const char* szRelativePath, ITexture2D** ppOut) = 0;

	/**
	* @brief 비동기 2D 텍스처 자산 로드
	* @param szRelativePath 자산의 상대 경로
	* @param ppOut 로드된 ITexture1D 포인터를 받을 변수의 주소
	* @return 로드 성공 시 TRUE, 실패 시 FALSE
	*/
	virtual BOOL LoadAsync(const char* szRelativePath, ITexture2D** ppOut) = 0;

};


#endif // __IASSETS_H__
