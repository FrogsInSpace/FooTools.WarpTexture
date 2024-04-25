#include "WarpTexture.h"
#include "resource.h"
#include "AboutRollup.h"
#include "WarpContext.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

extern HINSTANCE hInstance;
extern TCHAR* GetString(int id);

///////////////////////////////////////////////////////////////////////////////////////////////////

enum
{
	kRef_tex_out,
	kRef_pblock,
	kRef_target_texture,
	kRef_warp_texture,
	kRef_count,
};

enum
{
	kSub_tex_out,
	kSub_pblock,
	kSub_target_texture,
	kSub_warp_texture,
	kSub_count,
};

enum
{
	kMap_target_texture,
	kMap_warp_texture,
	kMap_count,
};

///////////////////////////////////////////////////////////////////////////////////////////////////

class WarpTextureClassDesc : public ClassDesc2
{
public:
	int 			IsPublic()				{ return 1; }
	void*			Create(BOOL loading)	{ return new WarpTexture(); }
	const TCHAR*	ClassName()				{ return GetString(IDS_CLASS_NAME); }
#if MAX_VERSION_MAJOR >= 24
	const TCHAR* NonLocalizedClassName() { return ClassName(); }
#endif

	SClass_ID		SuperClassID()			{ return TEXMAP_CLASS_ID; }
	Class_ID		ClassID()				{ return kWarpTexture_ClassID; }
	const TCHAR* 	Category()				{ return TEXMAP_CAT_3D; }
	const TCHAR*	InternalName()			{ return _T("WarpTexture"); }
	HINSTANCE		HInstance()				{ return hInstance; }
};

static WarpTextureClassDesc warpTextureClassDesc;
ClassDesc2* GetWarpTextureClassDesc() { return &warpTextureClassDesc; }

///////////////////////////////////////////////////////////////////////////////////////////////////

#if MAX_VERSION_MAJOR >= 15
#define end p_end
#endif

static ParamBlockDesc2 warpTexturePBlockDesc
(
	kWarp_pblock_params, _T("parameters"),  0, &warpTextureClassDesc, P_AUTO_CONSTRUCT + P_AUTO_UI, kRef_pblock,
	IDD_WARP_TEXTURE, IDS_WARP_TEXTURE, 0, 0, NULL,

	kWarp_tex_out, _T("output"), TYPE_REFTARG, P_OWNERS_REF, IDS_OUTPUT,
		p_refno,	kRef_tex_out,
		end,

	kWarp_target_texture, _T("targetTexture"), TYPE_TEXMAP, P_SUBTEX, IDS_TARGET_TEXTURE,
		p_subtexno,	0,
		p_ui,		TYPE_TEXMAPBUTTON, IDC_TARGET_TEXTURE,
		end,

	kWarp_warp_texture, _T("warpTexture"), TYPE_TEXMAP, P_SUBTEX, IDS_WARP_TEXTURE,
		p_subtexno,	1,
		p_ui,		TYPE_TEXMAPBUTTON, IDC_WARP_TEXTURE,
		end,

	kWarp_warp_amount, _T("warpAmount"), TYPE_FLOAT, P_ANIMATABLE, IDS_WARP_AMOUNT,
		p_range,	-9999999.0f, 9999999.0f,
		p_default,	1.0f,
		p_ui,		TYPE_SPINNER, EDITTYPE_FLOAT, IDC_WARP_AMOUNT_EDIT, IDC_WARP_AMOUNT_SPIN, SPIN_AUTOSCALE,
		end,

	end
);

///////////////////////////////////////////////////////////////////////////////////////////////////

ParamDlg* WarpTexture::m_texOutDlg = NULL;

WarpTexture::WarpTexture() :
	m_pblock				(NULL),
	m_texOut				(NULL),
	m_targetTexture			(NULL),
	m_warpTexture			(NULL)
{
	warpTextureClassDesc.MakeAutoParamBlocks(this);
	assert(m_pblock);

	Init();
}

