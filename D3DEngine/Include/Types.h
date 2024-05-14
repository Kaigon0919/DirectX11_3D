
#pragma once

#include "Flag.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"

ENGINE_BEGIN

typedef struct ENGINE_DLL _tagResolution
{
	int	iWidth;
	int	iHeight;
}Resolution, *PResolution;

// Color Vertex
typedef struct ENGINE_DLL _tagVertexColor
{
	Vector3		vPos;
	Vector4		vColor;

	_tagVertexColor()
	{
	}

	_tagVertexColor(const _tagVertexColor& vtx)
	{
		*this = vtx;
	}

	_tagVertexColor(const Vector3& vP, const Vector4& vC) :
		vPos(vP),
		vColor(vC)
	{
	}
}VertexColor, *PVertexColor;

// Color Normal Vertex
typedef struct ENGINE_DLL _tagVertexNormalColor
{
	Vector3		vPos;
	Vector3		vNormal;
	Vector4		vColor;

	_tagVertexNormalColor()
	{
	}

	_tagVertexNormalColor(const _tagVertexNormalColor& vtx)
	{
		*this = vtx;
	}

	_tagVertexNormalColor(const Vector3& vP,
		const Vector3& vN, const Vector4& vC) :
		vPos(vP),
		vNormal(vN),
		vColor(vC)
	{
	}
}VertexNormalColor, *PVertexNormalColor;

typedef struct ENGINE_DLL _tagVertexUV
{
	Vector3		vPos;
	Vector2		vUV;

	_tagVertexUV()
	{
	}

	_tagVertexUV(const _tagVertexUV& vtx)
	{
		*this = vtx;
	}

	_tagVertexUV(const Vector3& vP,
		const Vector2& _vUV) :
		vPos(vP),
		vUV(_vUV)
	{
	}
}VertexUV, *PVertexUV;

typedef struct ENGINE_DLL _tagVertexDefaultParticle
{
	Vector3		vPos;
	Vector2		vSize;

	_tagVertexDefaultParticle()
	{
	}

	_tagVertexDefaultParticle(const _tagVertexDefaultParticle& vtx)
	{
		*this = vtx;
	}

	_tagVertexDefaultParticle(const Vector3& vP,
		const Vector2& _vSize) :
		vPos(vP),
		vSize(_vSize)
	{
	}
}VertexDefaultParticle, *PVertexDefaultParticle;

typedef struct ENGINE_DLL _tagCBuffer
{
	string			strName;
	ID3D11Buffer*	pBuffer;
	void*	pData;
	int		iSize;
	int		iRegister;
	int		iConstant;
}CBuffer, *PCBuffer;

typedef struct ENGINE_DLL _tagTransformCBuffer
{
	Matrix	matWorldRot;
	Matrix	matWVRot;
	Matrix	matWorld;
	Matrix	matView;
	Matrix	matProj;
	Matrix	matInvProj;
	Matrix	matWV;
	Matrix	matWVP;
	Matrix	matVP;
}TransformCBuffer, *PTransformCBuffer;

typedef struct ENGINE_DLL _tagMaterial
{
	Vector4	vDif;
	Vector4	vAmb;
	Vector4	vSpc;
	Vector4	vEmv;
	int		iNormal;
	int		iSpecular;
	int		iSkinning;
	float	fEmpty;

	_tagMaterial() :
		vDif(Vector4::White),
		vAmb(Vector4::White),
		vSpc(Vector4::Black),
		vEmv(Vector4::Black),
		iNormal(0),
		iSpecular(0),
		iSkinning(0)
	{
		vSpc.w = 1.0f;
	}
}Material, *PMaterial;

typedef struct ENGINE_DLL _tagVertex3D
{
	Vector3	vPos;
	Vector3	vNormal;
	Vector2	vUV;
	Vector3	vTangent;
	Vector3	vBinormal;
	Vector4	vBlendWeights;
	Vector4	vBlendIndices;
}Vertex3D, *PVertex3D;

typedef struct ENGINE_DLL _tagLightInfo
{
	Vector4	vDif;
	Vector4	vAmb;
	Vector4	vSpc;
	Vector3	vPos;
	int		iType;
	Vector3	vDir;
	float	fDistance;
	float	fInAngle;
	float	fOutAngle;
	Vector2	vEmpty;

	_tagLightInfo() :
		vDif(Vector4::White),
		vAmb(Vector4(0.2f, 0.2f, 0.2f, 1.f)),
		vSpc(Vector4::White),
		iType(LT_DIR),
		fDistance(0.f),
		fInAngle(0.f),
		fOutAngle(0.f)
	{
	}
}LightInfo, *PLightInfo;

