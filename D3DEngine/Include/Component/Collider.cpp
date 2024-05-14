#include "Collider.h"
#include "../CollisionManager.h"

#ifdef _DEBUG
#include "../Render/Shader.h"
#include "../Render/ShaderManager.h"
#include "../Resource/Mesh.h"
#include "../Render/RenderState.h"
#include "../Render/RenderManager.h"
#include "../Device.h"
#endif // _DEBUG

ENGINE_USING

CCollider::CCollider()	:  m_pPrevNumber(nullptr), m_pCollisionList(nullptr), m_bPick(false), m_bAddCollisionManager(true)
{
	m_eComType = CT_COLLIDER;
	memset(m_bCallback, 0, sizeof(bool) * CCS_END);

#ifdef _DEBUG
	m_vColor = Vector4::Green;

	m_pShader = GET_SINGLE(CShaderManager)->FindShader(COLLIDER_SHADER);
	m_pLayout = GET_SINGLE(CShaderManager)->FindInputLayout(POS_LAYOUT);
	m_pMesh = nullptr;
	m_pWireFrame = GET_SINGLE(CRenderManager)->FindRenderState("WireFrame");
#endif // _DEBUG

	m_pProfile = GET_SINGLE(CCollisionManager)->FindProfile("WorldStatic");
	m_iChannelIndex = 0;
}


CCollider::CCollider(const CCollider & com)	:
	CComponent(com)
{
	*this = com;
	m_iRefCount = 1;
	memset(m_bCallback, 0, sizeof(bool) * CCS_END);

#ifdef _DEBUG
	if (m_pShader)
		m_pShader->AddRef();

	if (m_pMesh)
		m_pMesh->AddRef();

	if (m_pWireFrame)
		m_pWireFrame->AddRef();
#endif // _DEBUG
}

CCollider::~CCollider()
{
#ifdef _DEBUG
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_pMesh);
	SAFE_RELEASE(m_pWireFrame);
#endif // _DEBUG

	// 이전 충돌 목록과의 관계를 초기화한다.
	ClearState();

	SAFE_DELETE_ARRAY(m_pCollisionList);
	SAFE_DELETE_ARRAY(m_pPrevNumber);

	if (m_iSerialNumber != UINT_MAX)
	{
		GET_SINGLE(CCollisionManager)->AddValidSerialNumber(m_iSerialNumber);
	}
}

COLLIDER_TYPE CCollider::GetColliderType() const
{
	return m_eCollType;
}

Vector3 CCollider::GetSectionMin() const
{
	return m_vSectionMin;
}

Vector3 CCollider::GetSectionMax() const
{
	return m_vSectionMax;
}

_tagCollisionProfile * CCollider::GetCollisionProfile() const
{
	return m_pProfile;
}

unsigned int CCollider::GetCollisionChannelIndex() const
{
	return m_iChannelIndex;
}
bool CCollider::GetPickEnable() const
{
	return m_bPick;
}

void CCollider::PickEnable()
{
	m_bPick = true;
}
void CCollider::EnableCollisionManager(bool bCollisionManager)
{
	m_bAddCollisionManager = bCollisionManager;
}
void CCollider::SetChannel(const string & strChannel)
{
	m_iChannelIndex = GET_SINGLE(CCollisionManager)->GetChannelIndex(strChannel);
}

void CCollider::SetProfile(const string & strProfile)
{
	m_pProfile = GET_SINGLE(CCollisionManager)->FindProfile(strProfile);
}

void CCollider::SetSerialNumber(unsigned int iSerialNumber)
{
	m_iSerialNumber = iSerialNumber;
}

void CCollider::ClearState()
{
	if (m_iSerialNumber == UINT_MAX)
		return;

	for (unsigned int i = 0; i < m_iPrevSize; ++i)
	{
		GET_SINGLE(CCollisionManager)->DeletePrevCollider(m_iSerialNumber,
			m_pPrevNumber[i]);

		CCollider*	pDest = GET_SINGLE(CCollisionManager)->FindCollider(m_pPrevNumber[i]);

		if (pDest)
			pDest->DeleteSerialNumber(m_iSerialNumber);

		m_pPrevNumber[i] = UINT_MAX;
	}

	m_iPrevSize = 0;
}

