#pragma once

#include "max.h"
#if (MAX_RELEASE >= 9000)
#include "maxheapdirect.h"
#endif
#include "iparamm2.h"

extern TCHAR* GetString(int id);

const Class_ID	kWarpTexture_ClassID(0x7e636608, 0xb9e6678);

enum
{
	kWarp_pblock_params,
	kWarp_pblock_count,
};

enum
{
	kWarp_tex_out,
	kWarp_target_texture,
	kWarp_warp_texture,
	kWarp_warp_amount,
};

///////////////////////////////////////////////////////////////////////////////////////////////////

class WarpTexture : public Texmap
{
public:
	WarpTexture();

	// Animatable /////////////////////////////////////////////////////////////////////////////////
#if MAX_RELEASE_R24
	void GetClassName(TSTR& s, BOOL localized = false);
#else
	void GetClassName(TSTR& s);
#endif

	Class_ID		ClassID();
	SClass_ID		SuperClassID();

	void			DeleteThis();

	int				NumSubs();

	Animatable*		SubAnim(int subNum);

#if MAX_RELEASE_R24
	TSTR			SubAnimName(int subNum, BOOL isLocalized);
#else
	TSTR			SubAnimName(int subNum);
#endif

	int				SubNumToRefNum(int subNum);

	int				NumParamBlocks();
	IParamBlock2*	GetParamBlock(int i);
	IParamBlock2*	GetParamBlockByID(BlockID id);

	// ReferenceTarget ////////////////////////////////////////////////////////////////////////////

	RefTargetHandle	Clone(RemapDir& remap);

	// ReferenceMaker /////////////////////////////////////////////////////////////////////////////

	int				NumRefs();
	RefTargetHandle	GetReference(int i);
	void			SetReference(int i, RefTargetHandle refTarg);

#if MAX_VERSION_MAJOR < 17 //Max 2015
	RefResult		NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message);
#else
	RefResult		NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate);
#endif

	// MtlBase ////////////////////////////////////////////////////////////////////////////////////

	void			Update(TimeValue t, Interval& valid);
	void			Reset();
	Interval		Validity(TimeValue t);

	IOResult		Save(ISave* isave);
	IOResult		Load(ILoad* iload);

	ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams* imp);

	// ISubMap ////////////////////////////////////////////////////////////////////////////////////

	int				NumSubTexmaps();
	Texmap*			GetSubTexmap(int i);
	void			SetSubTexmap(int i, Texmap* texmap);
	int				SubTexmapOn(int i);

	int				MapSlotType(int i);

#if MAX_RELEASE_R24
	TSTR			GetSubTexmapSlotName(int i, BOOL localized = false);
#else
	TSTR			GetSubTexmapSlotName(int i);
#endif

	BOOL			SetDlgThing(ParamDlg* paramDlg);

	// Texmap /////////////////////////////////////////////////////////////////////////////////////

	AColor			EvalColor(ShadeContext& sc);
	float			EvalMono(ShadeContext& sc);
	Point3			EvalNormalPerturb(ShadeContext& sc);

	bool			IsLocalOutputMeaningful(ShadeContext& sc);

private:
	void			Init();
	Point3			GetWarpVector(ShadeContext& sc);

	IParamBlock2* m_pblock;

	TextureOutput* m_texOut;
	static ParamDlg* m_texOutDlg;

	Texmap* m_targetTexture;
	Texmap* m_warpTexture;

	Interval			m_valid;

	// Cached stuff only valid during rendering
	float				m_warpAmount;
	bool				m_canRenderWarp;
};