typedef struct ENGINE_DLL _tagRenderCBuffer
{
	int		iRenderMode;
	Vector3	vEmpty;
}RenderCBuffer, *PRenderCBuffer;

typedef struct ENGINE_DLL _tagDebugCBuffer
{
	Matrix	matWVP;
}DebugCBuffer, *PDebugCBuffer;

typedef struct ENGINE_DLL _tagParticleCBuffer
{
	Vector3	vParticlePos;
	float	fSizeX;
	Vector3	vAxisX;
	float	fSizeY;
	Vector3	vAxisY;
	float	fEmpty;
}ParticleCBuffer, *PParticleCBuffer;

typedef struct ENGINE_DLL _tagSphereInfo
{
	Vector3	vCenter;
	float	fRadius;

	_tagSphereInfo() :
		fRadius(0.f)
	{
	}
}SphereInfo, *PSphereInfo;

typedef struct ENGINE_DLL _tagAABBInfo
{
	Vector3	vMin;
	Vector3	vMax;

	_tagAABBInfo()
	{
	}
}AABBInfo, *PAABBInfo;

typedef struct ENGINE_DLL _tagOBBInfo
{
	Vector3	vCenter;
	Vector3	vAxis[AXIS_END];
	Vector3	vHalfLength;

	_tagOBBInfo()
	{
		for (int i = 0; i < AXIS_END; ++i)
		{
			vAxis[i] = Vector3::Axis[i];
			vHalfLength[i] = 1.f;
		}
	}
}OBBInfo, *POBBInfo;

typedef struct ENGINE_DLL _tagAnimationFrameCBuffer
{
	int	iAnimationFrameType;
	int	iAnimationOption;
	Vector2	vTextureSize;
	Vector2	vStart;
	Vector2	vEnd;
	int		iFrame;
	Vector3	vEmpty;
}AnimationFrameCBuffer, *PAnimationFrameCBuffer;

typedef struct ENGINE_DLL _tagLandScaleCBuffer
{
	float	fDetailLevelX;
	float	fDetailLevelZ;
	int		iSplatCount;
	float	fEmpty;
}LandScaleCBuffer, *PLandScaleCBuffer;

typedef struct ENGINE_DLL _tagButtonCBuffer
{
	Vector4	vColor;
}ButtonCBuffer, *PButtonCBuffer;

typedef struct ENGINE_DLL _tagRayInfo
{
	Vector3	vOrigin;
	Vector3	vDir;
}RayInfo, *PRayInfo;

typedef struct ENGINE_DLL _tagInstancingStaticBuffer
{
	Matrix	matWVP;
	Matrix	matWV;
	Matrix	matWVRot;
}InstancingStaticBuffer, *PInstancingStaticBuffer;

typedef struct ENGINE_DLL _tagInstancingAnimFrameBuffer
{
	Matrix	matWVP;
	Matrix	matVP;
	Vector2	vStart;
	Vector2	vEnd;
	Vector2	vSize;
	int		iFrame;
}InstancingAnimFrameBuffer, *PInstancingAnimFrameBuffer;

typedef struct ENGINE_DLL _tagInstancingGeometry
{
	bool	bAnimation;
	bool	bAnimationFrame;
	class CGameObject**	pObjList;
	int		iSize;
	int		iCapacity;

	void Resize()
	{
		if (iSize == iCapacity)
		{
			iCapacity *= 2;
			class CGameObject**	pList = new CGameObject*[iCapacity];

			memset(pList, 0, sizeof(CGameObject*) * iCapacity);
			memcpy(pList, pObjList, sizeof(CGameObject*) * iSize);

			SAFE_DELETE_ARRAY(pObjList);
			pObjList = pList;
		}
	}

	void Add(class CGameObject* pObj)
	{
		Resize();
		pObjList[iSize] = pObj;
		++iSize;
	}

	void Clear()
	{
		iSize = 0;
	}

	_tagInstancingGeometry()
	{
		iSize = 0;
		iCapacity = 100;
		bAnimation = false;
		bAnimationFrame = false;
		pObjList = new CGameObject*[iCapacity];
	}

	~_tagInstancingGeometry()
	{
		SAFE_DELETE_ARRAY(pObjList);
	}
}InstancingGeometry, *PInstancingGeometry;

typedef struct ENGINE_DLL _tagInstancingBuffer
{
	ID3D11Buffer*	pBuffer;
	void*			pData;
	int				iSize;
	int				iCount;
	D3D11_USAGE		eUsage;
}InstancingBuffer, *PInstancingBuffer;

ENGINE_END