void CCollider::AddSerialNumber(unsigned int iSerialNumber)
{
	if (m_iPrevSize == m_iPrevCapacity)
	{
		m_iPrevCapacity *= 2;

		unsigned int* pList = new unsigned int[m_iPrevCapacity];
		memcpy(pList, m_pPrevNumber, sizeof(unsigned int) * m_iPrevSize);

		SAFE_DELETE_ARRAY(m_pPrevNumber);

		m_pPrevNumber = pList;
	}

	m_pPrevNumber[m_iPrevSize] = iSerialNumber;
	++m_iPrevSize;
}

void CCollider::DeleteSerialNumber(unsigned int iSerialNumber)
{
	for (unsigned int i = 0; i < m_iPrevSize; ++i)
	{
		if (m_pPrevNumber[i] == iSerialNumber)
		{
			--m_iPrevSize;
			for (unsigned int j = i; j < m_iPrevSize; ++j)
			{
				m_pPrevNumber[j] = m_pPrevNumber[j + 1];
			}

			m_pPrevNumber[m_iPrevSize] = UINT_MAX;
			break;
		}
	}
}

void CCollider::AddCollisionList(unsigned int iSerialNumber)
{
	if (m_iCollisionSize == m_iCollisionCapacity)
	{
		m_iCollisionCapacity *= 2;

		unsigned int* pList = new unsigned int[m_iCollisionCapacity];
		memcpy(pList, m_pCollisionList, sizeof(unsigned int) * m_iCollisionSize);

		SAFE_DELETE_ARRAY(m_pCollisionList);

		m_pCollisionList = pList;
	}

	m_pCollisionList[m_iCollisionSize] = iSerialNumber;
	++m_iCollisionSize;
}

void CCollider::DeleteCollisionList(unsigned int iSerialNumber)
{
	for (unsigned int i = 0; i < m_iCollisionSize; ++i)
	{
		if (m_pCollisionList[i] == iSerialNumber)
		{
			--m_iCollisionSize;
			for (unsigned int j = i; j < m_iCollisionSize; ++j)
			{
				m_pCollisionList[j] = m_pCollisionList[j + 1];
			}

			m_pCollisionList[m_iCollisionSize] = UINT_MAX;
			break;
		}
	}
}

void CCollider::ClearCollisionList()
{
	m_iCollisionSize = 0;
}

bool CCollider::CheckCollisionList(unsigned int iSerialNumber)
{
	for (unsigned int i = 0; i < m_iCollisionSize; ++i)
	{
		if (m_pCollisionList[i] == iSerialNumber)
			return true;
	}

	return false;
}

bool CCollider::CheckCollisionList()
{
	return m_iCollisionSize != 0;
}

void CCollider::Call(COLLISION_CALLBACK_STATE eState,
	CCollider* pDest, float fTime)
{
	if (m_bCallback[eState])
		m_Callback[eState](this, pDest, fTime);
}

void CCollider::Start()
{
	m_iSerialNumber = GET_SINGLE(CCollisionManager)->GetSerialNumber();

	if (m_bAddCollisionManager)
		GET_SINGLE(CCollisionManager)->AddCollider(this);

	m_iPrevSize = 0;
	m_iPrevCapacity = 10;
	m_pPrevNumber = new unsigned int[m_iPrevCapacity];

	m_iCollisionSize = 0;
	m_iCollisionCapacity = 10;
	m_pCollisionList = new unsigned int[m_iCollisionCapacity];
}

bool CCollider::Init()
{
	return true;
}

int CCollider::Input(float fTime)
{
	return 0;
}

int CCollider::Update(float fTime)
{
	return 0;
}

int CCollider::LateUpdate(float fTime)
{
	return 0;
}

int CCollider::Collision(float fTime)
{
	return 0;
}

int CCollider::PrevRender(float fTime)
{
	return 0;
}

int CCollider::Render(float fTime)
{
#ifdef _DEBUG
	if (m_iPrevSize > 0)
		m_vColor = Vector4::Red;

	else
		m_vColor = Vector4::Green;

	m_pWireFrame->SetState();

	GET_SINGLE(CShaderManager)->UpdateCBuffer("Transform", &m_tCBuffer);
	GET_SINGLE(CShaderManager)->UpdateCBuffer("Collider", &m_vColor);

	m_pShader->SetShader();
	_CONTEXT->IASetInputLayout(m_pLayout);

	m_pMesh->Render();


	m_pWireFrame->ResetState();
#endif // _DEBUG

	m_iCollisionSize = 0;

	return 0;
}

