#include "RenderState.h"

ENGINE_USING

CRenderState::CRenderState()	:
	m_pOldState(nullptr),
	m_pState(nullptr)
{
}


CRenderState::~CRenderState()
{
	SAFE_RELEASE(m_pOldState);
	SAFE_RELEASE(m_pState);
}

RENDER_STATE CRenderState::GetRenderStateType() const
{
	return m_eType;
}
