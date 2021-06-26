#pragma once

class IMaterialVar;
class IClientRenderable;
enum OverrideType_t;
enum PreviewImageRetVal_t;
typedef unsigned short ModelInstanceHandle_t;
struct model_t;
struct virtualmodel_t;
struct Ray_t;
class CStudioHdr;
struct studiohdr_t;
struct mstudiomodel_t;
struct mstudiomesh_t;
struct mstudioboneweight_t;
enum MaterialVarFlags_t;
enum MaterialPropertyTypes_t;
enum MaterialVarType_t;
class IVModelInfo;
extern IVModelInfo *g_pVModelInfo;
typedef float vec_t;
void MatrixAngles(const matrix3x4_t& matrix, float *angles);
void QuaternionMatrix(const Quaternion &q, matrix3x4_t& matrix);
class RadianEuler
{
public:
	inline RadianEuler(void) { }
	inline RadianEuler(vec_t X, vec_t Y, vec_t Z) { x = X; y = Y; z = Z; }
	inline RadianEuler(Quaternion const &q) {
		auto QA = [](const Quaternion &q, RadianEuler &angles) -> void {

			auto MA = [](const matrix3x4_t &matrix, RadianEuler &angles) -> void {
				MatrixAngles(matrix, &angles.x);
				angles.Init(DEG2RAD(angles.z), DEG2RAD(angles.x), DEG2RAD(angles.y));
			};

			matrix3x4_t matrix;
			QuaternionMatrix(q, matrix);
			MA(matrix, angles);
		};

		QA(q, *this);
	};	// evil auto type promotion!!!
	inline RadianEuler(QAngle const &angles);	// evil auto type promotion!!!

	// Initialization
	inline void Init(vec_t ix = 0.0f, vec_t iy = 0.0f, vec_t iz = 0.0f) { x = ix; y = iy; z = iz; }

	//	conversion to qangle
	QAngle ToQAngle(void) const;
	bool IsValid() const;
	void Invalidate();

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	vec_t x, y, z;
};

class IMaterial
{
public:
	// Get the name of the material.  This is a full path to 
	// the vmt file starting from "hl2/materials" (or equivalent) without
	// a file extension.
	virtual const char *	GetName() const = 0;
	virtual const char *	GetTextureGroupName() const = 0;

	// Get the preferred size/bitDepth of a preview image of a material.
	// This is the sort of image that you would use for a thumbnail view
	// of a material, or in WorldCraft until it uses materials to render.
	// separate this for the tools maybe
	virtual PreviewImageRetVal_t GetPreviewImageProperties(int *width, int *height,
		ImageFormat *imageFormat, bool* isTranslucent) const = 0;

	// Get a preview image at the specified width/height and bitDepth.
	// Will do resampling if necessary.(not yet!!! :) )
	// Will do color format conversion. (works now.)
	virtual PreviewImageRetVal_t GetPreviewImage(unsigned char *data,
		int width, int height,
		ImageFormat imageFormat) const = 0;
	// 
	virtual int				GetMappingWidth() = 0;
	virtual int				GetMappingHeight() = 0;

	virtual int				GetNumAnimationFrames() = 0;

	// For material subrects (material pages).  Offset(u,v) and scale(u,v) are normalized to texture.
	virtual bool			InMaterialPage(void) = 0;
	virtual	void			GetMaterialOffset(float *pOffset) = 0;
	virtual void			GetMaterialScale(float *pScale) = 0;
	virtual IMaterial		*GetMaterialPage(void) = 0;

	// find a vmt variable.
	// This is how game code affects how a material is rendered.
	// The game code must know about the params that are used by
	// the shader for the material that it is trying to affect.
	virtual IMaterialVar *	FindVar(const char *varName, bool *found, bool complain = true) = 0;

	// The user never allocates or deallocates materials.  Reference counting is
	// used instead.  Garbage collection is done upon a call to 
	// IMaterialSystem::UncacheUnusedMaterials.
	virtual void			IncrementReferenceCount(void) = 0;
	virtual void			DecrementReferenceCount(void) = 0;

	inline void AddRef() { IncrementReferenceCount(); }
	inline void Release() { DecrementReferenceCount(); }

	// Each material is assigned a number that groups it with like materials
	// for sorting in the application.
	virtual int 			GetEnumerationID(void) const = 0;

	virtual void			GetLowResColorSample(float s, float t, float *color) const = 0;

	// This computes the state snapshots for this material
	virtual void			RecomputeStateSnapshots() = 0;

	// Are we translucent?
	virtual bool			IsTranslucent() = 0;

	// Are we alphatested?
	virtual bool			IsAlphaTested() = 0;

	// Are we vertex lit?
	virtual bool			IsVertexLit() = 0;

	// Gets the vertex format
	virtual VertexFormat_t	GetVertexFormat() const = 0;

	// returns true if this material uses a material proxy
	virtual bool			HasProxy(void) const = 0;

	virtual bool			UsesEnvCubemap(void) = 0;

	virtual bool			NeedsTangentSpace(void) = 0;

	virtual bool			NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
	virtual bool			NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;

	// returns true if the shader doesn't do skinning itself and requires
	// the data that is sent to it to be preskinned.
	virtual bool			NeedsSoftwareSkinning(void) = 0;

	// Apply constant color or alpha modulation
	virtual void			AlphaModulate(float alpha) = 0;
	virtual void			ColorModulate(float r, float g, float b) = 0;

	// Material Var flags...
	virtual void			SetMaterialVarFlag(MaterialVarFlags_t flag, bool on) = 0;
	virtual bool			GetMaterialVarFlag(MaterialVarFlags_t flag) const = 0;

	// Gets material reflectivity
	virtual void			GetReflectivity(Vector& reflect) = 0;

	// Gets material property flags
	virtual bool			GetPropertyFlag(MaterialPropertyTypes_t type) = 0;

	// Is the material visible from both sides?
	virtual bool			IsTwoSided() = 0;

	// Sets the shader associated with the material
	virtual void			SetShader(const char *pShaderName) = 0;

	// Can't be const because the material might have to precache itself.
	virtual int				GetNumPasses(void) = 0;

	// Can't be const because the material might have to precache itself.
	virtual int				GetTextureMemoryBytes(void) = 0;

	// Meant to be used with materials created using CreateMaterial
	// It updates the materials to reflect the current values stored in the material vars
	virtual void			Refresh() = 0;

	// GR - returns true is material uses lightmap alpha for blending
	virtual bool			NeedsLightmapBlendAlpha(void) = 0;

	// returns true if the shader doesn't do lighting itself and requires
	// the data that is sent to it to be prelighted
	virtual bool			NeedsSoftwareLighting(void) = 0;

	// Gets at the shader parameters
	virtual int				ShaderParamCount() const = 0;
	virtual void	**GetShaderParams(void) = 0;

	// Returns true if this is the error material you get back from IMaterialSystem::FindMaterial if
	// the material can't be found.
	virtual bool			IsErrorMaterial() const = 0;

	virtual void			SetUseFixedFunctionBakedLighting(bool bEnable) = 0;

	// Gets the current alpha modulation
	virtual float			GetAlphaModulation() = 0;
	virtual void			GetColorModulation(float *r, float *g, float *b) = 0;

	// Gets the morph format
	virtual MorphFormat_t	GetMorphFormat() const = 0;

	// fast find that stores the index of the found var in the string table in local cache
	virtual void *	FindVarFast(char const *pVarName, unsigned int *pToken) = 0;

	// Sets new VMT shader parameters for the material
	virtual void			SetShaderAndParams(void *pKeyValues) = 0;
	virtual const char *	GetShaderName() const = 0;

	virtual void			DeleteIfUnreferenced() = 0;

	virtual bool			IsSpriteCard() = 0;

	virtual void			CallBindProxy(void *proxyData) = 0;

	virtual void			RefreshPreservingMaterialVars() = 0;

	virtual bool			WasReloadedFromWhitelist() = 0;
};
class ITexture;
struct Rect_t;
typedef unsigned short MaterialVarSym_t;
typedef unsigned short UtlSymId_t;
class IMaterialVar
{
public:
	typedef unsigned long FourCC;

protected:
	// base data and accessors
	char* m_pStringVal;
	int m_intVal;
	Vector4D m_VecVal;

	// member data. total = 4 bytes
	uint8 m_Type : 4;
	uint8 m_nNumVectorComps : 3;
	uint8 m_bFakeMaterialVar : 1;
	uint8 m_nTempIndex;
	UtlSymId_t   m_Id;

public:
	// class factory methods
	static IMaterialVar* Create(IMaterial* pMaterial, char const* pKey, VMatrix const& matrix);
	static IMaterialVar* Create(IMaterial* pMaterial, char const* pKey, char const* pVal);
	static IMaterialVar* Create(IMaterial* pMaterial, char const* pKey, float* pVal, int numcomps);
	static IMaterialVar* Create(IMaterial* pMaterial, char const* pKey, float val);
	static IMaterialVar* Create(IMaterial* pMaterial, char const* pKey, int val);
	static IMaterialVar* Create(IMaterial* pMaterial, char const* pKey);
	static void Destroy(IMaterialVar* pVar);
	static MaterialVarSym_t	GetSymbol(char const* pName);
	static MaterialVarSym_t	FindSymbol(char const* pName);
	static bool SymbolMatches(char const* pName, MaterialVarSym_t symbol);
	static void DeleteUnreferencedTextures(bool enable);

	virtual ITexture		*GetTextureValue(void) = 0;

	virtual char const *	GetName(void) const = 0;
	virtual MaterialVarSym_t	GetNameAsSymbol() const = 0;

	virtual void			SetFloatValue(float val) = 0;

	virtual void			SetIntValue(int val) = 0;

	virtual void			SetStringValue(char const *val) = 0;
	virtual char const *	GetStringValue(void) const = 0;

	// Use FourCC values to pass app-defined data structures between
	// the proxy and the shader. The shader should ignore the data if
	// its FourCC type not correct.	
	virtual void			SetFourCCValue(FourCC type, void *pData) = 0;
	virtual void			GetFourCCValue(FourCC *type, void **ppData) = 0;

	// Vec (dim 2-4)
	virtual void			SetVecValue(float const* val, int numcomps) = 0;
	virtual void			SetVecValue(float x, float y) = 0;
	virtual void			SetVecValue(float x, float y, float z) = 0;
	virtual void			SetVecValue(float x, float y, float z, float w) = 0;
	virtual void			GetLinearVecValue(float *val, int numcomps) const = 0;

	// revisit: is this a good interface for textures?
	virtual void			SetTextureValue(ITexture *) = 0;

	virtual IMaterial *		GetMaterialValue(void) = 0;
	virtual void			SetMaterialValue(IMaterial *) = 0;

	virtual bool			IsDefined() const = 0;
	virtual void			SetUndefined() = 0;

	// Matrix
	virtual void			SetMatrixValue(VMatrix const& matrix) = 0;
	virtual const VMatrix  &GetMatrixValue() = 0;
	virtual bool			MatrixIsIdentity() const = 0;

	// Copy....
	virtual void			CopyFrom(IMaterialVar *pMaterialVar) = 0;

	virtual void			SetValueAutodetectType(char const *val) = 0;

	virtual IMaterial *		GetOwningMaterial() = 0;

	//set just 1 component
	virtual void			SetVecComponentValue(float fVal, int nComponent) = 0;

protected:
	virtual int				GetIntValueInternal(void) const = 0;
	virtual float			GetFloatValueInternal(void) const = 0;
	virtual float const*	GetVecValueInternal() const = 0;
	virtual void			GetVecValueInternal(float *val, int numcomps) const = 0;
	virtual int				VectorSizeInternal() const = 0;

public:

	FORCEINLINE MaterialVarType_t GetType(void) const
	{
		return (MaterialVarType_t)m_Type;
	}

	FORCEINLINE bool IsTexture() const
	{
		return m_Type == 3;
	}

	FORCEINLINE operator ITexture*()
	{
		return GetTextureValue();
	}

	// NOTE: Fast methods should only be called in thread-safe situations
	FORCEINLINE int GetIntValueFast(void) const
	{
		// Set methods for float and vector update this
		return m_intVal;
	}


