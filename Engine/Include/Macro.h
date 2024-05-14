
#pragma once

#ifdef ENGINE_EXPORT
#define	ENGINE_DLL	__declspec(dllexport)
#else
#define	ENGINE_DLL	__declspec(dllimport)
#endif // ENGINE_EXPORT

#define	ENGINE_BEGIN	namespace Engine {
#define	ENGINE_END		}
#define	ENGINE_USING	using namespace Engine;

#define	SAFE_DELETE(p)	if(p)	{ delete p; p = nullptr; }
#define	SAFE_DELETE_ARRAY(p)	if(p)	{ delete[] p; p = nullptr; }
#define	SAFE_RELEASE(p)	if(p)	{ p->Release(); p = nullptr; }

#define	DECLARE_SINGLE(Type)	\
private:\
	static Type* m_pInst;\
public:\
	static Type* GetInst()\
	{\
		if(!m_pInst)\
			m_pInst = new Type;\
		return m_pInst;\
	}\
	static void DestroyInst()\
	{\
		SAFE_DELETE(m_pInst);\
	}\
private:\
	Type();\
	~Type();

#define	DEFINITION_SINGLE(Type)	Type* Type::m_pInst = nullptr;

#define	GET_SINGLE(Type)		Type::GetInst()
#define	DESTROY_SINGLE(Type)	Type::DestroyInst()

#define	_DEVICE		GET_SINGLE(CDevice)->GetDevice()
#define	_CONTEXT	GET_SINGLE(CDevice)->GetContext()
#define	_RESOLUTION	GET_SINGLE(CDevice)->GetResolution()
#define	WINDOWINSTANCE	GET_SINGLE(CCore)->GetWindowInstance()
#define	WINDOWHANDLE	GET_SINGLE(CCore)->GetWindowHandle()
#define	RENDERTARGET2D	GET_SINGLE(CDevice)->Get2DRenderTarget()

#ifdef UNICODE
#define	SplitPath	_wsplitpath_s
#else
#define	SplitPath	_splitpath_s
#endif // UNICODE