// Animatable /////////////////////////////////////////////////////////////////////////////////////
#if MAX_RELEASE_R24
void WarpTexture::GetClassName(TSTR& s, BOOL localized )
#else
void WarpTexture::GetClassName(TSTR& s)
#endif
{
	s = GetString(IDS_WARP_TEXTURE);
}

Class_ID WarpTexture::ClassID()
{
	return kWarpTexture_ClassID;
}

SClass_ID WarpTexture::SuperClassID()
{
	return TEXMAP_CLASS_ID;
}

void WarpTexture::DeleteThis()
{
	delete this;
}

int WarpTexture::NumSubs()
{
	return kSub_count;
}

Animatable* WarpTexture::SubAnim(int subNum)
{
	switch (subNum)
	{
		case kSub_tex_out:				return m_texOut;
		case kSub_pblock:				return m_pblock;
		case kSub_target_texture:		return m_targetTexture;
		case kSub_warp_texture:			return m_warpTexture;
		default:						return NULL;
	}
}

#if MAX_RELEASE_R24
TSTR WarpTexture::SubAnimName(int subNum, BOOL isLocalized)
#else
TSTR WarpTexture::SubAnimName(int subNum)
#endif
{
	switch (subNum)
	{
		case kSub_tex_out:				return GetString(IDS_OUTPUT);
		case kSub_pblock:				return GetString(IDS_PARAMS);
		case kSub_target_texture:		return GetString(IDS_TARGET_TEXTURE);
		case kSub_warp_texture:			return GetString(IDS_WARP_TEXTURE);
		default:						return _T("");
	}
}

int WarpTexture::SubNumToRefNum(int subNum)
{
	DbgAssert(NumRefs() == NumSubs());

	// NOTE: this is only valid as long as there's a one to one correspondence between refs and subanims!
	return subNum;
}

int WarpTexture::NumParamBlocks()
{
	return kWarp_pblock_count;
}

IParamBlock2* WarpTexture::GetParamBlock(int i)
{
	switch (i)
	{
		case kWarp_pblock_params:	return m_pblock;
		default:					return NULL;
	}
}

IParamBlock2* WarpTexture::GetParamBlockByID(BlockID id)
{
	switch (id)
	{
		case kWarp_pblock_params:	return m_pblock;
		default:					return NULL;
	}
}

// ReferenceTarget ////////////////////////////////////////////////////////////////////////////////

RefTargetHandle WarpTexture::Clone(RemapDir &remap)
{
	WarpTexture* newtex = new WarpTexture();

	*((MtlBase*)newtex) = *((MtlBase*)this); // copy superclass stuff
	newtex->ReplaceReference(kRef_tex_out, remap.CloneRef(m_texOut));
	newtex->ReplaceReference(kRef_pblock, remap.CloneRef(m_pblock));
	newtex->ReplaceReference(kRef_target_texture, remap.CloneRef(m_targetTexture));
	newtex->ReplaceReference(kRef_warp_texture, remap.CloneRef(m_warpTexture));
	newtex->m_valid.SetEmpty();
	BaseClone(this, newtex, remap);

	return static_cast<RefTargetHandle>(newtex);
}

// ReferenceMaker /////////////////////////////////////////////////////////////////////////////////

int WarpTexture::NumRefs()
{
	return kRef_count;
}

RefTargetHandle WarpTexture::GetReference(int i)
{
	switch (i)
	{
		case kRef_tex_out:				return m_texOut;
		case kRef_pblock:				return m_pblock;
		case kRef_target_texture:		return m_targetTexture;
		case kRef_warp_texture:			return m_warpTexture;
		default:						return NULL;
	}
}