	FORCEINLINE int VectorSizeFast() const
	{
		return m_nNumVectorComps;
	}

#ifdef FAST_MATERIALVAR_ACCESS
	FORCEINLINE int GetIntValue(void) const
	{
		return GetIntValueFast();
	}

	FORCEINLINE float GetFloatValue(void) const
	{
		return GetFloatValueFast();
	}

	FORCEINLINE float const* GetVecValue() const
	{
		return GetVecValueFast();
	}

	FORCEINLINE void GetVecValue(float *val, int numcomps) const
	{
		GetVecValueFast(val, numcomps);
	}

	FORCEINLINE int VectorSize() const
	{
		return VectorSizeFast();
	}
#else // !FAST_MATERIALVAR_ACCESS
	FORCEINLINE int GetIntValue(void) const
	{
		return GetIntValueInternal();
	}

	FORCEINLINE float GetFloatValue(void) const
	{
		return GetFloatValueInternal();
	}

	FORCEINLINE float const* GetVecValue() const
	{
		return GetVecValueInternal();
	}

	FORCEINLINE void GetVecValue(float *val, int numcomps) const
	{
		return GetVecValueInternal(val, numcomps);
	}

	FORCEINLINE int VectorSize() const
	{
		return VectorSizeInternal();
	}
#endif

private:
	FORCEINLINE void SetTempIndex(int nIndex)
	{
		m_nTempIndex = nIndex;
	}

	friend void EnableThreadedMaterialVarAccess(bool bEnable, IMaterialVar **ppParams, int nVarCount);
};

struct ModelRenderInfo_t
{
	Vector origin;
	QAngle angles;
	IClientRenderable *pRenderable;
	const model_t *pModel;
	matrix3x4_t *pModelToWorld;
	const matrix3x4_t *pLightingOffset;
	const Vector *pLightingOrigin;
	int flags;
	int entity_index;
	int skin;
	int body;
	int hitboxset;
	ModelInstanceHandle_t instance;

	ModelRenderInfo_t()
	{
		pModelToWorld = NULL;
		pLightingOffset = NULL;
		pLightingOrigin = NULL;
	}
};
struct ClientModelRenderInfo_t : public ModelRenderInfo_t
{
	// Added space for lighting origin override. Just allocated space, need to set base pointer
	matrix3x4_t lightingOffset;

	// Added space for model to world matrix. Just allocated space, need to set base pointer
	matrix3x4_t modelToWorld;
};
struct mstudiobbox_t
{
	int bone;
	int group;
	Vector bbmin;
	Vector bbmax;
	int szhitboxnameindex;
	int					unused[8];

	const char* pszHitboxName()
	{
		if (szhitboxnameindex == 0)
			return "";

		return ((const char*)this) + szhitboxnameindex;
	}

	mstudiobbox_t() {}

};
struct mstudiohitboxset_t
{
	int					sznameindex;
	inline char * const	pszName(void) const { return ((char *)this) + sznameindex; }
	int					numhitboxes;
	int					hitboxindex;
	inline mstudiobbox_t *pHitbox(int i) const { return (mstudiobbox_t *)(((byte *)this) + hitboxindex) + i; };
};
struct mstudiobone_t
{
	int sznameindex;
	inline char * const GetName(void) const { return ((char *)this) + sznameindex; }
	int parent;
	int bonecontroller[6];
	Vector pos;
	Quaternion quat;
	RadianEuler rot;
	Vector posscale;
	Vector rotscale;
	matrix3x4_t poseToBone;
	Quaternion qAlignment;
	int	flags;
	int	proctype;
	int	procindex;			  // procedural rule
	mutable int	physicsbone;	// index into physically simulated bone
	inline void * GetProcedure() const { if (procindex == 0) return NULL; else return  (void *)(((byte *)this) + procindex); };
	int	surfacepropidx; // index into string tablefor property name
	inline char * const GetSurfaceProps(void) const { return ((char *)this) + surfacepropidx; }
	int contents;			   // See BSPFlags.h for the contents flags
	int	unused[8];			  // remove as appropriate
};
struct mstudiobonecontroller_t
{
	int					bone;	// -1 == 0
	int					type;	// X, Y, Z, XR, YR, ZR, M
	float				start;
	float				end;
	int					rest;	// byte index value at rest
	int					inputfield;	// 0-3 user set controller, 4 mouth
	int					unused[8];
};
struct mstudiomovement_t
{
	int					endframe;
	int					motionflags;
	float				v0;			// velocity at start of block
	float				v1;			// velocity at end of block
	float				angle;		// YAW rotation at end of this blocks movement
	Vector				vector;		// movement vector relative to this blocks initial angle
	Vector				position;	// relative to start of animation???

	mstudiomovement_t() {}
private:
	// No copy constructors allowed
	mstudiomovement_t(const mstudiomovement_t& vOther);
};
union mstudioanimvalue_t
{
	struct
	{
		byte	valid;
		byte	total;
	} num;
	short		value;
};
struct mstudioanim_valueptr_t
{
	short	offset[3];
	inline mstudioanimvalue_t *pAnimvalue(int i) const { if (offset[i] > 0) return  (mstudioanimvalue_t *)(((byte *)this) + offset[i]); else return NULL; };
};
#define STUDIO_ANIM_RAWPOS	0x01 // Vector48
#define STUDIO_ANIM_RAWROT	0x02 // Quaternion48
#define STUDIO_ANIM_ANIMPOS	0x04 // mstudioanim_valueptr_t
#define STUDIO_ANIM_ANIMROT	0x08 // mstudioanim_valueptr_t
#define STUDIO_ANIM_DELTA	0x10
#define STUDIO_ANIM_RAWROT2	0x20 // Quaternion64



class Quaternion48
{
public:
	// Construction/destruction:
	Quaternion48(void);
	Quaternion48(vec_t X, vec_t Y, vec_t Z);

	// assignment
	// Quaternion& operator=(const Quaternion48 &vOther);
	Quaternion48& operator=(const Quaternion &vOther);
	operator Quaternion ();
private:
	unsigned short x : 16;
	unsigned short y : 16;
	unsigned short z : 15;
	unsigned short wneg : 1;
};


inline Quaternion48::operator Quaternion ()
{
	Quaternion tmp;

	tmp.x = ((int)x - 32768) * (1 / 32768.0);
	tmp.y = ((int)y - 32768) * (1 / 32768.0);
	tmp.z = ((int)z - 16384) * (1 / 16384.0);
	tmp.w = sqrt(1 - tmp.x * tmp.x - tmp.y * tmp.y - tmp.z * tmp.z);
	if (wneg)
		tmp.w = -tmp.w;
	return tmp;
}
#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

inline Quaternion48& Quaternion48::operator=(const Quaternion &vOther)
{
	x = clamp((int)(vOther.x * 32768) + 32768, 0, 65535);
	y = clamp((int)(vOther.y * 32768) + 32768, 0, 65535);
	z = clamp((int)(vOther.z * 16384) + 16384, 0, 32767);
	wneg = (vOther.w < 0);
	return *this;
}

class Quaternion64
{
public:
	// Construction/destruction:
	Quaternion64(void);
	Quaternion64(vec_t X, vec_t Y, vec_t Z);

	// assignment
	// Quaternion& operator=(const Quaternion64 &vOther);
	Quaternion64& operator=(const Quaternion &vOther);
	operator Quaternion ();
private:
	uint64 x : 21;
	uint64 y : 21;
	uint64 z : 21;
	uint64 wneg : 1;
};

inline Quaternion64::operator Quaternion ()
{
	Quaternion tmp;

	// shift to -1048576, + 1048575, then round down slightly to -1.0 < x < 1.0
	tmp.x = ((int)x - 1048576) * (1 / 1048576.5f);
	tmp.y = ((int)y - 1048576) * (1 / 1048576.5f);
	tmp.z = ((int)z - 1048576) * (1 / 1048576.5f);
	tmp.w = sqrt(1 - tmp.x * tmp.x - tmp.y * tmp.y - tmp.z * tmp.z);
	if (wneg)
		tmp.w = -tmp.w;
	return tmp;
}

inline Quaternion64& Quaternion64::operator=(const Quaternion &vOther)
{

	x = clamp((int)(vOther.x * 1048576) + 1048576, 0, 2097151);
	y = clamp((int)(vOther.y * 1048576) + 1048576, 0, 2097151);
	z = clamp((int)(vOther.z * 1048576) + 1048576, 0, 2097151);
	wneg = (vOther.w < 0);
	return *this;
}
const int float32bias = 127;
const int float16bias = 15;

const float maxfloat16bits = 65504.0f;
class float16
{
public:
	//float16() {}
	//float16( float f ) { m_storage.rawWord = ConvertFloatTo16bits(f); }

	void Init() { m_storage.rawWord = 0; }
	//	float16& operator=(const float16 &other) { m_storage.rawWord = other.m_storage.rawWord; return *this; }
	//	float16& operator=(const float &other) { m_storage.rawWord = ConvertFloatTo16bits(other); return *this; }
	//	operator unsigned short () { return m_storage.rawWord; }
	//	operator float () { return Convert16bitFloatTo32bits( m_storage.rawWord ); }
	unsigned short GetBits() const
	{
		return m_storage.rawWord;
	}
	float GetFloat() const
	{
		return Convert16bitFloatTo32bits(m_storage.rawWord);
	}
	void SetFloat(float in)
	{
		m_storage.rawWord = ConvertFloatTo16bits(in);
	}

	bool IsInfinity() const
	{
		return m_storage.bits.biased_exponent == 31 && m_storage.bits.mantissa == 0;
	}
	bool IsNaN() const
	{
		return m_storage.bits.biased_exponent == 31 && m_storage.bits.mantissa != 0;
	}

	bool operator==(const float16 other) const { return m_storage.rawWord == other.m_storage.rawWord; }
	bool operator!=(const float16 other) const { return m_storage.rawWord != other.m_storage.rawWord; }

	//	bool operator< (const float other) const	   { return GetFloat() < other; }
	//	bool operator> (const float other) const	   { return GetFloat() > other; }

protected:
	union float32bits
	{
		float rawFloat;
		struct
		{
			unsigned int mantissa : 23;
			unsigned int biased_exponent : 8;
			unsigned int sign : 1;
		} bits;
	};

	union float16bits
	{
		unsigned short rawWord;
		struct
		{
			unsigned short mantissa : 10;
			unsigned short biased_exponent : 5;
			unsigned short sign : 1;
		} bits;
	};

	static bool IsNaN(float16bits in)
	{
		return in.bits.biased_exponent == 31 && in.bits.mantissa != 0;
	}
	static bool IsInfinity(float16bits in)
	{
		return in.bits.biased_exponent == 31 && in.bits.mantissa == 0;
	}

	// 0x0001 - 0x03ff
	static unsigned short ConvertFloatTo16bits(float input)
	{
		if (input > maxfloat16bits)
			input = maxfloat16bits;
		else if (input < -maxfloat16bits)
			input = -maxfloat16bits;

		float16bits output;
		float32bits inFloat;

		inFloat.rawFloat = input;

		output.bits.sign = inFloat.bits.sign;

		if ((inFloat.bits.biased_exponent == 0) && (inFloat.bits.mantissa == 0))
		{
			// zero
			output.bits.mantissa = 0;
			output.bits.biased_exponent = 0;
		}
		else if ((inFloat.bits.biased_exponent == 0) && (inFloat.bits.mantissa != 0))
		{
			// denorm -- denorm float maps to 0 half
			output.bits.mantissa = 0;
			output.bits.biased_exponent = 0;
		}
		else if ((inFloat.bits.biased_exponent == 0xff) && (inFloat.bits.mantissa == 0))
		{
#if 0
			// infinity
			output.bits.mantissa = 0;
			output.bits.biased_exponent = 31;
#else
			// infinity maps to maxfloat
			output.bits.mantissa = 0x3ff;
			output.bits.biased_exponent = 0x1e;
#endif
		}
		else if ((inFloat.bits.biased_exponent == 0xff) && (inFloat.bits.mantissa != 0))
		{
#if 0
			// NaN
			output.bits.mantissa = 1;
			output.bits.biased_exponent = 31;
#else
			// NaN maps to zero
			output.bits.mantissa = 0;
			output.bits.biased_exponent = 0;
#endif
		}
		else
		{
			// regular number
			int new_exp = inFloat.bits.biased_exponent - 127;

			if (new_exp < -24)
			{
				// this maps to 0
				output.bits.mantissa = 0;
				output.bits.biased_exponent = 0;
			}

			if (new_exp < -14)
			{
				// this maps to a denorm
				output.bits.biased_exponent = 0;
				unsigned int exp_val = (unsigned int)(-14 - (inFloat.bits.biased_exponent - float32bias));
				if (exp_val > 0 && exp_val < 11)
				{
					output.bits.mantissa = (1 << (10 - exp_val)) + (inFloat.bits.mantissa >> (13 + exp_val));
				}
			}
			else if (new_exp > 15)
			{
#if 0
				// map this value to infinity
				output.bits.mantissa = 0;
				output.bits.biased_exponent = 31;
#else
				// to big. . . maps to maxfloat
				output.bits.mantissa = 0x3ff;
				output.bits.biased_exponent = 0x1e;
#endif
			}
			else
			{
				output.bits.biased_exponent = new_exp + 15;
				output.bits.mantissa = (inFloat.bits.mantissa >> 13);
			}
		}
		return output.rawWord;
	}

