#include "Matrix.h"
#include "Vector3.h"

ENGINE_USING

_tagMatrix::_tagMatrix()
{
	/*
	1 0 0 0
	0 1 0 0
	0 0 1 0
	0 0 0 1
	*/
	// XMMatrixIdentity() : 항등행렬을 리턴해주는 함수이다.
	m = XMMatrixIdentity();
}

_tagMatrix::_tagMatrix(const _tagMatrix & _m)
{
	m = _m.m;
}

_tagMatrix::_tagMatrix(const XMMATRIX & _m)
{
	m = _m;
}

void _tagMatrix::operator=(const _tagMatrix & _m)
{
	m = _m.m;
}

void _tagMatrix::operator=(const XMMATRIX & _m)
{
	m = _m;
}

void _tagMatrix::operator=(float _f[4][4])
{
	for (int i = 0; i < 4; ++i)
	{
		v[i] = _f[i];
	}
}

_tagMatrix _tagMatrix::operator*(const _tagMatrix & _m)	const
{
	return _tagMatrix(m * _m.m);
}

_tagMatrix _tagMatrix::operator*(const XMMATRIX & _m)	const
{
	return _tagMatrix(m * _m);
}

_tagMatrix _tagMatrix::operator*(float f)	const
{
	return _tagMatrix(m * f);
}

_tagMatrix _tagMatrix::operator*(int i)	const
{
	return _tagMatrix(m * (float)i);
}

_tagMatrix & _tagMatrix::operator*=(const _tagMatrix & _m)
{
	m *= _m.m;
	return *this;
}

_tagMatrix & _tagMatrix::operator*=(const XMMATRIX & _m)
{
	m *= _m;
	return *this;
}

_tagMatrix & _tagMatrix::operator*=(float f)
{
	m *= f;
	return *this;
}

_tagMatrix & _tagMatrix::operator*=(int i)
{
	m *= (float)i;
	return *this;
}

XMMATRIX _tagMatrix::Identity()
{
	m = XMMatrixIdentity();
	return m;
}

XMMATRIX _tagMatrix::Transpose()
{
	m = XMMatrixTranspose(m);
	return m;
}

XMMATRIX _tagMatrix::Inverse()
{
	m = XMMatrixInverse(&XMMatrixDeterminant(m), m);
	return m;
}

XMMATRIX _tagMatrix::Scaling(float x, float y, float z)
{
	m = XMMatrixScaling(x, y, z);
	return m;
}

XMMATRIX _tagMatrix::Scaling(const _tagVector3 & vScale)
{
	m = XMMatrixScaling(vScale.x, vScale.y, vScale.z);
	return m;
}

XMMATRIX _tagMatrix::Rotation(float x, float y, float z)
{
	XMMATRIX	matRotX, matRotY, matRotZ;
	matRotX = XMMatrixRotationX(DegreeToRadian(x));
	matRotY = XMMatrixRotationY(DegreeToRadian(y));
	matRotZ = XMMatrixRotationZ(DegreeToRadian(z));
	m = matRotX * matRotY * matRotZ;
	return m;
}

XMMATRIX _tagMatrix::Rotation(const _tagVector3 & vRot)
{
	XMMATRIX	matRotX, matRotY, matRotZ;

	Vector3 Test = Vector3(DegreeToRadian(vRot.x), DegreeToRadian(vRot.y), DegreeToRadian(vRot.z));
	matRotX = XMMatrixRotationX(DegreeToRadian(vRot.x));
	matRotY = XMMatrixRotationY(DegreeToRadian(vRot.y));
	matRotZ = XMMatrixRotationZ(DegreeToRadian(vRot.z));
	m = matRotX * matRotY * matRotZ;
	return m;
}

XMMATRIX _tagMatrix::RotationX(float x)
{
	m = XMMatrixRotationX(DegreeToRadian(x));
	return m;
}

XMMATRIX _tagMatrix::RotationY(float y)
{
	m = XMMatrixRotationY(DegreeToRadian(y));
	return m;
}

XMMATRIX _tagMatrix::RotationZ(float z)
{
	m = XMMatrixRotationZ(DegreeToRadian(z));
	return m;
}

XMMATRIX _tagMatrix::RotationAxis(float fAngle,	const _tagVector3 & vAxis)
{
	m = XMMatrixRotationAxis(vAxis.Convert(), DegreeToRadian(fAngle));
	return m;
}

XMMATRIX _tagMatrix::Translation(float x, float y, float z)
{
	m = XMMatrixTranslation(x, y, z);
	return m;
}

XMMATRIX _tagMatrix::Translation(const _tagVector3 & vPos)
{
	m = XMMatrixTranslation(vPos.x, vPos.y, vPos.z);
	return m;
}

void * _tagMatrix::operator new(size_t iSize)
{
	return _aligned_malloc(iSize, 16);
}

void _tagMatrix::operator delete(void * p)
{
	_aligned_free(p);
}

Vector4 & _tagMatrix::operator[](unsigned int idx)
{
	return v[idx];
}

XMMATRIX _tagMatrix::WorldMatrixSet(const _tagVector3& vPos, const _tagVector3& vRot, const _tagVector3& vScale)
{
	Matrix matPos, matRot, matScale;
	matPos.Translation(vPos);
	matRot.Rotation(vRot);
	matScale.Scaling(vScale);

	*this = matScale * matRot * matPos;

	return this->m;
}

_tagVector3 _tagMatrix::GetPosition()const
{
	return _tagVector3(_41, _42, _43);
}
_tagVector3 _tagMatrix::GetRotate()const
{
	_tagVector3 vScale = GetScale();
	float r11, r21, r31, r32, r33;
	r11 = _11 / vScale.x;
	r21 = _12 / vScale.x;
	r31 = _13 / vScale.x;
	r32 = _23 / vScale.y;
	r33 = _33 / vScale.z;

	_tagVector3 vRot;
	vRot.x = atan2(r32, r33);
	vRot.y = atan2(-r31, sqrtf(r32 * r32 + r33*r33));
	vRot.z = atan2(r21, r11);
	return vRot;
}
_tagVector3 _tagMatrix::GetScale()const
{
	_tagVector3 vScale;
	vScale.x = sqrtf(_11*_11 + _12*_12 + _13*_13);
	vScale.y = sqrtf(_21*_21 + _22*_22 + _23*_23);
	vScale.z = sqrtf(_31*_31 + _32*_32 + _33*_33);

	const float fRounding = 10000;

	vScale *= fRounding;
	vScale.x = roundf(vScale.x);
	vScale.y = roundf(vScale.y);
	vScale.z = roundf(vScale.z);

	vScale *= 1 / fRounding;

	return vScale;
}