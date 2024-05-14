#pragma once
#include "Component.h"

ENGINE_BEGIN

class ENGINE_DLL CLight :
	public CComponent
{
	friend class CGameObject;

protected:
	CLight();
	CLight(const CLight& com);
	~CLight();

private:
	LightInfo	m_tInfo;
	float		m_fInAngle;
	float		m_fOutAngle;

public:
	LightInfo GetLightInfo()	const;

public:
	void SetLightType(LIGHT_TYPE eType);
	void SetLightColor(const Vector4& vDif, const Vector4& vAmb,
		const Vector4& vSpc);
	void SetLightDistance(float fDist);
	void SetLightAngle(float fInAngle, float fOutAngle);

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CLight* Clone()	const;

public:
	void SetShader();
};

ENGINE_END