	static float Convert16bitFloatTo32bits(unsigned short input)
	{
		float32bits output;
		const float16bits &inFloat = *((float16bits *)&input);

		if (IsInfinity(inFloat))
		{
			return maxfloat16bits * ((inFloat.bits.sign == 1) ? -1.0f : 1.0f);
		}
		if (IsNaN(inFloat))
		{
			return 0.0;
		}
		if (inFloat.bits.biased_exponent == 0 && inFloat.bits.mantissa != 0)
		{
			// denorm
			const float half_denorm = (1.0f / 16384.0f); // 2^-14
			float mantissa = ((float)(inFloat.bits.mantissa)) / 1024.0f;
			float sgn = (inFloat.bits.sign) ? -1.0f : 1.0f;
			output.rawFloat = sgn * mantissa*half_denorm;
		}
		else
		{
			// regular number
			unsigned mantissa = inFloat.bits.mantissa;
			unsigned biased_exponent = inFloat.bits.biased_exponent;
			unsigned sign = ((unsigned)inFloat.bits.sign) << 31;
			biased_exponent = ((biased_exponent - float16bias + float32bias) * (biased_exponent != 0)) << 23;
			mantissa <<= (23 - 10);

			*((unsigned *)&output) = (mantissa | biased_exponent | sign);
		}

		return output.rawFloat;
	}


	float16bits m_storage;
};
class Vector48
{
public:
	// Construction/destruction:
	Vector48(void) {}
	Vector48(vec_t X, vec_t Y, vec_t Z) { x.SetFloat(X); y.SetFloat(Y); z.SetFloat(Z); }

	// assignment
	Vector48& operator=(const Vector &vOther);
	operator Vector ();

	const float operator[](int i) const { return (((float16 *)this)[i]).GetFloat(); }

	float16 x;
	float16 y;
	float16 z;
};

inline Vector48& Vector48::operator=(const Vector &vOther)
{

	x.SetFloat(vOther.m_x);
	y.SetFloat(vOther.m_y);
	z.SetFloat(vOther.m_z);
	return *this;
}


inline Vector48::operator Vector ()
{
	Vector tmp;

	tmp.m_x = x.GetFloat();
	tmp.m_y = y.GetFloat();
	tmp.m_z = z.GetFloat();

	return tmp;
}

struct mstudioanim_t
{
	byte				bone;
	byte				flags;		// weighing options

	// valid for animating data only
	inline byte				*pData(void) const { return (((byte *)this) + sizeof(struct mstudioanim_t)); };
	inline mstudioanim_valueptr_t	*pRotV(void) const { return (mstudioanim_valueptr_t *)(pData()); };
	inline mstudioanim_valueptr_t	*pPosV(void) const { return (mstudioanim_valueptr_t *)(pData()) + ((flags & STUDIO_ANIM_ANIMROT) != 0); };

	// valid if animation unvaring over timeline
	inline Quaternion48		*pQuat48(void) const { return (Quaternion48 *)(pData()); };
	inline Quaternion64		*pQuat64(void) const { return (Quaternion64 *)(pData()); };
	inline Vector48			*pPos(void) const { return (Vector48 *)(pData() + ((flags & STUDIO_ANIM_RAWROT) != 0) * sizeof(*pQuat48()) + ((flags & STUDIO_ANIM_RAWROT2) != 0) * sizeof(*pQuat64())); };

	short				nextoffset;
	inline mstudioanim_t	*pNext(void) const { if (nextoffset != 0) return  (mstudioanim_t *)(((byte *)this) + nextoffset); else return NULL; };
};
struct mstudiocompressedikerror_t
{
	float	scale[6];
	short	offset[6];
	inline mstudioanimvalue_t *pAnimvalue(int i) const { if (offset[i] > 0) return  (mstudioanimvalue_t *)(((byte *)this) + offset[i]); else return NULL; };
	mstudiocompressedikerror_t() {}

private:
	// No copy constructors allowed
	mstudiocompressedikerror_t(const mstudiocompressedikerror_t& vOther);
};
struct mstudioikerror_t
{
	Vector		pos;
	Quaternion	q;

	mstudioikerror_t() {}

private:
	// No copy constructors allowed
	mstudioikerror_t(const mstudioikerror_t& vOther);
};
struct mstudioikrule_t
{
	int			index;

	int			type;
	int			chain;

	int			bone;

	int			slot;	// iktarget slot.  Usually same as chain.
	float		height;
	float		radius;
	float		floor;
	Vector		pos;
	Quaternion	q;

	int			compressedikerrorindex;
	inline mstudiocompressedikerror_t *pCompressedError() const { return (mstudiocompressedikerror_t *)(((byte *)this) + compressedikerrorindex); };
	int			unused2;

	int			iStart;
	int			ikerrorindex;
	inline mstudioikerror_t *pError(int i) const { return  (ikerrorindex) ? (mstudioikerror_t *)(((byte *)this) + ikerrorindex) + (i - iStart) : NULL; };

	float		start;	// beginning of influence
	float		peak;	// start of full influence
	float		tail;	// end of full influence
	float		end;	// end of all influence

	float		unused3;	// 
	float		contact;	// frame footstep makes ground concact
	float		drop;		// how far down the foot should drop when reaching for IK
	float		top;		// top of the foot box

	int			unused6;
	int			unused7;
	int			unused8;

	int			szattachmentindex;		// name of world attachment
	inline char * const pszAttachment(void) const { return ((char *)this) + szattachmentindex; }

	int			unused[7];

	mstudioikrule_t() {}

private:
	// No copy constructors allowed
	mstudioikrule_t(const mstudioikrule_t& vOther);
};
struct mstudiolocalhierarchy_t
{
	int			iBone;			// bone being adjusted
	int			iNewParent;		// the bones new parent

	float		start;			// beginning of influence
	float		peak;			// start of full influence
	float		tail;			// end of full influence
	float		end;			// end of all influence

	int			iStart;			// first frame 

	int			localanimindex;
	inline mstudiocompressedikerror_t *pLocalAnim() const { return (mstudiocompressedikerror_t *)(((byte *)this) + localanimindex); };

	int			unused[4];
};

struct mstudioanimsections_t
{
	int					animblock;
	int					animindex;
};

struct mstudioanimdesc_t
{
	int					baseptr;
	inline studiohdr_t	*pStudiohdr(void) const { return (studiohdr_t *)(((byte *)this) + baseptr); }

	int					sznameindex;
	inline char * const pszName(void) const { return ((char *)this) + sznameindex; }

	float				fps;		// frames per second	
	int					flags;		// looping/non-looping flags

	int					numframes;

	// piecewise movement
	int					nummovements;
	int					movementindex;
	inline mstudiomovement_t * const pMovement(int i) const { return (mstudiomovement_t *)(((byte *)this) + movementindex) + i; };

	int					unused1[6];			// remove as appropriate (and zero if loading older versions)	

	int					animblock;
	int					animindex;	 // non-zero when anim data isn't in sections
	mstudioanim_t *pAnimBlock(int block, int index) const; // returns pointer to a specific anim block (local or external)
	mstudioanim_t *pAnim(int *piFrame, float &flStall) const; // returns pointer to data and new frame index
	mstudioanim_t *pAnim(int *piFrame) const; // returns pointer to data and new frame index

	int					numikrules;
	int					ikruleindex;	// non-zero when IK data is stored in the mdl
	int					animblockikruleindex; // non-zero when IK data is stored in animblock file
	mstudioikrule_t *pIKRule(int i) const;

	int					numlocalhierarchy;
	int					localhierarchyindex;
	mstudiolocalhierarchy_t *pHierarchy(int i) const;

	int					sectionindex;
	int					sectionframes; // number of frames used in each fast lookup section, zero if not used
	inline mstudioanimsections_t * const pSection(int i) const { return (mstudioanimsections_t *)(((byte *)this) + sectionindex) + i; }

	short				zeroframespan;	// frames per span
	short				zeroframecount; // number of spans
	int					zeroframeindex;
	byte				*pZeroFrameData() const { if (zeroframeindex) return (((byte *)this) + zeroframeindex); else return NULL; };
	mutable float		zeroframestalltime;		// saved during read stalls

	mstudioanimdesc_t() {}
private:
	// No copy constructors allowed
	mstudioanimdesc_t(const mstudioanimdesc_t& vOther);
};
struct mstudioevent_t
{
	float				cycle;
	int					event;
	int					type;
	inline const char * pszOptions(void) const { return options; }
	char				options[64];

	int					szeventindex;
	inline char * const pszEventName(void) const { return ((char *)this) + szeventindex; }
};

struct mstudioautolayer_t
{
	//private:
	short				iSequence;
	short				iPose;
	//public:
	int					flags;
	float				start;	// beginning of influence
	float				peak;	// start of full influence
	float				tail;	// end of full influence
	float				end;	// end of all influence
};

struct mstudioiklock_t
{
	int			chain;
	float		flPosWeight;
	float		flLocalQWeight;
	int			flags;

	int			unused[4];
};

struct mstudioactivitymodifier_t
{

	int					sznameindex;
	inline char			*pszName() { return (sznameindex) ? (char *)(((byte *)this) + sznameindex) : NULL; }
};
struct studiohdr_t;
struct mstudioseqdesc_t
{
	int					baseptr;
	inline studiohdr_t	*pStudiohdr(void) const { return (studiohdr_t *)(((byte *)this) + baseptr); }

	int					szlabelindex;
	inline char * const pszLabel(void) const { return ((char *)this) + szlabelindex; }

	int					szactivitynameindex;
	inline char * const pszActivityName(void) const { return ((char *)this) + szactivitynameindex; }

	int					flags;		// looping/non-looping flags

	int					activity;	// initialized at loadtime to game DLL values
	int					actweight;

	int					numevents;
	int					eventindex;
	inline mstudioevent_t *pEvent(int i) const { Assert(i >= 0 && i < numevents); return (mstudioevent_t *)(((byte *)this) + eventindex) + i; };

	Vector				bbmin;		// per sequence bounding box
	Vector				bbmax;

	int					numblends;

	// Index into array of shorts which is groupsize[0] x groupsize[1] in length
	int					animindexindex;

	inline int			anim(int x, int y) const
	{
		if (x >= groupsize[0])
		{
			x = groupsize[0] - 1;
		}

		if (y >= groupsize[1])
		{
			y = groupsize[1] - 1;
		}

		int offset = y * groupsize[0] + x;
		short *blends = (short *)(((byte *)this) + animindexindex);
		int value = (int)blends[offset];
		return value;
	}

	int					movementindex;	// [blend] float array for blended movement
	int					groupsize[2];
	int					paramindex[2];	// X, Y, Z, XR, YR, ZR
	float				paramstart[2];	// local (0..1) starting value
	float				paramend[2];	// local (0..1) ending value
	int					paramparent;

	float				fadeintime;		// ideal cross fate in time (0.2 default)
	float				fadeouttime;	// ideal cross fade out time (0.2 default)

	int					localentrynode;		// transition node at entry
	int					localexitnode;		// transition node at exit
	int					nodeflags;		// transition rules