void WarpTexture::SetReference(int i, RefTargetHandle refTarg)
{
	switch (i)
	{
		case kRef_tex_out:				m_texOut = static_cast<TextureOutput*>(refTarg); break;
		case kRef_pblock:				m_pblock = static_cast<IParamBlock2*>(refTarg); break;
		case kRef_target_texture:		m_targetTexture = static_cast<Texmap*>(refTarg); break;
		case kRef_warp_texture:			m_warpTexture = static_cast<Texmap*>(refTarg); break;
	}
}

#if MAX_VERSION_MAJOR < 17 //Max 2015
RefResult WarpTexture::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget,
	PartID& partID, RefMessage message)
#else
RefResult WarpTexture::NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget,
	PartID& partID, RefMessage message, BOOL propagate)
#endif
{
	switch (message)
	{
		case REFMSG_CHANGE:
		{
			m_valid.SetEmpty();
			if (hTarget == m_pblock)
			{
				ParamID changingParam = m_pblock->LastNotifyParamID();
				warpTexturePBlockDesc.InvalidateUI(changingParam);
			}
			break;
		}
	}

	return REF_SUCCEED;
}

// MtlBase ////////////////////////////////////////////////////////////////////////////////////////

void WarpTexture::Update(TimeValue t, Interval& valid)
{
	if (!m_valid.InInterval(t))
	{
		m_valid.SetInfinite();

		m_pblock->GetValidity(t, m_valid);

		if (m_texOut) m_texOut->Update(t, m_valid);
		if (m_targetTexture) m_targetTexture->Update(t, m_valid);
		if (m_warpTexture) m_warpTexture->Update(t, m_valid);

		m_warpAmount = m_pblock->GetFloat(kWarp_warp_amount, t);
		m_canRenderWarp = m_warpTexture && m_targetTexture;
	}

	valid &= m_valid;
}

void WarpTexture::Reset()
{
	warpTextureClassDesc.Reset(this, TRUE);

	DeleteReference(kRef_target_texture);
	DeleteReference(kRef_warp_texture);

	Init();
}

Interval WarpTexture::Validity(TimeValue t)
{
	Interval valid(FOREVER);
	Update(t, valid);
	return valid;
}

#define MTL_HDR_CHUNK 0x4000

IOResult WarpTexture::Save(ISave* isave)
{
	IOResult res;

	isave->BeginChunk(MTL_HDR_CHUNK);
	res = MtlBase::Save(isave);
	if (res != IO_OK) return res;
	isave->EndChunk();

	return IO_OK;
}

IOResult WarpTexture::Load(ILoad* iload)
{
	while (IO_OK == iload->OpenChunk())
	{
		switch (iload->CurChunkID())
		{
			case MTL_HDR_CHUNK:
			{
				IOResult res = MtlBase::Load(iload);
				if (res != IO_OK) return res;
				break;
			}
		}
		iload->CloseChunk();
	}

	return IO_OK;
}

ParamDlg* WarpTexture::CreateParamDlg(HWND hwMtlEdit, IMtlParams* imp)
{
	IAutoMParamDlg* masterDlg = warpTextureClassDesc.CreateParamDlgs(hwMtlEdit, imp, this);
	m_texOutDlg = m_texOut->CreateParamDlg(hwMtlEdit, imp);

	masterDlg->AddDlg(m_texOutDlg);

	imp->AddRollupPage(hInstance, MAKEINTRESOURCE(IDD_ABOUT), aboutDlgProc, _T("About"));

	return masterDlg;
}

// ISubMap ////////////////////////////////////////////////////////////////////////////////////////

int WarpTexture::NumSubTexmaps()
{
	return kMap_count;
}

Texmap* WarpTexture::GetSubTexmap(int i)
{
	switch (i)
	{
		case kMap_target_texture:		return m_targetTexture;
		case kMap_warp_texture:			return m_warpTexture;
	}
	return NULL;
}