CCollider * CCollider::Clone() const
{
	return nullptr;
}

bool CCollider::CollisionSphereToSphere(const SphereInfo & tSrc, const SphereInfo & tDest)
{
	float	fDist = tSrc.vCenter.Distance(tDest.vCenter);
	return fDist <= tSrc.fRadius + tDest.fRadius;
}

bool CCollider::CollisionOBBToOBB(const OBBInfo & tSrc, 
	const OBBInfo & tDest)
{
	Vector3	vDir = tDest.vCenter - tSrc.vCenter;

	float	fAxisDot[3][3] = {};
	float	fAbsAxisDot[3][3] = {};
	float	fDirDot[3] = {};
	float	r, r1, r2;
	bool	bAxis = false;

	for (int i = 0; i < 3; ++i)
	{
		fAxisDot[AXIS_X][i] = tSrc.vAxis[AXIS_X].Dot(tDest.vAxis[i]);
		fAbsAxisDot[AXIS_X][i] = abs(fAxisDot[AXIS_X][i]);

		if (fAbsAxisDot[AXIS_X][i] > 0.99999f)
			bAxis = true;
	}

	// Center Dir을 Src의 X축에 내적한다.
	fDirDot[0] = vDir.Dot(tSrc.vAxis[AXIS_X]);
	r = abs(fDirDot[0]);
	r1 = tSrc.vHalfLength.x;
	r2 = tDest.vHalfLength.x * fAbsAxisDot[AXIS_X][AXIS_X] +
		tDest.vHalfLength.y * fAbsAxisDot[AXIS_X][AXIS_Y] +
		tDest.vHalfLength.z * fAbsAxisDot[AXIS_X][AXIS_Z];

	if (r > r1 + r2)
		return false;

	for (int i = 0; i < 3; ++i)
	{
		fAxisDot[AXIS_Y][i] = tSrc.vAxis[AXIS_Y].Dot(tDest.vAxis[i]);
		fAbsAxisDot[AXIS_Y][i] = abs(fAxisDot[AXIS_Y][i]);

		if (fAbsAxisDot[AXIS_Y][i] > 0.99999f)
			bAxis = true;
	}

	// Center Dir을 Src의 Y축에 내적한다.
	fDirDot[1] = vDir.Dot(tSrc.vAxis[AXIS_Y]);
	r = abs(fDirDot[1]);
	r1 = tSrc.vHalfLength.y;
	r2 = tDest.vHalfLength.x * fAbsAxisDot[AXIS_Y][AXIS_X] +
		tDest.vHalfLength.y * fAbsAxisDot[AXIS_Y][AXIS_Y] +
		tDest.vHalfLength.z * fAbsAxisDot[AXIS_Y][AXIS_Z];

	if (r > r1 + r2)
		return false;

	for (int i = 0; i < 3; ++i)
	{
		fAxisDot[AXIS_Z][i] = tSrc.vAxis[AXIS_Z].Dot(tDest.vAxis[i]);
		fAbsAxisDot[AXIS_Z][i] = abs(fAxisDot[AXIS_Z][i]);

		if (fAbsAxisDot[AXIS_Z][i] > 0.99999f)
			bAxis = true;
	}

	// Center Dir을 Src의 Z축에 내적한다.
	fDirDot[2] = vDir.Dot(tSrc.vAxis[AXIS_Z]);
	r = abs(fDirDot[2]);
	r1 = tSrc.vHalfLength.z;
	r2 = tDest.vHalfLength.x * fAbsAxisDot[AXIS_Z][AXIS_X] +
		tDest.vHalfLength.y * fAbsAxisDot[AXIS_Z][AXIS_Y] +
		tDest.vHalfLength.z * fAbsAxisDot[AXIS_Z][AXIS_Z];

	if (r > r1 + r2)
		return false;

	// Dest의 X축에 Center Dir을 내적한다.
	r = abs(vDir.Dot(tDest.vAxis[AXIS_X]));
	r1 = tSrc.vHalfLength.x * fAbsAxisDot[AXIS_X][AXIS_X] +
		tSrc.vHalfLength.y * fAbsAxisDot[AXIS_Y][AXIS_X] +
		tSrc.vHalfLength.z * fAbsAxisDot[AXIS_Z][AXIS_X];
	r2 = tDest.vHalfLength.x;

	if (r > r1 + r2)
		return false;

	// Dest의 Y축에 Center Dir을 내적한다.
	r = abs(vDir.Dot(tDest.vAxis[AXIS_Y]));
	r1 = tSrc.vHalfLength.x * fAbsAxisDot[AXIS_X][AXIS_Y] +
		tSrc.vHalfLength.y * fAbsAxisDot[AXIS_Y][AXIS_Y] +
		tSrc.vHalfLength.z * fAbsAxisDot[AXIS_Z][AXIS_Y];
	r2 = tDest.vHalfLength.y;

	if (r > r1 + r2)
		return false;

	// Dest의 Z축에 Center Dir을 내적한다.
	r = abs(vDir.Dot(tDest.vAxis[AXIS_Z]));
	r1 = tSrc.vHalfLength.x * fAbsAxisDot[AXIS_X][AXIS_Z] +
		tSrc.vHalfLength.y * fAbsAxisDot[AXIS_Y][AXIS_Z] +
		tSrc.vHalfLength.z * fAbsAxisDot[AXIS_Z][AXIS_Z];
	r2 = tDest.vHalfLength.z;

	if (r > r1 + r2)
		return false;

	if (bAxis)
		return true;

	r = abs(fDirDot[AXIS_Z] * fAxisDot[AXIS_Y][AXIS_X] -
		fDirDot[AXIS_Y] * fAxisDot[AXIS_Z][AXIS_X]);
	r1 = tSrc.vHalfLength.y * fAbsAxisDot[AXIS_Z][AXIS_X] +
		tSrc.vHalfLength.z * fAbsAxisDot[AXIS_Y][AXIS_X];
	r2 = tDest.vHalfLength.y * fAbsAxisDot[AXIS_X][AXIS_Z] +
		tDest.vHalfLength.z * fAbsAxisDot[AXIS_X][AXIS_Y];

	if (r > r1 + r2)
		return false;

	r = abs(fDirDot[AXIS_Z] * fAxisDot[AXIS_Y][AXIS_Y] -
		fDirDot[AXIS_Y] * fAxisDot[AXIS_Z][AXIS_Y]);
	r1 = tSrc.vHalfLength.y * fAbsAxisDot[AXIS_Z][AXIS_Y] +
		tSrc.vHalfLength.z * fAbsAxisDot[AXIS_Y][AXIS_Y];
	r2 = tDest.vHalfLength.x * fAbsAxisDot[AXIS_X][AXIS_Z] +
		tDest.vHalfLength.z * fAbsAxisDot[AXIS_X][AXIS_X];

	if (r > r1 + r2)
		return false;

	r = abs(fDirDot[AXIS_Z] * fAxisDot[AXIS_Y][AXIS_Z] -
		fDirDot[AXIS_Y] * fAxisDot[AXIS_Z][AXIS_Z]);
	r1 = tSrc.vHalfLength.y * fAbsAxisDot[AXIS_Z][AXIS_Z] +
		tSrc.vHalfLength.z * fAbsAxisDot[AXIS_Y][AXIS_Z];
	r2 = tDest.vHalfLength.x * fAbsAxisDot[AXIS_X][AXIS_Y] +
		tDest.vHalfLength.y * fAbsAxisDot[AXIS_X][AXIS_X];

	if (r > r1 + r2)
		return false;

	r = abs(fDirDot[AXIS_X] * fAxisDot[AXIS_Z][AXIS_X] -
		fDirDot[AXIS_Y] * fAxisDot[AXIS_Z][AXIS_Z]);
	r1 = tSrc.vHalfLength.x * fAbsAxisDot[AXIS_Z][AXIS_X] +
		tSrc.vHalfLength.z * fAbsAxisDot[AXIS_X][AXIS_X];
	r2 = tDest.vHalfLength.y * fAbsAxisDot[AXIS_Y][AXIS_Z] +
		tDest.vHalfLength.z * fAbsAxisDot[AXIS_Y][AXIS_Y];

	if (r > r1 + r2)
		return false;

	r = abs(fDirDot[AXIS_X] * fAxisDot[AXIS_Z][AXIS_Y] -
		fDirDot[AXIS_Z] * fAxisDot[AXIS_X][AXIS_Y]);
	r1 = tSrc.vHalfLength.x * fAbsAxisDot[AXIS_Z][AXIS_Y] +
		tSrc.vHalfLength.z * fAbsAxisDot[AXIS_X][AXIS_Y];
	r2 = tDest.vHalfLength.x * fAbsAxisDot[AXIS_Y][AXIS_Z] +
		tDest.vHalfLength.z * fAbsAxisDot[AXIS_Y][AXIS_X];

	if (r > r1 + r2)
		return false;

	r = abs(fDirDot[AXIS_X] * fAxisDot[AXIS_Z][AXIS_Z] -
		fDirDot[AXIS_Z] * fAxisDot[AXIS_X][AXIS_Z]);
	r1 = tSrc.vHalfLength.x * fAbsAxisDot[AXIS_Z][AXIS_Z] +
		tSrc.vHalfLength.z * fAbsAxisDot[AXIS_X][AXIS_Z];
	r2 = tDest.vHalfLength.x * fAbsAxisDot[AXIS_Y][AXIS_Y] +
		tDest.vHalfLength.y * fAbsAxisDot[AXIS_Y][AXIS_X];

	if (r > r1 + r2)
		return false;

	r = abs(fDirDot[AXIS_Y] * fAxisDot[AXIS_X][AXIS_X] -
		fDirDot[AXIS_X] * fAxisDot[AXIS_Y][AXIS_X]);
	r1 = tSrc.vHalfLength.x * fAbsAxisDot[AXIS_Y][AXIS_X] +
		tSrc.vHalfLength.y * fAbsAxisDot[AXIS_X][AXIS_X];
	r2 = tDest.vHalfLength.y * fAbsAxisDot[AXIS_Z][AXIS_Z] +
		tDest.vHalfLength.z * fAbsAxisDot[AXIS_Z][AXIS_Y];

	if (r > r1 + r2)
		return false;

	r = abs(fDirDot[AXIS_Y] * fAxisDot[AXIS_X][AXIS_Y] -
		fDirDot[AXIS_X] * fAxisDot[AXIS_Y][AXIS_Y]);
	r1 = tSrc.vHalfLength.x * fAbsAxisDot[AXIS_Y][AXIS_Y] +
		tSrc.vHalfLength.y * fAbsAxisDot[AXIS_X][AXIS_Y];
	r2 = tDest.vHalfLength.x * fAbsAxisDot[AXIS_Z][AXIS_Z] +
		tDest.vHalfLength.z * fAbsAxisDot[AXIS_Z][AXIS_X];

	if (r > r1 + r2)
		return false;

	r = abs(fDirDot[AXIS_Y] * fAxisDot[AXIS_X][AXIS_Z] -
		fDirDot[AXIS_X] * fAxisDot[AXIS_Y][AXIS_Z]);
	r1 = tSrc.vHalfLength.x * fAbsAxisDot[AXIS_Y][AXIS_Z] +
		tSrc.vHalfLength.y * fAbsAxisDot[AXIS_X][AXIS_Z];
	r2 = tDest.vHalfLength.x * fAbsAxisDot[AXIS_Z][AXIS_Y] +
		tDest.vHalfLength.y * fAbsAxisDot[AXIS_Z][AXIS_X];

	if (r > r1 + r2)
		return false;

	return true;
}


