#ifndef __MARK3D_H__
#define __MARK3D_H__

#include "IMark3D.h"


class Mark3D : public IMark3D
{
	DECLARATION_IUNKNOWN_INTERFACE(Mark3D);

public:
	Mark3D();

	virtual BOOL CreateEngine(const ENGINE_CREATE_DESC* pDesc) override;

private:
	IRenderSystem* m_pRenderSystem = nullptr;

};

#endif // __MARK3D_H__