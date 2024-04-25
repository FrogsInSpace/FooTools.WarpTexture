#pragma once

#include "max.h"
#include "toneop.h"

class WarpContext : public ShadeContext
{
public:
	WarpContext(ShadeContext* const sc, const Point3& warp) :
		m_sc			(sc),
		m_warp			(warp)
	{
		mode			= sc->mode;
		doMaps			= sc->doMaps;
		filterMaps		= sc->filterMaps;
		shadow			= sc->shadow;
		backFace		= sc->backFace;
		mtlNum			= sc->mtlNum;
		ambientLight	= sc->ambientLight;
		nLights			= sc->nLights;
		rayLevel		= sc->rayLevel;
		xshadeID		= sc->xshadeID;
		atmosSkipLight	= sc->atmosSkipLight;
		globContext		= sc->globContext;
		out				= sc->out;
		m_toneOp = sc->globContext != NULL ? sc->globContext->pToneOp : NULL;
	}

	void			ResetOutput				(int n)										{ m_sc->ResetOutput(n); }	
	
	Class_ID		ClassID					()											{ return m_sc->ClassID(); }
	BOOL			InMtlEditor				()											{ return m_sc->InMtlEditor(); }
	int				Antialias				()											{ return m_sc->Antialias(); }
	int				ProjType				()											{ return m_sc->ProjType(); }
	LightDesc*		Light					(int n)										{ return m_sc->Light(n); }
	TimeValue		CurTime					()											{ return m_sc->CurTime(); }
	int				NodeID					()											{ return m_sc->NodeID(); }
	INode*			Node					()											{ return m_sc->Node(); }
	Object*			GetEvalObject			()											{ return m_sc->GetEvalObject(); }
	Point3			BarycentricCoords		()											{ return m_sc->BarycentricCoords(); }
	int				FaceNumber				()											{ return m_sc->FaceNumber(); }
	Point3			Normal					()											{ return m_sc->Normal(); }
	void			SetNormal				(Point3 p)									{ m_sc->SetNormal(p); }
	Point3			OrigNormal				()											{ return m_sc->OrigNormal(); }
	Point3			GNormal					()											{ return m_sc->GNormal(); }
	float			Curve					()											{ return m_sc->Curve(); }
	Point3			V						()											{ return m_sc->V(); }
	void			SetView					(Point3 p)									{ m_sc->SetView(p); }
	Point3			OrigView				()											{ return m_sc->OrigView(); }
	Point3			ReflectVector			()											{ return m_sc->ReflectVector(); }
	Point3			RefractVector			(float ior)									{ return m_sc->RefractVector(ior); }
	void			SetIOR					(float ior)									{ m_sc->SetIOR(ior); }
	float			GetIOR					()											{ return m_sc->GetIOR(); }
	Point3			CamPos					()											{ return m_sc->CamPos(); }
	inline Point3	P						();											// overridden
	Point3			DP						()											{ return m_sc->DP(); }
	void			DP						(Point3& dpdx, Point3& dpdy)				{ m_sc->DP(dpdx, dpdx); }
	inline Point3	PObj					();											// overridden
	Point3			DPObj					()											{ return m_sc->DPObj(); }
	Box3			ObjectBox				()											{ return m_sc->ObjectBox(); }
	inline Point3	PObjRelBox				();											// overridden
	Point3			DPObjRelBox				()											{ return m_sc->DPObjRelBox(); }
	inline void		ScreenUV				(Point2& uv, Point2 &duv);					// overridden
	IPoint2			ScreenCoord				()											{ return m_sc->ScreenCoord(); }
	Point2			SurfacePtScreen			()											{ return m_sc->SurfacePtScreen(); }

	inline Point3	UVW						(int channel);								// overridden
	Point3			DUVW					(int channel)								{ return m_sc->DUVW(channel); }
	void			DPdUVW					(Point3 dP[3], int channel)					{ m_sc->DPdUVW(dP, channel); }

	int				BumpBasisVectors		(Point3 dP[2], int axis, int channel)		{ return m_sc->BumpBasisVectors(dP, axis, channel); } 

	BOOL			IsSuperSampleOn			()											{ return m_sc->IsSuperSampleOn(); }
	BOOL			IsTextureSuperSampleOn	()											{ return m_sc->IsTextureSuperSampleOn(); }
	int				GetNSuperSample			()											{ return m_sc->GetNSuperSample(); }
	float			GetSampleSizeScale		()											{ return m_sc->GetSampleSizeScale(); }

	Point3			UVWNormal				(int channel)								{ return m_sc->UVWNormal(channel); }

	float			RayDiam					()											{ return m_sc->RayDiam(); }

	float			RayConeAngle			()											{ return m_sc->RayConeAngle(); } 

	AColor			EvalEnvironMap			(Texmap* map, Point3 view)					{ return m_sc->EvalEnvironMap(map, view); }
	void			GetBGColor				(Color& bgcol, Color& transp, BOOL fogBG)	{ m_sc->GetBGColor(bgcol, transp, fogBG); }

