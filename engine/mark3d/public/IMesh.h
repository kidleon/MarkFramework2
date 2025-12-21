#ifndef __I_MESH_H__
#define __I_MESH_H__


using namespace mark;

interface IMesh : public IAsset
{
	/**
	* @brief 프리미티브 추가
	* @return 추가된 프리미티브 인덱스
	* @note 프리미티브는 일종의 서브메시로, 각 프리미티브는 독립적인 기하구조를 가질 수 있습니다.
	*/
	virtual int32 AddPrimitive(PRIMITIVE_TYPE PrimitiveType, size_t MaxVertex, size_t MaxIndex) noexcept = 0;

	/**
	* @brief 프리미티브 수 반환
	* @return 프리미티브 수
	*/
	virtual int32 GetNumPrimitives() const noexcept = 0;

	/**
	* @brief 프리미티브 유형 설정
	* @param PrimitiveType 프리미티브 유형
	* @return 없음
	*/
	virtual void SetPrimitiveType(int32 PirmitiveIndex, PRIMITIVE_TYPE PrimitiveType) = 0;

	/**
	* @brief 프리미티브 유형 반환
	* @param PirmitiveIndex 프리미티브 인덱스
	* @return 프리미티브 유형
	*/
	virtual PRIMITIVE_TYPE GetPrimitiveType(int32 PirmitiveIndex) const noexcept = 0;

	/**
	* @brief 버텍스 및 인덱스 데이터 설정 및 추가
	* @param PirmitiveIndex 프리미티브 인덱스
	* @param pPositions 버텍스 위치 배열
	* @param NumPosition 위치 수
	^ @return 없음
	*/
	virtual void SetPositions(int32 PirmitiveIndex, const FLOAT3* pPositions, size_t NumPosition) = 0;


	virtual void SetNormals(int32 PirmitiveIndex, const FLOAT3* pNormals, size_t NumNormal) = 0;
	virtual void SetColors(int32 PirmitiveIndex, const COLOR_VAULE* pColors, size_t NumColor) = 0;
	virtual void SetTexCoords(int32 PirmitiveIndex, const FLOAT2* pTexCoords, size_t NumTexCoord) = 0;
	virtual void SetTexCoords1(int32 PirmitiveIndex, const FLOAT4* pTexCoords, size_t NumTexCoord) = 0;
	virtual void SetTangents(int32 PirmitiveIndex, const FLOAT3* pTangents, size_t NumTangent) = 0;
	virtual void SetBinormals(int32 PirmitiveIndex, const FLOAT3* pBinormals, size_t NumBinormal) = 0;
	virtual void SetIndices16(int32 PirmitiveIndex, const uint16* pIndices, size_t NumIndex) = 0;

	virtual int32 AddPosition(int32 PirmitiveIndex, const FLOAT3& Position) = 0;
	virtual int32 AddNormal(int32 PirmitiveIndex, const FLOAT3& Normal) = 0;
	virtual int32 AddColor(int32 PirmitiveIndex, const COLOR_VAULE& Color) = 0;
	virtual int32 AddTexCoord(int32 PirmitiveIndex, const FLOAT2& TexCoord) = 0;
	virtual int32 AddTexCoord1(int32 PirmitiveIndex, const FLOAT4& TexCoord) = 0;
	virtual int32 AddTangent(int32 PirmitiveIndex, const FLOAT3& Tangent) = 0;
	virtual int32 AddBinormal(int32 PirmitiveIndex, const FLOAT3& Binormal) = 0;
	virtual int32 AddIndex16(int32 PirmitiveIndex, uint16 Index) = 0;

	virtual FLOAT3* GetPosition(int32 PirmitiveIndex) noexcept = 0;
	virtual FLOAT3* GetNormal(int32 PirmitiveIndex) noexcept = 0;
	virtual COLOR_VAULE* GetColor(int32 PirmitiveIndex) noexcept = 0;
	virtual FLOAT2* GetTexCoord(int32 PirmitiveIndex) noexcept = 0;
	virtual FLOAT4* GetTexCoord1(int32 PirmitiveIndex) noexcept = 0;
	virtual FLOAT3* GetTangent(int32 PirmitiveIndex) noexcept = 0;
	virtual FLOAT3* GetBinormal(int32 PirmitiveIndex) noexcept = 0;
	virtual uint16* GetIndex16(int32 PirmitiveIndex) noexcept = 0;

	virtual size_t GetVertexCount(int32 PirmitiveIndex) const noexcept = 0;
	virtual size_t GetIndexCount(int32 PirmitiveIndex) const noexcept = 0;

};

#endif // __I_MESH_H__