	float				entryphase;		// used to match entry gait
	float				exitphase;		// used to match exit gait

	float				lastframe;		// frame that should generation EndOfSequence

	int					nextseq;		// auto advancing sequences
	int					pose;			// index of delta animation between end and nextseq

	int					numikrules;

	int					numautolayers;	//
	int					autolayerindex;
	inline mstudioautolayer_t *pAutolayer(int i) const { Assert(i >= 0 && i < numautolayers); return (mstudioautolayer_t *)(((byte *)this) + autolayerindex) + i; };

	int					weightlistindex;
	inline float		*pBoneweight(int i) const { return ((float *)(((byte *)this) + weightlistindex) + i); };
	inline float		weight(int i) const { return *(pBoneweight(i)); };

	// FIXME: make this 2D instead of 2x1D arrays
	int					posekeyindex;
	float				*pPoseKey(int iParam, int iAnim) const { return (float *)(((byte *)this) + posekeyindex) + iParam * groupsize[0] + iAnim; }
	float				poseKey(int iParam, int iAnim) const { return *(pPoseKey(iParam, iAnim)); }

	int					numiklocks;
	int					iklockindex;
	inline mstudioiklock_t *pIKLock(int i) const { Assert(i >= 0 && i < numiklocks); return (mstudioiklock_t *)(((byte *)this) + iklockindex) + i; };

	// Key values
	int					keyvalueindex;
	int					keyvaluesize;
	inline const char * KeyValueText(void) const { return keyvaluesize != 0 ? ((char *)this) + keyvalueindex : NULL; }

	int					cycleposeindex;		// index of pose parameter to use as cycle index

	int					activitymodifierindex;
	int					numactivitymodifiers;
	inline mstudioactivitymodifier_t *pActivityModifier(int i) const { Assert(i >= 0 && i < numactivitymodifiers); return activitymodifierindex != 0 ? (mstudioactivitymodifier_t *)(((byte *)this) + activitymodifierindex) + i : NULL; };

	int					unused[5];		// remove/add as appropriate (grow back to 8 ints on version change!)

	mstudioseqdesc_t() {}
private:
	// No copy constructors allowed
	mstudioseqdesc_t(const mstudioseqdesc_t& vOther);
};

struct mstudiotexture_t
{
	int						sznameindex;
	inline char * const		pszName(void) const { return ((char *)this) + sznameindex; }
	int						flags;
	int						used;
	int						unused1;
	mutable IMaterial		*material;  // fixme: this needs to go away . .isn't used by the engine, but is used by studiomdl
	mutable void			*clientmaterial;	// gary, replace with client material pointer if used

	int						unused[10];
};

#define MAXSTUDIOTRIANGLES	25000
#define MAXSTUDIOVERTS		10000
#define	MAXSTUDIOFLEXVERTS	1000
#define MAXSTUDIOSKINS		32		// total textures
#define MAXSTUDIOBONES		256		// total bones actually used
#define MAXSTUDIOFLEXDESC	1024	// maximum number of low level flexes (actual morph targets)
#define MAXSTUDIOFLEXCTRL	96		// maximum number of flexcontrollers (input sliders)
#define MAXSTUDIOPOSEPARAM	24
#define MAXSTUDIOBONECTRLS	4
#define MAXSTUDIOANIMBLOCKS 256

#define MAXSTUDIOBONEBITS	7		// NOTE: MUST MATCH MAXSTUDIOBONES

// NOTE!!! : Changing this number also changes the vtx file format!!!!!
#define MAX_NUM_BONES_PER_VERT 3

//Adrian - Remove this when we completely phase out the old event system.
#define NEW_EVENT_STYLE ( 1 << 10 )

struct mstudioboneweight_t
{
	float	weight[MAX_NUM_BONES_PER_VERT];
	char	bone[MAX_NUM_BONES_PER_VERT];
	byte	numbones;

	//	byte	material;
	//	short	firstref;
	//	short	lastref;
};
struct mstudiovertex_t
{
	mstudioboneweight_t	m_BoneWeights;
	Vector				m_vecPosition;
	Vector				m_vecNormal;
	Vector2D			m_vecTexCoord;

	mstudiovertex_t() {}

private:
	// No copy constructors allowed
	mstudiovertex_t(const mstudiovertex_t& vOther);
};
struct mstudio_modelvertexdata_t
{
	Vector				*Position(int i) const;
	Vector				*Normal(int i) const;
	Vector4D			*TangentS(int i) const;
	Vector2D			*Texcoord(int i) const;
	mstudioboneweight_t	*BoneWeights(int i) const;
	mstudiovertex_t		*Vertex(int i) const;
	bool				HasTangentData(void) const;
	int					GetGlobalVertexIndex(int i) const;
	int					GetGlobalTangentIndex(int i) const;

	// base of external vertex data stores
	const void			*pVertexData;
	const void			*pTangentData;
};
#define MAX_NUM_LODS 8

struct mstudio_meshvertexdata_t
{
	Vector				*Position(int i) const;
	Vector				*Normal(int i) const;
	Vector4D			*TangentS(int i) const;
	Vector2D			*Texcoord(int i) const;
	mstudioboneweight_t *BoneWeights(int i) const;
	mstudiovertex_t		*Vertex(int i) const;
	bool				HasTangentData(void) const;
	int					GetModelVertexIndex(int i) const;
	int					GetGlobalVertexIndex(int i) const;

	// indirection to this mesh's model's vertex data
	const mstudio_modelvertexdata_t	*modelvertexdata;

	// used for fixup calcs when culling top level lods
	// expected number of mesh verts at desired lod
	int					numLODVertexes[MAX_NUM_LODS];
};

FORCEINLINE unsigned int * PackNormal_UBYTE4(float nx, float ny, float nz, unsigned int *pPackedNormal, bool bIsTangent = false, float binormalSign = +1.0f)
{
	float xSign = nx < 0.0f ? -1.0f : 1.0f;			// -1 or 1 sign
	float ySign = ny < 0.0f ? -1.0f : 1.0f;
	float zSign = nz < 0.0f ? -1.0f : 1.0f;
	float tSign = binormalSign;
	Assert((binormalSign == +1.0f) || (binormalSign == -1.0f));

	float xSignBit = 0.5f*(1 - xSign);			// [-1,+1] -> [1,0]
	float ySignBit = 0.5f*(1 - ySign);			// 1 is negative bit (like slt instruction)
	float zSignBit = 0.5f*(1 - zSign);
	float tSignBit = 0.5f*(1 - binormalSign);

	float absX = xSign * nx;							// 0..1 range (abs)
	float absY = ySign * ny;
	float absZ = zSign * nz;

	float xbits = absX / (absX + absY + absZ);	// Project onto x+y+z=1 plane
	float ybits = absY / (absX + absY + absZ);

	xbits *= 63;									// 0..63
	ybits *= 63;

	xbits = xbits * xSign - xSignBit;				// -64..63 range
	ybits = ybits * ySign - ySignBit;
	xbits += 64.0f;									// 0..127 range
	ybits += 64.0f;

	xbits = xbits * zSign - zSignBit;				// Negate based on z and t
	ybits = ybits * tSign - tSignBit;				// -128..127 range

	xbits += 128.0f;								// 0..255 range
	ybits += 128.0f;

	unsigned char cX = (unsigned char)xbits;
	unsigned char cY = (unsigned char)ybits;

	if (!bIsTangent)
		*pPackedNormal = (cX << 0) | (cY << 8);	// xy for normal
	else
		*pPackedNormal = (cX << 16) | (cY << 24);	// zw for tangent

	return pPackedNormal;
}
FORCEINLINE float * UnpackNormal_UBYTE4(const unsigned int *pPackedNormal, float *pNormal, bool bIsTangent = FALSE)
{
	unsigned char cX, cY;
	if (bIsTangent)
	{
		cX = *pPackedNormal >> 16;					// Unpack Z
		cY = *pPackedNormal >> 24;					// Unpack W
	}
	else
	{
		cX = *pPackedNormal >> 0;					// Unpack X
		cY = *pPackedNormal >> 8;					// Unpack Y
	}

	float x = cX - 128.0f;
	float y = cY - 128.0f;
	float z;

	float zSignBit = x < 0 ? 1.0f : 0.0f;			// z and t negative bits (like slt asm instruction)
	float tSignBit = y < 0 ? 1.0f : 0.0f;
	float zSign = -(2 * zSignBit - 1);			// z and t signs
	float tSign = -(2 * tSignBit - 1);

	x = x * zSign - zSignBit;							// 0..127
	y = y * tSign - tSignBit;
	x = x - 64;										// -64..63
	y = y - 64;

	float xSignBit = x < 0 ? 1.0f : 0.0f;	// x and y negative bits (like slt asm instruction)
	float ySignBit = y < 0 ? 1.0f : 0.0f;
	float xSign = -(2 * xSignBit - 1);			// x and y signs
	float ySign = -(2 * ySignBit - 1);

	x = (x*xSign - xSignBit) / 63.0f;				// 0..1 range
	y = (y*ySign - ySignBit) / 63.0f;
	z = 1.0f - x - y;

	float oolen = 1.0f / sqrt(x*x + y * y + z * z);	// Normalize and
	x *= oolen * xSign;					// Recover signs
	y *= oolen * ySign;
	z *= oolen * zSign;

	pNormal[0] = x;
	pNormal[1] = y;
	pNormal[2] = z;
	if (bIsTangent)
	{
		pNormal[3] = tSign;
	}

	return pNormal;
}
struct mstudiomesh_t;
struct thinModelVertices_t
{
	void Init(int numBoneInfluences, Vector *positions, unsigned short *normals, float *boneWeights, char *boneIndices)
	{
		m_numBoneInfluences = numBoneInfluences;
		m_vecPositions = positions;
		m_vecNormals = normals;
		m_boneWeights = boneWeights;
		m_boneIndices = boneIndices;
	}

	void SetPosition(int vertIndex, const Vector & position)
	{
		Assert(m_vecPositions);
		m_vecPositions[vertIndex] = position;
	}

	void SetNormal(int vertIndex, const Vector & normal)
	{
		Assert(m_vecNormals);
		unsigned int packedNormal;
		PackNormal_UBYTE4(normal.m_x, normal.m_y, normal.m_z, &packedNormal);
		m_vecNormals[vertIndex] = (unsigned short)(0x0000FFFF & packedNormal);
	}

	void SetBoneWeights(int vertIndex, const mstudioboneweight_t & boneWeights)
	{
		Assert((m_numBoneInfluences >= 1) && (m_numBoneInfluences <= 3));
		Assert((boneWeights.numbones >= 1) && (boneWeights.numbones <= m_numBoneInfluences));
		int    numStoredWeights = max(0, (m_numBoneInfluences - 1));
		float *pBaseWeight = m_boneWeights + vertIndex * numStoredWeights;
		char  *pBaseIndex = m_boneIndices + vertIndex * m_numBoneInfluences;
		for (int i = 0; i < m_numBoneInfluences; i++)
		{
			pBaseIndex[i] = boneWeights.bone[i];
		}
		for (int i = 0; i < numStoredWeights; i++)
		{
			pBaseWeight[i] = boneWeights.weight[i];
		}
	}

	void GetMeshPosition(mstudiomesh_t *pMesh, int meshIndex, Vector *pPosition) const
	{
		Assert(pMesh);
		//GetPosition(pMesh->vertexdata.GetGlobalVertexIndex(meshIndex), pPosition);
	}

	void GetMeshNormal(mstudiomesh_t *pMesh, int meshIndex, Vector *pNormal) const
	{
		Assert(pMesh);
		//GetNormal(pMesh->vertexdata.GetGlobalVertexIndex(meshIndex), pNormal);
	}

	void GetMeshBoneWeights(mstudiomesh_t *pMesh, int meshIndex, mstudioboneweight_t *pBoneWeights) const
	{
		Assert(pMesh);
		//GetBoneWeights(pMesh->vertexdata.GetGlobalVertexIndex(meshIndex), pBoneWeights);
	}

	void GetModelPosition(mstudiomodel_t *pModel, int modelIndex, Vector *pPosition) const
	{
		Assert(pModel);
		//GetPosition(pModel->vertexdata.GetGlobalVertexIndex(modelIndex), pPosition);
	}