bool CCollider::CollisionSpeherToOBB(const SphereInfo & tShere, const OBBInfo & tOBB)
{
	Vector3 ClosePoint = {};
	Vector3 vDir = tOBB.vCenter - tShere.vCenter;
	Vector3 vShereCenter =
	{
		tOBB.vAxis[AXIS_X].Dot(vDir),
		tOBB.vAxis[AXIS_Y].Dot(vDir),
		tOBB.vAxis[AXIS_Z].Dot(vDir)
	};

	// x 축
	if (vShereCenter.x < -tOBB.vHalfLength.x)
		ClosePoint.x = -tOBB.vHalfLength.x;

	else if (vShereCenter.x > tOBB.vHalfLength.x)
		ClosePoint.x = tOBB.vHalfLength.x;
	else
		ClosePoint.x = vShereCenter.x;

	// y 축
	if (vShereCenter.y < -tOBB.vHalfLength.y)
		ClosePoint.y = -tOBB.vHalfLength.y;

	else if (vShereCenter.y > tOBB.vHalfLength.y)
		ClosePoint.y = tOBB.vHalfLength.y;
	else
		ClosePoint.y = vShereCenter.y;

	// z 축
	if (vShereCenter.z < -tOBB.vHalfLength.z)
		ClosePoint.z = -tOBB.vHalfLength.z;

	else if (vShereCenter.z > tOBB.vHalfLength.z)
		ClosePoint.z = tOBB.vHalfLength.z;
	else
		ClosePoint.z = vShereCenter.z;

	float fDistance = vShereCenter.Distance(ClosePoint);

	if (fDistance > tShere.fRadius)
		return false;

	return true;
}

