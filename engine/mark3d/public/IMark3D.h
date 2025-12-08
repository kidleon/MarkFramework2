#ifndef __IMARK3D_H__
#define __IMARK3D_H__


interface IMark3D : public IUNKNOWN
{
	/**
	* @brief 3D 엔진 생성
	* @param CreateDesc 엔진 생성 정보
	* @return 생성 성공 시 TRUE, 실패 시 FALSE
	*/
	virtual BOOL CreateEngine(const ENGINE_CREATE_DESC* pDesc) = 0;
};


#endif // __IMARK3D_H__
