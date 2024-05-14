#pragma once
#include "../Ref.h"

ENGINE_BEGIN

class ENGINE_DLL CRenderState :
	public CRef
{
	friend class CRenderManager;

protected:
	CRenderState();
	~CRenderState();

protected:
	ID3D11DeviceChild*	m_pState;
	ID3D11DeviceChild*	m_pOldState;
	RENDER_STATE		m_eType;

public:
	RENDER_STATE GetRenderStateType()	const;

public:
	virtual void SetState() = 0;
	virtual void ResetState() = 0;
};

ENGINE_END