void WarpTexture::SetSubTexmap(int i, Texmap* texmap)
{
	switch (i)
	{
		case kMap_target_texture:
		{
			ReplaceReference(kRef_target_texture, texmap);
			warpTexturePBlockDesc.InvalidateUI(kWarp_target_texture);
			break;
		}
		case kMap_warp_texture:
		{
			ReplaceReference(kRef_warp_texture, texmap);
			warpTexturePBlockDesc.InvalidateUI(kWarp_warp_texture);
			break;
		}
	}

	m_valid.SetEmpty();
}

int WarpTexture::MapSlotType(int i)
{
	return MAPSLOT_TEXTURE;
}

int WarpTexture::SubTexmapOn(int i)
{
	return 1;
}
#if MAX_RELEASE_R24
TSTR WarpTexture::GetSubTexmapSlotName(int i, BOOL localized)
#else
TSTR WarpTexture::GetSubTexmapSlotName(int i)
#endif
{
	TSTR slotName(_T(""));

	switch (i)
	{
		case kMap_target_texture:		slotName = GetString(IDS_TARGET_TEXTURE); break;
		case kMap_warp_texture:			slotName = GetString(IDS_WARP_TEXTURE); break;
	}

	return slotName;
}

BOOL WarpTexture::SetDlgThing(ParamDlg* paramDlg)
{
	if ((m_texOutDlg) && (paramDlg == m_texOutDlg))
	{
		m_texOutDlg->SetThing(m_texOut);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

// Texmap /////////////////////////////////////////////////////////////////////////////////////////

AColor WarpTexture::EvalColor(ShadeContext& sc)
{
#if _DEBUG
	IPoint2 screenCoord = sc.ScreenCoord();
	if (
		screenCoord.x == 300 &&
		screenCoord.y == 230
	) {
		int breakHere = 0;
	}
#endif

	if (gbufID) sc.SetGBufferID(gbufID);

	AColor output;
	if (m_canRenderWarp)
	{
		WarpContext warpContext(&sc, GetWarpVector(sc));
		output = m_targetTexture->EvalColor(warpContext);
	}
	else if (m_targetTexture)
	{
		output = m_targetTexture->EvalColor(sc);
	}
	else
	{
		output.Black();
	}

	return m_texOut->Filter(output);
}

float WarpTexture::EvalMono(ShadeContext& sc)
{
	if (gbufID) sc.SetGBufferID(gbufID);

	float output(0.0f);
	if (m_canRenderWarp)
	{
		WarpContext warpContext(&sc, GetWarpVector(sc));
		output = m_targetTexture->EvalMono(warpContext);
	}
	else if (m_targetTexture)
	{
		output = m_targetTexture->EvalMono(sc);
	}

	return m_texOut->Filter(output);
}

Point3 WarpTexture::EvalNormalPerturb(ShadeContext& sc)
{
	if (gbufID) sc.SetGBufferID(gbufID);

	Point3 output(Point3::Origin);
	if (m_canRenderWarp)
	{
		WarpContext warpContext(&sc, GetWarpVector(sc));
		output = m_targetTexture->EvalNormalPerturb(warpContext);
	}
	else if (m_targetTexture)
	{
		output = m_targetTexture->EvalNormalPerturb(sc);
	}

	return m_texOut->Filter(output);
}

bool WarpTexture::IsLocalOutputMeaningful(ShadeContext& sc)
{
	return false;
}

// WarpTexture ////////////////////////////////////////////////////////////////////////////////////

void WarpTexture::Init()
{
	if (m_texOut)
	{
		m_texOut->Reset();
	} else {
		ReplaceReference(kRef_tex_out, GetNewDefaultTextureOutput());
	}

	m_valid.SetEmpty();
	m_warpAmount = 0.0f;
	m_canRenderWarp = false;
}

// Get amount to warp for given shade point.
Point3 WarpTexture::GetWarpVector(ShadeContext& sc)
{
	AColor warpVector = m_warpTexture->EvalColor(sc);

	return Point3(	warpVector.r * m_warpAmount,
					warpVector.g * m_warpAmount,
					warpVector.b * m_warpAmount);
}