	void GetModelNormal(mstudiomodel_t *pModel, int modelIndex, Vector *pNormal) const
	{
		Assert(pModel);
		//GetNormal(pModel->vertexdata.GetGlobalVertexIndex(modelIndex), pNormal);
	}

	void GetModelBoneWeights(mstudiomodel_t *pModel, int modelIndex, mstudioboneweight_t *pBoneWeights) const
	{
		Assert(pModel);
		//GetBoneWeights(pModel->vertexdata.GetGlobalVertexIndex(modelIndex), pBoneWeights);
	}

private:
	void GetPosition(int vertIndex, Vector *pPosition) const
	{
		Assert(pPosition);
		Assert(m_vecPositions);
		*pPosition = m_vecPositions[vertIndex];
	}

	void GetNormal(int vertIndex, Vector *pNormal) const
	{
		Assert(pNormal);
		Assert(m_vecNormals);
		unsigned int packedNormal = 0x0000FFFF & m_vecNormals[vertIndex];
		UnpackNormal_UBYTE4(&packedNormal, (float*)pNormal);
	}

	void GetBoneWeights(int vertIndex, mstudioboneweight_t *pBoneWeights) const
	{

		int    numStoredWeights = max(0, (m_numBoneInfluences - 1));
		float *pBaseWeight = m_boneWeights + vertIndex * numStoredWeights;
		char  *pBaseIndex = m_boneIndices + vertIndex * m_numBoneInfluences;
		float  sum = 0.0f;
		for (int i = 0; i < MAX_NUM_BONES_PER_VERT; i++)
		{
			if (i < (m_numBoneInfluences - 1))
				pBoneWeights->weight[i] = pBaseWeight[i];
			else
				pBoneWeights->weight[i] = 1.0f - sum;
			sum += pBoneWeights->weight[i];

			pBoneWeights->bone[i] = (i < m_numBoneInfluences) ? pBaseIndex[i] : 0;
		}

		// Treat 'zero weights' as '100% binding to bone zero':
		pBoneWeights->numbones = m_numBoneInfluences ? m_numBoneInfluences : 1;
	}

	int				m_numBoneInfluences;// Number of bone influences per vertex, N
	float			*m_boneWeights;		// This array stores (N-1) weights per vertex (unless N is zero)
	char			*m_boneIndices;		// This array stores N indices per vertex
	Vector			*m_vecPositions;
	unsigned short	*m_vecNormals;		// Normals are compressed into 16 bits apiece (see PackNormal_UBYTE4() )
};
struct mstudiovertanim_t
{
	unsigned short		index;
	byte				speed;	// 255/max_length_in_flex
	byte				side;	// 255/left_right

protected:
	// JasonM changing this type a lot, to prefer fixed point 16 bit...
	union
	{
		short			delta[3];
		float16			flDelta[3];
	};

	union
	{
		short			ndelta[3];
		float16			flNDelta[3];
	};

public:
	inline void ConvertToFixed(float flVertAnimFixedPointScale)
	{
		delta[0] = flDelta[0].GetFloat() / flVertAnimFixedPointScale;
		delta[1] = flDelta[1].GetFloat() / flVertAnimFixedPointScale;
		delta[2] = flDelta[2].GetFloat() / flVertAnimFixedPointScale;
		ndelta[0] = flNDelta[0].GetFloat() / flVertAnimFixedPointScale;
		ndelta[1] = flNDelta[1].GetFloat() / flVertAnimFixedPointScale;
		ndelta[2] = flNDelta[2].GetFloat() / flVertAnimFixedPointScale;
	}

	inline Vector GetDeltaFixed(float flVertAnimFixedPointScale)
	{
		return Vector(delta[0] * flVertAnimFixedPointScale, delta[1] * flVertAnimFixedPointScale, delta[2] * flVertAnimFixedPointScale);
	}
	inline Vector GetNDeltaFixed(float flVertAnimFixedPointScale)
	{
		return Vector(ndelta[0] * flVertAnimFixedPointScale, ndelta[1] * flVertAnimFixedPointScale, ndelta[2] * flVertAnimFixedPointScale);
	}
	inline Vector GetDeltaFloat()
	{
		return Vector(flDelta[0].GetFloat(), flDelta[1].GetFloat(), flDelta[2].GetFloat());
	}
	inline Vector GetNDeltaFloat()
	{
		return Vector(flNDelta[0].GetFloat(), flNDelta[1].GetFloat(), flNDelta[2].GetFloat());
	}
	inline void SetDeltaFixed(const Vector& vInput, float flVertAnimFixedPointScale)
	{
		delta[0] = vInput.m_x / flVertAnimFixedPointScale;
		delta[1] = vInput.m_y / flVertAnimFixedPointScale;
		delta[2] = vInput.m_z / flVertAnimFixedPointScale;
	}
	inline void SetNDeltaFixed(const Vector& vInputNormal, float flVertAnimFixedPointScale)
	{
		ndelta[0] = vInputNormal.m_x / flVertAnimFixedPointScale;
		ndelta[1] = vInputNormal.m_y / flVertAnimFixedPointScale;
		ndelta[2] = vInputNormal.m_z / flVertAnimFixedPointScale;
	}

	// Ick...can also force fp16 data into this structure for writing to file in legacy format...
	inline void SetDeltaFloat(const Vector& vInput)
	{
		flDelta[0].SetFloat(vInput.m_x);
		flDelta[1].SetFloat(vInput.m_y);
		flDelta[2].SetFloat(vInput.m_z);
	}
	inline void SetNDeltaFloat(const Vector& vInputNormal)
	{
		flNDelta[0].SetFloat(vInputNormal.m_x);
		flNDelta[1].SetFloat(vInputNormal.m_y);
		flNDelta[2].SetFloat(vInputNormal.m_z);
	}

	class CSortByIndex
	{
	public:
		bool operator()(const mstudiovertanim_t &left, const mstudiovertanim_t & right)const
		{
			return left.index < right.index;
		}
	};
	friend class CSortByIndex;

	mstudiovertanim_t() {}
	//private:
	// No copy constructors allowed, but it's needed for std::sort()
	//	mstudiovertanim_t(const mstudiovertanim_t& vOther);
};
struct mstudiovertanim_wrinkle_t : public mstudiovertanim_t
{

	short	wrinkledelta;

	inline void SetWrinkleFixed(float flWrinkle, float flVertAnimFixedPointScale)
	{
		int nWrinkleDeltaInt = flWrinkle / flVertAnimFixedPointScale;
		wrinkledelta = clamp(nWrinkleDeltaInt, -32767, 32767);
	}

	inline Vector4D GetDeltaFixed(float flVertAnimFixedPointScale)
	{
		return Vector4D(delta[0] * flVertAnimFixedPointScale, delta[1] * flVertAnimFixedPointScale, delta[2] * flVertAnimFixedPointScale, wrinkledelta * flVertAnimFixedPointScale);
	}
	inline float GetWrinkleDeltaFixed(float flVertAnimFixedPointScale)
	{
		return wrinkledelta * flVertAnimFixedPointScale;
	}
};


enum StudioVertAnimType_t
{
	STUDIO_VERT_ANIM_NORMAL = 0,
	STUDIO_VERT_ANIM_WRINKLE,
};
struct mstudioflex_t
{
	int					flexdesc;	// input value

	float				target0;	// zero
	float				target1;	// one
	float				target2;	// one
	float				target3;	// zero

	int					numverts;
	int					vertindex;

	inline	mstudiovertanim_t *pVertanim(int i) const { return (mstudiovertanim_t *)(((byte *)this) + vertindex) + i; };
	inline	mstudiovertanim_wrinkle_t *pVertanimWrinkle(int i) const { return  (mstudiovertanim_wrinkle_t *)(((byte *)this) + vertindex) + i; };

	inline	byte *pBaseVertanim() const { return ((byte *)this) + vertindex; };
	inline	int	VertAnimSizeBytes() const { return (vertanimtype == STUDIO_VERT_ANIM_NORMAL) ? sizeof(mstudiovertanim_t) : sizeof(mstudiovertanim_wrinkle_t); }

	int					flexpair;	// second flex desc
	unsigned char		vertanimtype;	// See StudioVertAnimType_t
	unsigned char		unusedchar[3];
	int					unused[6];
};
struct mstudiomesh_t
{
	int					material;

	int					modelindex;
	mstudiomodel_t *pModel() const;

	int					numvertices;		// number of unique vertices/normals/texcoords
	int					vertexoffset;		// vertex mstudiovertex_t

	// Access thin/fat mesh vertex data (only one will return a non-NULL result)
	const mstudio_meshvertexdata_t	*GetVertexData(void *pModelData = NULL);
	const thinModelVertices_t		*GetThinVertexData(void *pModelData = NULL);

	int					numflexes;			// vertex animation
	int					flexindex;
	inline mstudioflex_t *pFlex(int i) const { return (mstudioflex_t *)(((byte *)this) + flexindex) + i; };

	// special codes for material operations
	int					materialtype;
	int					materialparam;

	// a unique ordinal for this mesh
	int					meshid;

	Vector				center;

	mstudio_meshvertexdata_t vertexdata;

	int					unused[8]; // remove as appropriate

	mstudiomesh_t() {}
private:
	// No copy constructors allowed
	mstudiomesh_t(const mstudiomesh_t& vOther);
};
#define MODEL_VERTEX_FILE_ID		(('V'<<24)+('S'<<16)+('D'<<8)+'I')
#define MODEL_VERTEX_FILE_THIN_ID	(('V'<<24)+('C'<<16)+('D'<<8)+'I')

struct vertexFileHeader_t
{
	int		id;								// MODEL_VERTEX_FILE_ID
	int		version;						// MODEL_VERTEX_FILE_VERSION
	int		checksum;						// same as studiohdr_t, ensures sync
	int		numLODs;						// num of valid lods
	int		numLODVertexes[MAX_NUM_LODS];	// num verts for desired root lod
	int		numFixups;						// num of vertexFileFixup_t
	int		fixupTableStart;				// offset from base to fixup table
	int		vertexDataStart;				// offset from base to vertex block
	int		tangentDataStart;				// offset from base to tangent block

public:

	// Accessor to fat vertex data
	const mstudiovertex_t *GetVertexData() const
	{
		if ((id == MODEL_VERTEX_FILE_ID) && (vertexDataStart != 0))
			return (mstudiovertex_t *)(vertexDataStart + (byte *)this);
		else
			return NULL;
	}
	// Accessor to (fat) tangent vertex data (tangents aren't stored in compressed data)
	const Vector4D *GetTangentData() const
	{
		if ((id == MODEL_VERTEX_FILE_ID) && (tangentDataStart != 0))
			return (Vector4D *)(tangentDataStart + (byte *)this);
		else
			return NULL;
	}
	// Accessor to thin vertex data
	const  thinModelVertices_t *GetThinVertexData() const
	{
		if ((id == MODEL_VERTEX_FILE_THIN_ID) && (vertexDataStart != 0))
			return (thinModelVertices_t *)(vertexDataStart + (byte *)this);
		else
			return NULL;
	}
};
struct mstudioeyeball_t
{
	int					sznameindex;
	inline char * const pszName(void) const { return ((char *)this) + sznameindex; }
	int		bone;
	Vector	org;
	float	zoffset;
	float	radius;
	Vector	up;
	Vector	forward;
	int		texture;

	int		unused1;
	float	iris_scale;
	int		unused2;

	int		upperflexdesc[3];	// index of raiser, neutral, and lowerer flexdesc that is set by flex controllers
	int		lowerflexdesc[3];
	float	uppertarget[3];		// angle (radians) of raised, neutral, and lowered lid positions
	float	lowertarget[3];

	int		upperlidflexdesc;	// index of flex desc that actual lid flexes look to
	int		lowerlidflexdesc;
	int		unused[4];			// These were used before, so not guaranteed to be 0
	bool	m_bNonFACS;			// Never used before version 44
	char	unused3[3];
	int		unused4[7];

	mstudioeyeball_t() {}
private:
	// No copy constructors allowed
	mstudioeyeball_t(const mstudioeyeball_t& vOther);
};
struct mstudiomodel_t
{
	inline const char * pszName(void) const { return name; }
	char				name[64];