	float			CamNearRange			()											{ return m_sc->CamNearRange(); }
	float			CamFarRange				()											{ return m_sc->CamFarRange(); }

	Point3			PointTo					(const Point3& p, RefFrame ito)				{ return m_sc->PointTo(p, ito); }
	Point3			PointFrom				(const Point3& p, RefFrame ifrom)			{ return m_sc->PointFrom(p, ifrom); }
	Point3			VectorTo				(const Point3& p, RefFrame ito)				{ return m_sc->VectorTo(p, ito); }
	Point3			VectorFrom				(const Point3& p, RefFrame ifrom)			{ return m_sc->VectorFrom(p, ifrom); }
	Point3			VectorToNoScale			(const Point3& p, RefFrame ito)				{ return m_sc->VectorToNoScale(p, ito); }
	Point3			VectorFromNoScale		(const Point3& p, RefFrame ifrom)			{ return m_sc->VectorFromNoScale(p, ifrom); }

#if MAX_RELEASE_R26
	Matrix3			MatrixTo				(RefFrame ito)								{ return  m_sc->MatrixTo( ito );	}
	Matrix3			MatrixFrom				(RefFrame ifrom)							{ return  m_sc->MatrixFrom( ifrom ); }
#endif

	void			SetGBufferID			(int gbid)									{ m_sc->SetGBufferID(gbid); }

	FILE*			DebugFile				()											{ return m_sc->DebugFile(); }

	AColor			EvalGlobalEnvironMap	(Point3 dir)								{ return m_sc->EvalGlobalEnvironMap(dir); }

	BOOL			GetCache				(Texmap* map, AColor &c)					{ return m_sc->GetCache(map, c); }
	BOOL			GetCache				(Texmap* map, float &f)						{ return m_sc->GetCache(map, f); }
	BOOL			GetCache				(Texmap* map, Point3 &p)					{ return m_sc->GetCache(map, p); }
	void			PutCache				(Texmap* map, const AColor &c)				{ m_sc->PutCache(map, c); }
	void			PutCache				(Texmap* map, const float f)				{ m_sc->PutCache(map, f); }
	void			PutCache				(Texmap* map, const Point3 &p)				{ m_sc->PutCache(map, p); }
	void			TossCache				(Texmap* map)								{ m_sc->TossCache(map); }

	INT_PTR			Execute					(int cmd, ULONG_PTR arg1, ULONG_PTR arg2, ULONG_PTR arg3){ return m_sc->Execute(cmd, arg1, arg2, arg3); }

	LightDesc*		GetAtmosSkipLight		()											{ return m_sc->GetAtmosSkipLight(); }
	void			SetAtmosSkipLight		(LightDesc* lt)								{ m_sc->SetAtmosSkipLight(lt); }

	int				NRenderElements			()											{ return m_sc->NRenderElements(); }
	IRenderElement*	GetRenderElement		(int n)										{ return m_sc->GetRenderElement(n); }

	Color			DiffuseIllum			()											{ return m_sc->DiffuseIllum(); }

	template<class T> void ScaledToRGB		(T& energy) const							{ m_toneOp != NULL ? m_toneOp->ScaledToRGB(energy) : energy; }
	float			ScaledToRGB(float energy) const										{ return m_toneOp != NULL ? m_toneOp->ScaledToRGB(energy) : energy; }

	//void			ScaledToRGB				()											{ m_toneOp->ScaledToRGB();  }

	template<class T> void ScalePhysical	(T& energy) const							{ m_toneOp != NULL ? m_toneOp->ScalePhysical(energy) : energy; }
	float			ScalePhysical			(float energy) const						{ return m_toneOp != NULL ? m_toneOp->ScalePhysical(energy) : energy; }

	template<class T> void ScaleRGB			(T& energy) const							{ m_toneOp != NULL ? m_toneOp->ScaleRGB(energy) : energy; }
	float			ScaleRGB(float energy) const										{ return m_toneOp != NULL ? m_toneOp->ScaleRGB(energy) : energy; }
	

private:
	ShadeContext* const	m_sc;
	ToneOperator* m_toneOp;
	const Point3 m_warp;
};

inline Point3 WarpContext::PObjRelBox()
{
	return (m_sc->PObjRelBox() + m_warp);
}

inline void WarpContext::ScreenUV(Point2& uv, Point2 &duv)
{
	m_sc->ScreenUV(uv, duv);
	uv.x += m_warp.x;
	uv.y += m_warp.y;
}

inline Point3 WarpContext::P()
{
	return (m_sc->P() + m_warp);
}

inline Point3 WarpContext::PObj()
{
	return (m_sc->PObj() + m_warp);
}

inline Point3 WarpContext::UVW(int channel)
{
	return (m_sc->UVW(channel) + m_warp);
}