bool CCollider::CollisionSphereToRay(const SphereInfo & tSrc, const RayInfo & tDest)
{
	Vector3	vM = tDest.vOrigin - tSrc.vCenter;

	float	b = 2.f * vM.Dot(tDest.vDir);
	float	c = vM.Dot(vM) - tSrc.fRadius * tSrc.fRadius;

	float	fDet = b * b - 4.f * c;

	// 판별식이 0보다 작을 경우 해는 허수이므로 불가.
	if (fDet < 0)
		return false;

	fDet = sqrtf(fDet);

	float	t0, t1;

	// +-에 대해 둘다 계산해본다.
	t0 = (-b + fDet) / 2.f;
	t1 = (-b - fDet) / 2.f;

	// 둘다 0보다 작으면 X.
	if (t0 < 0 && t1 < 0)
		return false;

	return true;
}

bool CCollider::CollisionAABBToRay(const AABBInfo & tAABB, const RayInfo & tRay)
{
	float tmin = (tAABB.vMin.x - tRay.vOrigin.x) / tRay.vDir.x;
	float tmax = (tAABB.vMax.x - tRay.vOrigin.x) / tRay.vDir.x;

	if (tmin > tmax) swap(tmin, tmax);

	float tymin = (tAABB.vMin.y - tRay.vOrigin.y) / tRay.vDir.y;
	float tymax = (tAABB.vMax.y - tRay.vOrigin.y) / tRay.vDir.y;

	if (tymin > tymax) swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (tAABB.vMin.z - tRay.vOrigin.z) / tRay.vDir.z;
	float tzmax = (tAABB.vMax.z - tRay.vOrigin.z) / tRay.vDir.z;

	if (tzmin > tzmax) swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;

	if (tzmin > tmin)
		tmin = tzmin;

	if (tzmax < tmax)
		tmax = tzmax;

	return true;
}
bool CCollider::CollisionOBBToRay(const OBBInfo & tOBB, const RayInfo & tRay)
{
	AABBInfo tAABB = {};
	RayInfo tNewRay = {};

	Matrix matRot = {};
	for (int i = 0; i < AXIS_END; ++i)
	{
		memcpy(&matRot[i][0], &(tOBB.vAxis[i]), sizeof(Vector3));
 	}
	matRot.Transpose();

	for (int i = 0; i < AXIS_END; ++i)
	{
		matRot[3][i] = -tOBB.vCenter.Dot(tOBB.vAxis[i]);
	}

	Vector3 vHalfLength = tOBB.vHalfLength;
	tAABB.vMax = vHalfLength;
	tAABB.vMin = vHalfLength * (-1.0f);


	tNewRay.vOrigin = tRay.vOrigin;
	tNewRay.vOrigin = tNewRay.vOrigin.TransformCoord(matRot);
	tNewRay.vDir = tRay.vDir;
	tNewRay.vDir = tNewRay.vDir.TransformNormal(matRot);

	return CollisionAABBToRay(tAABB, tNewRay);
}

void CCollider::SetCallback(COLLISION_CALLBACK_STATE eState, void(*pFunc)(CCollider *, CCollider *, float))
{
	m_bCallback[eState] = true;
	m_Callback[eState] = bind(pFunc, placeholders::_1, placeholders::_2, placeholders::_3);
}