	int					type;

	float				boundingradius;

	int					nummeshes;
	int					meshindex;
	inline mstudiomesh_t *pMesh(int i) const { return (mstudiomesh_t *)(((byte *)this) + meshindex) + i; };

	// cache purposes
	int					numvertices;		// number of unique vertices/normals/texcoords
	int					vertexindex;		// vertex Vector
	int					tangentsindex;		// tangents Vector

	// These functions are defined in application-specific code:
	const vertexFileHeader_t			*CacheVertexData(void *pModelData);

	// Access thin/fat mesh vertex data (only one will return a non-NULL result)
	const mstudio_modelvertexdata_t		*GetVertexData(void *pModelData = NULL);
	const thinModelVertices_t			*GetThinVertexData(void *pModelData = NULL);

	int					numattachments;
	int					attachmentindex;

	int					numeyeballs;
	int					eyeballindex;
	inline  mstudioeyeball_t *pEyeball(int i) { return (mstudioeyeball_t *)(((byte *)this) + eyeballindex) + i; };

	mstudio_modelvertexdata_t vertexdata;

	int					unused[8];		// remove as appropriate
};

struct mstudiobodyparts_t
{
	int					sznameindex;
	inline char * const pszName(void) const { return ((char *)this) + sznameindex; }
	int					nummodels;
	int					base;
	int					modelindex; // index into models array
	inline mstudiomodel_t *pModel(int i) const { return (mstudiomodel_t *)(((byte *)this) + modelindex) + i; };
};
struct mstudioattachment_t
{
	int					sznameindex;
	inline char * const pszName(void) const { return ((char *)this) + sznameindex; }
	unsigned int		flags;
	int					localbone;
	matrix3x4_t			local; // attachment point
	int					unused[8];
};


struct mstudioflexdesc_t
{
	int					szFACSindex;
	inline char * const pszFACS(void) const { return ((char *)this) + szFACSindex; }
};

struct mstudioflexcontroller_t
{
	int					sztypeindex;
	inline char * const pszType(void) const { return ((char *)this) + sztypeindex; }
	int					sznameindex;
	inline char * const pszName(void) const { return ((char *)this) + sznameindex; }
	mutable int			localToGlobal;	// remapped at load time to master list
	float				min;
	float				max;
};
enum LocalFlexController_t
{
	// this isn't really an enum - its just a typed int. gcc will not accept it as a fwd decl, so we'll define one value
	DUMMY_FLEX_CONTROLLER = 0x7fffffff						// make take 32 bits
};
struct mstudioflexop_t
{
	int		op;
	union
	{
		int		index;
		float	value;
	} d;
};
struct mstudioflexrule_t
{
	int					flex;
	int					numops;
	int					opindex;
	inline mstudioflexop_t *iFlexOp(int i) const { return  (mstudioflexop_t *)(((byte *)this) + opindex) + i; };
};
struct mstudioiklink_t
{
	int		bone;
	Vector	kneeDir;	// ideal bending direction (per link, if applicable)
	Vector	unused0;	// unused

	mstudioiklink_t() {}
private:
	// No copy constructors allowed
	mstudioiklink_t(const mstudioiklink_t& vOther);
};
struct mstudioikchain_t
{
	int				sznameindex;
	inline char * const pszName(void) const { return ((char *)this) + sznameindex; }
	int				linktype;
	int				numlinks;
	int				linkindex;
	inline mstudioiklink_t *pLink(int i) const { return (mstudioiklink_t *)(((byte *)this) + linkindex) + i; };
	// FIXME: add unused entries
};
struct mstudiomouth_t
{
	int					bone;
	Vector				forward;
	int					flexdesc;

	mstudiomouth_t() {}
private:
	// No copy constructors allowed
	mstudiomouth_t(const mstudiomouth_t& vOther);
};
struct mstudioposeparamdesc_t
{
	int					sznameindex;
	inline char * const pszName(void) const { return ((char *)this) + sznameindex; }
	int					flags;	// ????
	float				start;	// starting value
	float				end;	// ending value
	float				loop;	// looping range, 0 for no looping, 360 for rotations, etc.
};
struct mstudiomodelgroup_t
{
	int					szlabelindex;	// textual name
	inline char * const pszLabel(void) const { return ((char *)this) + szlabelindex; }
	int					sznameindex;	// file name
	inline char * const pszName(void) const { return ((char *)this) + sznameindex; }
};
struct mstudioanimblock_t
{
	int					datastart;
	int					dataend;
};
enum FlexControllerRemapType_t
{
	FLEXCONTROLLER_REMAP_PASSTHRU = 0,
	FLEXCONTROLLER_REMAP_2WAY,	// Control 0 -> ramps from 1-0 from 0->0.5. Control 1 -> ramps from 0-1 from 0.5->1
	FLEXCONTROLLER_REMAP_NWAY,	// StepSize = 1 / (control count-1) Control n -> ramps from 0-1-0 from (n-1)*StepSize to n*StepSize to (n+1)*StepSize. A second control is needed to specify amount to use 
	FLEXCONTROLLER_REMAP_EYELID
};

struct mstudioflexcontrollerui_t
{
	int					sznameindex;
	inline char * const pszName(void) const { return ((char *)this) + sznameindex; }

	// These are used like a union to save space
	// Here are the possible configurations for a UI controller
	//
	// SIMPLE NON-STEREO:	0: control	1: unused	2: unused
	// STEREO:				0: left		1: right	2: unused
	// NWAY NON-STEREO:		0: control	1: unused	2: value
	// NWAY STEREO:			0: left		1: right	2: value

	int					szindex0;
	int					szindex1;
	int					szindex2;

	inline const mstudioflexcontroller_t *pController(void) const
	{
		return !stereo ? (mstudioflexcontroller_t *)((char *)this + szindex0) : NULL;
	}
	inline char * const	pszControllerName(void) const { return !stereo ? pController()->pszName() : NULL; }
	inline int			controllerIndex(const CStudioHdr &cStudioHdr) const;

	inline const mstudioflexcontroller_t *pLeftController(void) const
	{
		return stereo ? (mstudioflexcontroller_t *)((char *)this + szindex0) : NULL;
	}
	inline char * const	pszLeftName(void) const { return stereo ? pLeftController()->pszName() : NULL; }
	inline int			leftIndex(const CStudioHdr &cStudioHdr) const;

	inline const mstudioflexcontroller_t *pRightController(void) const
	{
		return stereo ? (mstudioflexcontroller_t *)((char *)this + szindex1) : NULL;
	}
	inline char * const	pszRightName(void) const { return stereo ? pRightController()->pszName() : NULL; }
	inline int			rightIndex(const CStudioHdr &cStudioHdr) const;

	inline const mstudioflexcontroller_t *pNWayValueController(void) const
	{
		return remaptype == FLEXCONTROLLER_REMAP_NWAY ? (mstudioflexcontroller_t *)((char *)this + szindex2) : NULL;
	}
	inline char * const	pszNWayValueName(void) const { return remaptype == FLEXCONTROLLER_REMAP_NWAY ? pNWayValueController()->pszName() : NULL; }
	inline int			nWayValueIndex(const CStudioHdr &cStudioHdr) const;

	// Number of controllers this ui description contains, 1, 2 or 3
	inline int			Count() const { return (stereo ? 2 : 1) + (remaptype == FLEXCONTROLLER_REMAP_NWAY ? 1 : 0); }
	inline const mstudioflexcontroller_t *pController(int index) const;

	unsigned char		remaptype;	// See the FlexControllerRemapType_t enum
	bool				stereo;		// Is this a stereo control?
	byte				unused[2];
};
// This flag is set if no hitbox information was specified
#define STUDIOHDR_FLAGS_AUTOGENERATED_HITBOX				0x00000001

// NOTE:  This flag is set at loadtime, not mdl build time so that we don't have to rebuild
// models when we change materials.
#define STUDIOHDR_FLAGS_USES_ENV_CUBEMAP					0x00000002

// Use this when there are translucent parts to the model but we're not going to sort it 
#define STUDIOHDR_FLAGS_FORCE_OPAQUE						0x00000004

// Use this when we want to render the opaque parts during the opaque pass
// and the translucent parts during the translucent pass
#define STUDIOHDR_FLAGS_TRANSLUCENT_TWOPASS					0x00000008

// This is set any time the .qc files has $staticprop in it
// Means there's no bones and no transforms
#define STUDIOHDR_FLAGS_STATIC_PROP							0x00000010

// NOTE:  This flag is set at loadtime, not mdl build time so that we don't have to rebuild
// models when we change materials.
#define STUDIOHDR_FLAGS_USES_FB_TEXTURE						0x00000020

// This flag is set by studiomdl.exe if a separate "$shadowlod" entry was present
//  for the .mdl (the shadow lod is the last entry in the lod list if present)
#define STUDIOHDR_FLAGS_HASSHADOWLOD						0x00000040

// NOTE:  This flag is set at loadtime, not mdl build time so that we don't have to rebuild
// models when we change materials.
#define STUDIOHDR_FLAGS_USES_BUMPMAPPING					0x00000080

// NOTE:  This flag is set when we should use the actual materials on the shadow LOD
// instead of overriding them with the default one (necessary for translucent shadows)
#define STUDIOHDR_FLAGS_USE_SHADOWLOD_MATERIALS				0x00000100

// NOTE:  This flag is set when we should use the actual materials on the shadow LOD
// instead of overriding them with the default one (necessary for translucent shadows)
#define STUDIOHDR_FLAGS_OBSOLETE							0x00000200

#define STUDIOHDR_FLAGS_UNUSED								0x00000400

// NOTE:  This flag is set at mdl build time
#define STUDIOHDR_FLAGS_NO_FORCED_FADE						0x00000800

// NOTE:  The npc will lengthen the viseme check to always include two phonemes
#define STUDIOHDR_FLAGS_FORCE_PHONEME_CROSSFADE				0x00001000

// This flag is set when the .qc has $constantdirectionallight in it
// If set, we use constantdirectionallightdot to calculate light intensity
// rather than the normal directional dot product
// only valid if STUDIOHDR_FLAGS_STATIC_PROP is also set
#define STUDIOHDR_FLAGS_CONSTANT_DIRECTIONAL_LIGHT_DOT		0x00002000

// Flag to mark delta flexes as already converted from disk format to memory format
#define STUDIOHDR_FLAGS_FLEXES_CONVERTED					0x00004000

// Indicates the studiomdl was built in preview mode
#define STUDIOHDR_FLAGS_BUILT_IN_PREVIEW_MODE				0x00008000

// Ambient boost (runtime flag)
#define STUDIOHDR_FLAGS_AMBIENT_BOOST						0x00010000

// Don't cast shadows from this model (useful on first-person models)
#define STUDIOHDR_FLAGS_DO_NOT_CAST_SHADOWS					0x00020000

// alpha textures should cast shadows in vrad on this model (ONLY prop_static!)
#define STUDIOHDR_FLAGS_CAST_TEXTURE_SHADOWS				0x00040000


// flagged on load to indicate no animation events on this model
#define STUDIOHDR_FLAGS_VERT_ANIM_FIXED_POINT_SCALE			0x00200000



struct mstudiolinearbone_t
{

	int numbones;

	int flagsindex;
	inline int flags(int i) const { Assert(i >= 0 && i < numbones); return *((int *)(((byte *)this) + flagsindex) + i); };
	inline int *pflags(int i) { Assert(i >= 0 && i < numbones); return ((int *)(((byte *)this) + flagsindex) + i); };

	int	parentindex;
	inline int parent(int i) const { Assert(i >= 0 && i < numbones); return *((int *)(((byte *)this) + parentindex) + i); };

	int	posindex;
	inline Vector pos(int i) const { Assert(i >= 0 && i < numbones); return *((Vector *)(((byte *)this) + posindex) + i); };

	int quatindex;
	inline Quaternion quat(int i) const { Assert(i >= 0 && i < numbones); return *((Quaternion *)(((byte *)this) + quatindex) + i); };

	int rotindex;
	inline RadianEuler rot(int i) const { Assert(i >= 0 && i < numbones); return *((RadianEuler *)(((byte *)this) + rotindex) + i); };

	int posetoboneindex;
	inline matrix3x4_t poseToBone(int i) const { Assert(i >= 0 && i < numbones); return *((matrix3x4_t *)(((byte *)this) + posetoboneindex) + i); };

	int	posscaleindex;
	inline Vector posscale(int i) const { Assert(i >= 0 && i < numbones); return *((Vector *)(((byte *)this) + posscaleindex) + i); };

	int	rotscaleindex;
	inline Vector rotscale(int i) const { Assert(i >= 0 && i < numbones); return *((Vector *)(((byte *)this) + rotscaleindex) + i); };

	int	qalignmentindex;
	inline Quaternion qalignment(int i) const { Assert(i >= 0 && i < numbones); return *((Quaternion *)(((byte *)this) + qalignmentindex) + i); };

	int unused[6];

	mstudiolinearbone_t() {}
private:
	// No copy constructors allowed
	mstudiolinearbone_t(const mstudiolinearbone_t& vOther);
};
struct mstudioboneflexdrivercontrol_t
{

	int m_nBoneComponent;		// Bone component that drives flex, StudioBoneFlexComponent_t
	int m_nFlexControllerIndex;	// Flex controller to drive
	float m_flMin;				// Min value of bone component mapped to 0 on flex controller
	float m_flMax;				// Max value of bone component mapped to 1 on flex controller

	mstudioboneflexdrivercontrol_t() {}
private:
	// No copy constructors allowed
	mstudioboneflexdrivercontrol_t(const mstudioboneflexdrivercontrol_t &vOther);
};

struct mstudioboneflexdriver_t
{

	int m_nBoneIndex;			// Bone to drive flex controller
	int m_nControlCount;		// Number of flex controllers being driven
	int m_nControlIndex;		// Index into data where controllers are (relative to this)

	inline mstudioboneflexdrivercontrol_t *pBoneFlexDriverControl(int i) const
	{
		Assert(i >= 0 && i < m_nControlCount);
		return (mstudioboneflexdrivercontrol_t *)(((byte *)this) + m_nControlIndex) + i;
	}

	int unused[3];

	mstudioboneflexdriver_t() {}
private:
	// No copy constructors allowed
	mstudioboneflexdriver_t(const mstudioboneflexdriver_t &vOther);
};

struct studiohdr2_t
{
	// NOTE: For forward compat, make sure any methods in this struct
	// are also available in studiohdr_t so no leaf code ever directly references
	// a studiohdr2_t structure
	int numsrcbonetransform;
	int srcbonetransformindex;

	int	illumpositionattachmentindex;
	inline int			IllumPositionAttachmentIndex() const { return illumpositionattachmentindex; }

	float flMaxEyeDeflection;
	inline float		MaxEyeDeflection() const { return flMaxEyeDeflection != 0.0f ? flMaxEyeDeflection : 0.866f; } // default to cos(30) if not set

	int linearboneindex;
	inline mstudiolinearbone_t *pLinearBones() const { return (linearboneindex) ? (mstudiolinearbone_t *)(((byte *)this) + linearboneindex) : NULL; }

	int sznameindex;
	inline char *pszName() { return (sznameindex) ? (char *)(((byte *)this) + sznameindex) : NULL; }

	int m_nBoneFlexDriverCount;
	int m_nBoneFlexDriverIndex;
	inline mstudioboneflexdriver_t *pBoneFlexDriver(int i) const { Assert(i >= 0 && i < m_nBoneFlexDriverCount); return (mstudioboneflexdriver_t *)(((byte *)this) + m_nBoneFlexDriverIndex) + i; }

	int reserved[56];
};

struct mstudiosrcbonetransform_t
{

	int			sznameindex;
	inline const char *pszName(void) const { return ((char *)this) + sznameindex; }
	matrix3x4_t	pretransform;
	matrix3x4_t	posttransform;
};







struct studiohdr_t
{
	int					id;
	int					version;

	int					checksum;		// this has to be the same in the phy and vtx files to load!

	inline const char *	pszName(void) const { if (studiohdr2index && pStudioHdr2()->pszName()) return pStudioHdr2()->pszName(); else return name; }
	char				name[64];
	int					length;


	Vector				eyeposition;	// ideal eye position

	Vector				illumposition;	// illumination center

	Vector				hull_min;		// ideal movement hull size
	Vector				hull_max;

	Vector				view_bbmin;		// clipping bounding box
	Vector				view_bbmax;

	int					flags;

	int					numbones;			// bones
	int					boneindex;
	inline mstudiobone_t *pBone(int i) const { Assert(i >= 0 && i < numbones); return (mstudiobone_t *)(((byte *)this) + boneindex) + i; };
	int					RemapSeqBone(int iSequence, int iLocalBone) const;	// maps local sequence bone to global bone
	int					RemapAnimBone(int iAnim, int iLocalBone) const;		// maps local animations bone to global bone

	int					numbonecontrollers;		// bone controllers
	int					bonecontrollerindex;
	inline mstudiobonecontroller_t *pBonecontroller(int i) const { Assert(i >= 0 && i < numbonecontrollers); return (mstudiobonecontroller_t *)(((byte *)this) + bonecontrollerindex) + i; };

	int					numhitboxsets;
	int					hitboxsetindex;

	// Look up hitbox set by index
	mstudiohitboxset_t	*pHitboxSet(int i) const
	{
		Assert(i >= 0 && i < numhitboxsets);
		return (mstudiohitboxset_t *)(((byte *)this) + hitboxsetindex) + i;
	};

	// Calls through to hitbox to determine size of specified set
	inline mstudiobbox_t *pHitbox(int i, int set) const
	{
		mstudiohitboxset_t const *s = pHitboxSet(set);
		if (!s)
			return NULL;

		return s->pHitbox(i);
	};

	// Calls through to set to get hitbox count for set
	inline int			iHitboxCount(int set) const
	{
		mstudiohitboxset_t const *s = pHitboxSet(set);
		if (!s)
			return 0;

		return s->numhitboxes;
	};

	// file local animations? and sequences
//private:
	int					numlocalanim;			// animations/poses
	int					localanimindex;		// animation descriptions
	inline mstudioanimdesc_t *pLocalAnimdesc(int i) const { if (i < 0 || i >= numlocalanim) i = 0; return (mstudioanimdesc_t *)(((byte *)this) + localanimindex) + i; };

	int					numlocalseq;				// sequences
	int					localseqindex;
	inline mstudioseqdesc_t *pLocalSeqdesc(int i) const { if (i < 0 || i >= numlocalseq) i = 0; return (mstudioseqdesc_t *)(((byte *)this) + localseqindex) + i; };

	//public:
	bool				SequencesAvailable() const;
	int					GetNumSeq() const;
	mstudioanimdesc_t	&pAnimdesc(int i) const;
	mstudioseqdesc_t	&pSeqdesc(int i) const;
	int					iRelativeAnim(int baseseq, int relanim) const;	// maps seq local anim reference to global anim index
	int					iRelativeSeq(int baseseq, int relseq) const;		// maps seq local seq reference to global seq index

//private:
	mutable int			activitylistversion;	// initialization flag - have the sequences been indexed?
	mutable int			eventsindexed;
	//public:
	int					GetSequenceActivity(int iSequence);
	void				SetSequenceActivity(int iSequence, int iActivity);
	int					GetActivityListVersion(void);
	void				SetActivityListVersion(int version) const;
	int					GetEventListVersion(void);
	void				SetEventListVersion(int version);

	// raw textures
	int					numtextures;
	int					textureindex;
	inline mstudiotexture_t *pTexture(int i) const { Assert(i >= 0 && i < numtextures); return (mstudiotexture_t *)(((byte *)this) + textureindex) + i; };


	// raw textures search paths
	int					numcdtextures;
	int					cdtextureindex;
	inline char			*pCdtexture(int i) const { return (((char *)this) + *((int *)(((byte *)this) + cdtextureindex) + i)); };

	// replaceable textures tables
	int					numskinref;
	int					numskinfamilies;
	int					skinindex;
	inline short		*pSkinref(int i) const { return (short *)(((byte *)this) + skinindex) + i; };

	int					numbodyparts;
	int					bodypartindex;
	inline mstudiobodyparts_t	*pBodypart(int i) const { return (mstudiobodyparts_t *)(((byte *)this) + bodypartindex) + i; };

	// queryable attachable points
//private:
	int					numlocalattachments;
	int					localattachmentindex;
	inline mstudioattachment_t	*pLocalAttachment(int i) const { Assert(i >= 0 && i < numlocalattachments); return (mstudioattachment_t *)(((byte *)this) + localattachmentindex) + i; };
	//public:
	int					GetNumAttachments(void) const;
	const mstudioattachment_t &pAttachment(int i) const;
	int					GetAttachmentBone(int i);
	// used on my tools in hlmv, not persistant
	void				SetAttachmentBone(int iAttachment, int iBone);

	// animation node to animation node transition graph
//private:
	int					numlocalnodes;
	int					localnodeindex;
	int					localnodenameindex;
	inline char			*pszLocalNodeName(int iNode) const { Assert(iNode >= 0 && iNode < numlocalnodes); return (((char *)this) + *((int *)(((byte *)this) + localnodenameindex) + iNode)); }
	inline byte			*pLocalTransition(int i) const { Assert(i >= 0 && i < (numlocalnodes * numlocalnodes)); return (byte *)(((byte *)this) + localnodeindex) + i; };

	//public:
	int					EntryNode(int iSequence);
	int					ExitNode(int iSequence);
	char				*pszNodeName(int iNode);
	int					GetTransition(int iFrom, int iTo) const;

	int					numflexdesc;
	int					flexdescindex;
	inline mstudioflexdesc_t *pFlexdesc(int i) const { Assert(i >= 0 && i < numflexdesc); return (mstudioflexdesc_t *)(((byte *)this) + flexdescindex) + i; };

	int					numflexcontrollers;
	int					flexcontrollerindex;
	inline mstudioflexcontroller_t *pFlexcontroller(LocalFlexController_t i) const { Assert(numflexcontrollers == 0 || (i >= 0 && i < numflexcontrollers)); return (mstudioflexcontroller_t *)(((byte *)this) + flexcontrollerindex) + i; };

	int					numflexrules;
	int					flexruleindex;
	inline mstudioflexrule_t *pFlexRule(int i) const { Assert(i >= 0 && i < numflexrules); return (mstudioflexrule_t *)(((byte *)this) + flexruleindex) + i; };

	int					numikchains;
	int					ikchainindex;
	inline mstudioikchain_t *pIKChain(int i) const { Assert(i >= 0 && i < numikchains); return (mstudioikchain_t *)(((byte *)this) + ikchainindex) + i; };

	int					nummouths;
	int					mouthindex;
	inline mstudiomouth_t *pMouth(int i) const { Assert(i >= 0 && i < nummouths); return (mstudiomouth_t *)(((byte *)this) + mouthindex) + i; };

	//private:
	int					numlocalposeparameters;
	int					localposeparamindex;
	inline mstudioposeparamdesc_t *pLocalPoseParameter(int i) const { Assert(i >= 0 && i < numlocalposeparameters); return (mstudioposeparamdesc_t *)(((byte *)this) + localposeparamindex) + i; };
	//public:
	int					GetNumPoseParameters(void) const;
	const mstudioposeparamdesc_t &pPoseParameter(int i);
	int					GetSharedPoseParameter(int iSequence, int iLocalPose) const;

	int					surfacepropindex;
	inline char * const pszSurfaceProp(void) const { return ((char *)this) + surfacepropindex; }

	// Key values
	int					keyvalueindex;
	int					keyvaluesize;
	inline const char * KeyValueText(void) const { return keyvaluesize != 0 ? ((char *)this) + keyvalueindex : NULL; }

	int					numlocalikautoplaylocks;
	int					localikautoplaylockindex;
	inline mstudioiklock_t *pLocalIKAutoplayLock(int i) const { Assert(i >= 0 && i < numlocalikautoplaylocks); return (mstudioiklock_t *)(((byte *)this) + localikautoplaylockindex) + i; };

	int					GetNumIKAutoplayLocks(void) const;
	const mstudioiklock_t &pIKAutoplayLock(int i);
	int					CountAutoplaySequences() const;
	int					CopyAutoplaySequences(unsigned short *pOut, int outCount) const;
	int					GetAutoplayList(unsigned short **pOut) const;

	// The collision model mass that jay wanted
	float				mass;
	int					contents;

	// external animations, models, etc.
	int					numincludemodels;
	int					includemodelindex;
	inline mstudiomodelgroup_t *pModelGroup(int i) const { Assert(i >= 0 && i < numincludemodels); return (mstudiomodelgroup_t *)(((byte *)this) + includemodelindex) + i; };
	// implementation specific call to get a named model
	const studiohdr_t	*FindModel(void **cache, char const *modelname) const;

	// implementation specific back pointer to virtual data
	mutable void		*virtualModel;
	virtualmodel_t		*GetVirtualModel(void) const;

	// for demand loaded animation blocks
	int					szanimblocknameindex;
	inline char * const pszAnimBlockName(void) const { return ((char *)this) + szanimblocknameindex; }
	int					numanimblocks;
	int					animblockindex;
	inline mstudioanimblock_t *pAnimBlock(int i) const { Assert(i > 0 && i < numanimblocks); return (mstudioanimblock_t *)(((byte *)this) + animblockindex) + i; };
	mutable void		*animblockModel;
	byte *				GetAnimBlock(int i) const;

	int					bonetablebynameindex;
	inline const byte	*GetBoneTableSortedByName() const { return (byte *)this + bonetablebynameindex; }

	// used by tools only that don't cache, but persist mdl's peer data
	// engine uses virtualModel to back link to cache pointers
	void				*pVertexBase;
	void				*pIndexBase;

	// if STUDIOHDR_FLAGS_CONSTANT_DIRECTIONAL_LIGHT_DOT is set,
	// this value is used to calculate directional components of lighting 
	// on static props
	byte				constdirectionallightdot;

	// set during load of mdl data to track *desired* lod configuration (not actual)
	// the *actual* clamped root lod is found in studiohwdata
	// this is stored here as a global store to ensure the staged loading matches the rendering
	byte				rootLOD;

	// set in the mdl data to specify that lod configuration should only allow first numAllowRootLODs
	// to be set as root LOD:
	//	numAllowedRootLODs = 0	means no restriction, any lod can be set as root lod.
	//	numAllowedRootLODs = N	means that lod0 - lod(N-1) can be set as root lod, but not lodN or lower.
	byte				numAllowedRootLODs;

	byte				unused[1];

	int					unused4; // zero out if version < 47

	int					numflexcontrollerui;
	int					flexcontrolleruiindex;
	mstudioflexcontrollerui_t *pFlexControllerUI(int i) const { Assert(i >= 0 && i < numflexcontrollerui); return (mstudioflexcontrollerui_t *)(((byte *)this) + flexcontrolleruiindex) + i; }

	float				flVertAnimFixedPointScale;
	inline float		VertAnimFixedPointScale() const { return (flags & STUDIOHDR_FLAGS_VERT_ANIM_FIXED_POINT_SCALE) ? flVertAnimFixedPointScale : 1.0f / 4096.0f; }

	int					unused3[1];

	// FIXME: Remove when we up the model version. Move all fields of studiohdr2_t into studiohdr_t.
	int					studiohdr2index;
	studiohdr2_t*		pStudioHdr2() const { return (studiohdr2_t *)(((byte *)this) + studiohdr2index); }

	// Src bone transforms are transformations that will convert .dmx or .smd-based animations into .mdl-based animations
	int					NumSrcBoneTransforms() const { return studiohdr2index ? pStudioHdr2()->numsrcbonetransform : 0; }
	const mstudiosrcbonetransform_t* SrcBoneTransform(int i) const { Assert(i >= 0 && i < NumSrcBoneTransforms()); return (mstudiosrcbonetransform_t *)(((byte *)this) + pStudioHdr2()->srcbonetransformindex) + i; }

	inline int			IllumPositionAttachmentIndex() const { return studiohdr2index ? pStudioHdr2()->IllumPositionAttachmentIndex() : 0; }

	inline float		MaxEyeDeflection() const { return studiohdr2index ? pStudioHdr2()->MaxEyeDeflection() : 0.866f; } // default to cos(30) if not set

	inline mstudiolinearbone_t *pLinearBones() const { return studiohdr2index ? pStudioHdr2()->pLinearBones() : NULL; }

	inline int			BoneFlexDriverCount() const { return studiohdr2index ? pStudioHdr2()->m_nBoneFlexDriverCount : 0; }
	inline const mstudioboneflexdriver_t* BoneFlexDriver(int i) const { Assert(i >= 0 && i < BoneFlexDriverCount()); return studiohdr2index ? pStudioHdr2()->pBoneFlexDriver(i) : NULL; }

	// NOTE: No room to add stuff? Up the .mdl file format version 
	// [and move all fields in studiohdr2_t into studiohdr_t and kill studiohdr2_t],
	// or add your stuff to studiohdr2_t. See NumSrcBoneTransforms/SrcBoneTransform for the pattern to use.
	int					unused2[1];

	studiohdr_t() {}

private:
	// No copy constructors allowed
	studiohdr_t(const studiohdr_t& vOther);

	friend struct virtualmodel_t;
};

// studio model data

struct studiomeshdata_t
{
	int					m_NumGroup;
	void*	            m_pMeshGroup;
};

struct studioloddata_t
{
	// not needed - this is really the same as studiohwdata_t.m_NumStudioMeshes
	//int					m_NumMeshes; 
	studiomeshdata_t	*m_pMeshData; // there are studiohwdata_t.m_NumStudioMeshes of these.
	float				m_SwitchPoint;
	// one of these for each lod since we can switch to simpler materials on lower lods.
	int					numMaterials;
	IMaterial			**ppMaterials; /* will have studiohdr_t.numtextures elements allocated */
									   // hack - this needs to go away.
	int					*pMaterialFlags; /* will have studiohdr_t.numtextures elements allocated */

										 // For decals on hardware morphing, we must actually do hardware skinning
										 // For this to work, we have to hope that the total # of bones used by
										 // hw flexed verts is < than the max possible for the dx level we're running under
	int					*m_pHWMorphDecalBoneRemap;
	int					m_nDecalBoneCount;
};

struct studiohwdata_t
{
	int					m_RootLOD;	// calced and clamped, nonzero for lod culling
	int					m_NumLODs;
	studioloddata_t		*m_pLODs;
	int					m_NumStudioMeshes;

	inline float LODMetric(float unitSphereSize) const { return (unitSphereSize != 0.0f) ? (100.0f / unitSphereSize) : 0.0f; }
	inline int GetLODForMetric(float lodMetric) const
	{
		if (!m_NumLODs)
			return 0;

		// shadow lod is specified on the last lod with a negative switch
		// never consider shadow lod as viable candidate
		int numLODs = (m_pLODs[m_NumLODs - 1].m_SwitchPoint < 0.0f) ? m_NumLODs - 1 : m_NumLODs;

		for (int i = m_RootLOD; i < numLODs - 1; i++)
		{
			if (m_pLODs[i + 1].m_SwitchPoint > lodMetric)
				return i;
		}

		return numLODs - 1;
	}
};

#define DECLARE_POINTER_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name

DECLARE_POINTER_HANDLE(StudioDecalHandle_t);
//-----------------------------------------------------------------------------
// Model rendering state
//-----------------------------------------------------------------------------
struct DrawModelState_t
{
	studiohdr_t*			m_pStudioHdr;
	studiohwdata_t*			m_pStudioHWData;
	IClientRenderable*		m_pRenderable;
	const matrix3x4_t		*m_pModelToWorld;
	StudioDecalHandle_t		m_decals;
	int						m_drawFlags;
	int						m_lod;
};

struct StaticPropRenderInfo_t
{
	const matrix3x4_t		*pModelToWorld;
	const model_t			*pModel;
	IClientRenderable		*pRenderable;
	Vector					*pLightingOrigin;
	short					skin;
	ModelInstanceHandle_t	instance;
};

enum
{
	ADDDECAL_TO_ALL_LODS = -1
};

class IVModelRender
{
public:
	virtual int		DrawModel(int flags,
		IClientRenderable *pRenderable,
		ModelInstanceHandle_t instance,
		int entity_index,
		const model_t *model,
		Vector const& origin,
		QAngle const& angles,
		int skin,
		int body,
		int hitboxset,
		const matrix3x4_t *modelToWorld = NULL,
		const matrix3x4_t *pLightingOffset = NULL) = 0;

	// This causes a material to be used when rendering the model instead 
	// of the materials the model was compiled with
	virtual void	ForcedMaterialOverride(IMaterial *newMaterial, OverrideType_t nOverrideType = (OverrideType_t)0) = 0;

	virtual void	SetViewTarget(const void *pStudioHdr, int nBodyIndex, const Vector& target) = 0;

	// Creates, destroys instance data to be associated with the model
	virtual ModelInstanceHandle_t CreateInstance(IClientRenderable *pRenderable, void *pCache = NULL) = 0;
	virtual void DestroyInstance(ModelInstanceHandle_t handle) = 0;

	// Associates a particular lighting condition with a model instance handle.
	// FIXME: This feature currently only works for static props. To make it work for entities, etc.,
	// we must clean up the lightcache handles as the model instances are removed.
	// At the moment, since only the static prop manager uses this, it cleans up all LightCacheHandles 
	// at level shutdown.
	virtual void SetStaticLighting(ModelInstanceHandle_t handle, void* pHandle) = 0;
	virtual void* GetStaticLighting(ModelInstanceHandle_t handle) = 0;

	// moves an existing InstanceHandle to a nex Renderable to keep decals etc. Models must be the same
	virtual bool ChangeInstance(ModelInstanceHandle_t handle, IClientRenderable *pRenderable) = 0;

	// Creates a decal on a model instance by doing a planar projection
	// along the ray. The material is the decal material, the radius is the
	// radius of the decal to create.
	virtual void AddDecal(ModelInstanceHandle_t handle, Ray_t const& ray,
		Vector const& decalUp, int decalIndex, int body, bool noPokeThru = false, int maxLODToDecal = ADDDECAL_TO_ALL_LODS) = 0;

	virtual void GMODAddDecal() = 0;//NOT SURE!!!!!!!!!!!!!!

	// Removes all the decals on a model instance
	virtual void RemoveAllDecals(ModelInstanceHandle_t handle) = 0;

	// Remove all decals from all models
	virtual void RemoveAllDecalsFromAllModels() = 0;

	// Shadow rendering, DrawModelShadowSetup returns the address of the bone-to-world array, NULL in case of error
	virtual matrix3x4_t* DrawModelShadowSetup() = 0;
	virtual void DrawModelShadow() = 0;

	// This gets called when overbright, etc gets changed to recompute static prop lighting.
	virtual bool RecomputeStaticLighting(ModelInstanceHandle_t handle) = 0;

	virtual void ReleaseAllStaticPropColorData(void) = 0;
	virtual void RestoreAllStaticPropColorData(void) = 0;

	// Extended version of drawmodel
	virtual int	DrawModelEx(ModelRenderInfo_t &pInfo) = 0;

	virtual int	DrawModelExStaticProp(ModelRenderInfo_t &pInfo) = 0;

	virtual bool DrawModelSetup(ModelRenderInfo_t &pInfo, DrawModelState_t *pState, matrix3x4_t *pCustomBoneToWorld, matrix3x4_t** ppBoneToWorldOut) = 0;
	virtual void DrawModelExecute(const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld = NULL) = 0;

	// Sets up lighting context for a point in space
	virtual void SetupLighting(const Vector &vecCenter) = 0;

	// doesn't support any debug visualization modes or other model options, but draws static props in the
	// fastest way possible
	virtual int DrawStaticPropArrayFast(StaticPropRenderInfo_t *pProps, int count, bool bShadowDepth) = 0;

	// Allow client to override lighting state
	virtual void SuppressEngineLighting(bool bSuppress) = 0;

	virtual void SetupColorMeshes(int nTotalVerts) = 0;

	virtual void AddColoredDecal(ModelInstanceHandle_t handle, Ray_t const& ray,
		Vector const& decalUp, int decalIndex, int body, Vector4D cColor, bool noPokeThru = false, int maxLODToDecal = ADDDECAL_TO_ALL_LODS) = 0;
};