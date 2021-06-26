#pragma once
class IMaterialProxyFactory;
class IVTFTexture;
//-----------------------------------------------------------------------------
// This will get called on procedural textures to re-fill the textures
// with the appropriate bit pattern. Calling Download() will also
// cause this interface to be called. It will also be called upon
// mode switch, or on other occasions where the bits are discarded.
//-----------------------------------------------------------------------------
class ITexture;
struct Rect_t;
enum ImageFormat;
enum NormalDecodeMode_t;
enum StencilOperation_t;
enum MaterialFogMode_t;
enum MaterialPrimitiveType_t;
enum MaterialNonInteractiveMode_t;
enum MaterialThreadMode_t;
enum MaterialNonInteractiveMode_t;
enum StencilComparisonFunction_t;
typedef unsigned int MorphFormat_t;
enum MaterialRenderTargetDepth_t;
typedef void(*MaterialBufferRestoreFunc_t)(int nChangeFlags);	// see RestoreChangeFlags_t


struct ImageFormatInfo_t
{
	char* m_pName;
	int m_NumBytes;
	int m_NumRedBits;
	int m_NumGreeBits;
	int m_NumBlueBits;
	int m_NumAlphaBits;
	bool m_IsCompressed;
};


enum Sampler_t
{
	SHADER_SAMPLER0 = 0,
	SHADER_SAMPLER1,
	SHADER_SAMPLER2,
	SHADER_SAMPLER3,
	SHADER_SAMPLER4,
	SHADER_SAMPLER5,
	SHADER_SAMPLER6,
	SHADER_SAMPLER7,
	SHADER_SAMPLER8,
	SHADER_SAMPLER9,
	SHADER_SAMPLER10,
	SHADER_SAMPLER11,
	SHADER_SAMPLER12,
	SHADER_SAMPLER13,
	SHADER_SAMPLER14,
	SHADER_SAMPLER15,
};


//-----------------------------------------------------------------------------
// the 3D shader API interface
// This interface is all that shaders see.
//-----------------------------------------------------------------------------
enum StandardTextureId_t
{
	// Lightmaps
	TEXTURE_LIGHTMAP = 0,
	TEXTURE_LIGHTMAP_FULLBRIGHT,
	TEXTURE_LIGHTMAP_BUMPED,
	TEXTURE_LIGHTMAP_BUMPED_FULLBRIGHT,

	// Flat colors
	TEXTURE_WHITE,
	TEXTURE_BLACK,
	TEXTURE_GREY,
	TEXTURE_GREY_ALPHA_ZERO,

	// Normalmaps
	TEXTURE_NORMALMAP_FLAT,

	// Normalization
	TEXTURE_NORMALIZATION_CUBEMAP,
	TEXTURE_NORMALIZATION_CUBEMAP_SIGNED,

	// Frame-buffer textures
	TEXTURE_FRAME_BUFFER_FULL_TEXTURE_0,
	TEXTURE_FRAME_BUFFER_FULL_TEXTURE_1,

	// Color correction
	TEXTURE_COLOR_CORRECTION_VOLUME_0,
	TEXTURE_COLOR_CORRECTION_VOLUME_1,
	TEXTURE_COLOR_CORRECTION_VOLUME_2,
	TEXTURE_COLOR_CORRECTION_VOLUME_3,

	// An alias to the Back Frame Buffer
	TEXTURE_FRAME_BUFFER_ALIAS,

	// Noise for shadow mapping algorithm
	TEXTURE_SHADOW_NOISE_2D,

	// A texture in which morph data gets accumulated (vs30, fast vertex textures required)
	TEXTURE_MORPH_ACCUMULATOR,

	// A texture which contains morph weights
	TEXTURE_MORPH_WEIGHTS,

	// A snapshot of the frame buffer's depth. Currently only valid on the 360
	TEXTURE_FRAME_BUFFER_FULL_DEPTH,

	// A snapshot of the frame buffer's depth. Currently only valid on the 360
	TEXTURE_IDENTITY_LIGHTWARP,

	TEXTURE_MAX_STD_TEXTURES = 32
};

//-----------------------------------------------------------------------------
// Vertex texture sampler identifiers
//-----------------------------------------------------------------------------
enum VertexTextureSampler_t
{
	SHADER_VERTEXTEXTURE_SAMPLER0 = 0,
	SHADER_VERTEXTEXTURE_SAMPLER1,
	SHADER_VERTEXTEXTURE_SAMPLER2,
	SHADER_VERTEXTEXTURE_SAMPLER3,
};

//-----------------------------------------------------------------------------
// Color correction info
//-----------------------------------------------------------------------------
struct ShaderColorCorrectionInfo_t
{
	bool m_bIsEnabled;
	int m_nLookupCount;
	float m_flDefaultWeight;
	float m_pLookupWeights[4];
};

class ITextureRegenerator
{
public:
	// This will be called when the texture bits need to be regenerated.
	// Use the VTFTexture interface, which has been set up with the
	// appropriate texture size + format
	// The rect specifies which part of the texture needs to be updated
	// You can choose to update all of the bits if you prefer
	virtual void RegenerateTextureBits(ITexture *pTexture, IVTFTexture *pVTFTexture, Rect_t *pRect) = 0;

	// This will be called when the regenerator needs to be deleted
	// which will happen when the texture is destroyed
	virtual void Release() = 0;
};

class ITexture
{
public:
	static IDirect3DTexture9* extract_texture(ITexture * texture) {
		return *(IDirect3DTexture9**)(**(DWORD**)((DWORD)texture + 0x40) + 0x48);
	}
	// Various texture polling methods
	virtual const char *GetName(void) const = 0;//0
	virtual int GetMappingWidth() const = 0;//1
	virtual int GetMappingHeight() const = 0;//2
	virtual int GetActualWidth() const = 0;//3
	virtual int GetActualHeight() const = 0;//4
	virtual int GetNumAnimationFrames() const = 0;//5
	virtual bool IsTranslucent() const = 0;//6
	virtual bool IsMipmapped() const = 0;//7

	virtual void GetLowResColorSample(float s, float t, float *color) const = 0;//8

	// Gets texture resource data of the specified type.
	// Params:
	//		eDataType		type of resource to retrieve.
	//		pnumBytes		on return is the number of bytes available in the read-only data buffer or is undefined
	// Returns:
	//		pointer to the resource data, or NULL
	virtual void *GetResourceData(uint32 eDataType, size_t *pNumBytes) const = 0;//9

	// Methods associated with reference count
	virtual void IncrementReferenceCount(void) = 0;//10
	virtual void DecrementReferenceCount(void) = 0;//11

	//inline void AddRef() { IncrementReferenceCount(); }
	//inline void Release() { DecrementReferenceCount(); }

	// Used to modify the texture bits (procedural textures only)
	virtual void SetTextureRegenerator(ITextureRegenerator *pTextureRegen) = 0;//12

	// Reconstruct the texture bits in HW memory

	// If rect is not specified, reconstruct all bits, otherwise just
	// reconstruct a subrect.
	virtual void Download(Rect_t *pRect = 0, int nAdditionalCreationFlags = 0) = 0;//13

	// Uses for stats. . .get the approximate size of the texture in it's current format.
	virtual int GetApproximateVidMemBytes(void) const = 0;//14

	// Returns true if the texture data couldn't be loaded.
	virtual bool IsError() const = 0;//15

	// NOTE: Stuff after this is added after shipping HL2.

	// For volume textures
	virtual bool IsVolumeTexture() const = 0;//16
	virtual int GetMappingDepth() const = 0;//17
	virtual int GetActualDepth() const = 0;//18

	virtual ImageFormat GetImageFormat() const = 0;//19
	virtual NormalDecodeMode_t GetNormalDecodeMode() const = 0;//20

	// Various information about the texture
	virtual bool IsRenderTarget() const = 0;//21
	virtual bool IsCubeMap() const = 0;//22
	virtual bool IsNormalMap() const = 0;//23
	virtual bool IsProcedural() const = 0;//24

	virtual void DeleteIfUnreferenced() = 0;//25


	// swap everything except the name with another texture
	virtual void SwapContents(ITexture *pOther) = 0;//26

	// Retrieve the vtf flags mask
	virtual unsigned int GetFlags(void) const = 0;//27

	// Force LOD override (automatically downloads the texture)
	virtual void ForceLODOverride(int iNumLodsOverrideUpOrDown) = 0;//28

	// Save texture to a file.
	virtual bool SaveToFile(const char *fileName) = 0;//29

	virtual void Bind(Sampler_t sampler) = 0;//30
	virtual void Bind(Sampler_t sampler1, int nFrame, Sampler_t sampler2 = (Sampler_t)-1) = 0;//31

	// Methods associated with reference counting
	virtual int GetReferenceCount() = 0;//32

	virtual void GetReflectivity(Vector& reflectivity) = 0;//33

	// Set this as the render target, return false for failure
	virtual bool SetRenderTarget(int nRenderTargetID) = 0;//34

	// Releases the texture's hw memory
	virtual void Release() = 0;

	// Called before Download() on restore. Gives render targets a change to change whether or
	// not they force themselves to have a separate depth buffer due to AA.
	virtual void OnRestore() = 0;

	// Resets the texture's filtering and clamping mode
	virtual void SetFilteringAndClampingMode() = 0;

	// Used by tools.... loads up the non-fallback information about the texture 
	virtual void Precache() = 0;

	// Stretch blit the framebuffer into this texture.
	virtual void CopyFrameBufferToMe(int nRenderTargetID = 0, Rect_t *pSrcRect = NULL, Rect_t *pDstRect = NULL) = 0;

	virtual void CopyMeToFrameBuffer(int nRenderTargetID = 0, Rect_t *pSrcRect = NULL, Rect_t *pDstRect = NULL) = 0;
};

class IMaterialSystemHardwareConfig;
class CShadowMgr;
class IShader;
//-----------------------------------------------------------------------------
// Special morph used in decalling pass
//-----------------------------------------------------------------------------
#define MATERIAL_MORPH_DECAL ( (IMorph*)1 )


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

class IMaterial;
struct MaterialSystem_SortInfo_t
{
	IMaterial	*material;
	int			lightmapPageID;
};


//-----------------------------------------------------------------------------
// Video mode info..
//-----------------------------------------------------------------------------
struct MaterialVideoMode_t
{
	int m_Width;			// if width and height are 0 and you select 
	int m_Height;			// windowed mode, it'll use the window size
	ImageFormat m_Format;	// use ImageFormats (ignored for windowed mode)
	int m_RefreshRate;		// 0 == default (ignored for windowed mode)
};

// fixme: should move this into something else.
struct FlashlightState_t
{
	FlashlightState_t()
	{
		m_bEnableShadows = false;						// Provide reasonable defaults for shadow depth mapping parameters
		m_bDrawShadowFrustum = false;
		m_flShadowMapResolution = 1024.0f;
		m_flShadowFilterSize = 3.0f;
		m_flShadowSlopeScaleDepthBias = 16.0f;
		m_flShadowDepthBias = 0.0005f;
		m_flShadowJitterSeed = 0.0f;
		m_flShadowAtten = 0.0f;
		m_bScissor = false;
		m_nLeft = -1;
		m_nTop = -1;
		m_nRight = -1;
		m_nBottom = -1;
		m_nShadowQuality = 0;
	}

	Vector m_vecLightOrigin;
	Vector m_quatOrientation;
	float m_NearZ;
	float m_FarZ;
	float m_fHorizontalFOVDegrees;
	float m_fVerticalFOVDegrees;
	float m_fQuadraticAtten;
	float m_fLinearAtten;
	float m_fConstantAtten;
	float m_Color[4];
	ITexture *m_pSpotlightTexture;
	int m_nSpotlightTextureFrame;

	// Shadow depth mapping parameters
	bool  m_bEnableShadows;
	bool  m_bDrawShadowFrustum;
	float m_flShadowMapResolution;
	float m_flShadowFilterSize;
	float m_flShadowSlopeScaleDepthBias;
	float m_flShadowDepthBias;
	float m_flShadowJitterSeed;
	float m_flShadowAtten;
	int   m_nShadowQuality;

	// Getters for scissor members
	bool DoScissor() { return m_bScissor; }
	int GetLeft() { return m_nLeft; }
	int GetTop() { return m_nTop; }
	int GetRight() { return m_nRight; }
	int GetBottom() { return m_nBottom; }

private:

	friend class CShadowMgr;

	bool m_bScissor;
	int m_nLeft;
	int m_nTop;
	int m_nRight;
	int m_nBottom;
};

//DECLARE_POINTER_HANDLE(OcclusionQueryObjectHandle_t);
#define INVALID_OCCLUSION_QUERY_OBJECT_HANDLE ( (OcclusionQueryObjectHandle_t)0 )


//DECLARE_POINTER_HANDLE(MaterialLock_t);
typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);
enum InitReturnVal_t;
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
class IAppSystem
{
public:
	// Here's where the app systems get to learn about each other 
	virtual bool Connect(CreateInterfaceFn factory) = 0;
	virtual void Disconnect() = 0;

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void *QueryInterface(const char *pInterfaceName) = 0;

	// Init, shutdown
	virtual InitReturnVal_t Init() = 0;
	virtual void Shutdown() = 0;
};
struct MaterialSystem_Config_t
{
	/*bool Windowed() const { return (m_Flags & MATSYS_VIDCFG_FLAGS_WINDOWED) != 0; }
	bool Resizing() const { return (m_Flags & MATSYS_VIDCFG_FLAGS_RESIZING) != 0; }

	bool WaitForVSync() const { return (m_Flags & MATSYS_VIDCFG_FLAGS_NO_WAIT_FOR_VSYNC) == 0; }

	bool Stencil() const { return (m_Flags & MATSYS_VIDCFG_FLAGS_STENCIL) != 0; }
	bool ForceTrilinear() const { return (m_Flags & MATSYS_VIDCFG_FLAGS_FORCE_TRILINEAR) != 0; }
	bool ForceHWSync() const { return (m_Flags & MATSYS_VIDCFG_FLAGS_FORCE_HWSYNC) != 0; }
	bool UseSpecular() const { return (m_Flags & MATSYS_VIDCFG_FLAGS_DISABLE_SPECULAR) == 0; }
	bool UseBumpmapping() const { return (m_Flags & MATSYS_VIDCFG_FLAGS_DISABLE_BUMPMAP) == 0; }
	bool UseParallaxMapping() const { return (m_Flags & MATSYS_VIDCFG_FLAGS_ENABLE_PARALLAX_MAPPING) != 0; }
	bool UseZPrefill() const { return (m_Flags & MATSYS_VIDCFG_FLAGS_USE_Z_PREFILL) != 0; }
	bool ReduceFillrate() const { return (m_Flags & MATSYS_VIDCFG_FLAGS_REDUCE_FILLRATE) != 0; }
	bool HDREnabled() const { return (m_Flags & MATSYS_VIDCFG_FLAGS_ENABLE_HDR) != 0; }
	bool LimitWindowedSize() const { return (m_Flags & MATSYS_VIDCFG_FLAGS_LIMIT_WINDOWED_SIZE) != 0; }
	bool ScaleToOutputResolution() const { return (m_Flags & MATSYS_VIDCFG_FLAGS_SCALE_TO_OUTPUT_RESOLUTION) != 0; }
	bool UsingMultipleWindows() const { return (m_Flags & MATSYS_VIDCFG_FLAGS_USING_MULTIPLE_WINDOWS) != 0; }
	bool UsePhong() const { return (m_Flags & MATSYS_VIDCFG_FLAGS_DISABLE_PHONG) == 0; }
	bool VRMode() const { return (m_Flags & MATSYS_VIDCFG_FLAGS_VR_MODE) != 0; }
	bool ShadowDepthTexture() const { return m_bShadowDepthTexture; }
	bool MotionBlur() const { return m_bMotionBlur; }
	bool SupportFlashlight() const { return m_bSupportFlashlight; }
	*/
	void SetFlag(unsigned int flag, bool val)
	{
		if (val)
		{
			m_Flags |= flag;
		}
		else
		{
			m_Flags &= ~flag;
		}
	}

	// control panel stuff
	MaterialVideoMode_t m_VideoMode;
	float m_fMonitorGamma;
	float m_fGammaTVRangeMin;
	float m_fGammaTVRangeMax;
	float m_fGammaTVExponent;
	bool m_bGammaTVEnabled;

	int m_nAASamples;
	int m_nForceAnisotropicLevel;
	int skipMipLevels;
	int dxSupportLevel;
	unsigned int m_Flags;
	bool bEditMode;				// true if in Hammer.
	unsigned char proxiesTestMode;	// 0 = normal, 1 = no proxies, 2 = alpha test all, 3 = color mod all
	bool bCompressedTextures;
	bool bFilterLightmaps;
	bool bFilterTextures;
	bool bReverseDepth;
	bool bBufferPrimitives;
	bool bDrawFlat;
	bool bMeasureFillRate;
	bool bVisualizeFillRate;
	bool bNoTransparency;
	bool bSoftwareLighting;
	bool bAllowCheats;
	char nShowMipLevels;
	bool bShowLowResImage;
	bool bShowNormalMap;
	bool bMipMapTextures;
	unsigned char nFullbright;
	bool m_bFastNoBump;
	bool m_bSuppressRendering;

	// debug modes
	bool bShowSpecular; // This is the fast version that doesn't require reloading materials
	bool bShowDiffuse;  // This is the fast version that doesn't require reloading materials

						// misc
	int m_nReserved;	// Currently unused

						// No depth bias
	float m_SlopeScaleDepthBias_Normal;
	float m_DepthBias_Normal;

	// Depth bias for rendering decals closer to the camera
	float m_SlopeScaleDepthBias_Decal;
	float m_DepthBias_Decal;

	// Depth bias for biasing shadow depth map rendering away from the camera
	float m_SlopeScaleDepthBias_ShadowMap;
	float m_DepthBias_ShadowMap;

	uint m_WindowedSizeLimitWidth;
	uint m_WindowedSizeLimitHeight;
	int m_nAAQuality;
	bool m_bShadowDepthTexture;
	bool m_bMotionBlur;
	bool m_bSupportFlashlight;

	int m_nVRModeAdapter;
	char big_buff[0x100];
	MaterialSystem_Config_t()
	{
		memset(this, 0, sizeof(*this));

		m_VideoMode.m_Width = 640;
		m_VideoMode.m_Height = 480;
		m_VideoMode.m_RefreshRate = 60;
		dxSupportLevel = 0;
		bCompressedTextures = true;
		bFilterTextures = true;
		bFilterLightmaps = true;
		bMipMapTextures = true;
		bBufferPrimitives = true;

		m_fMonitorGamma = 2.2f;
		m_fGammaTVRangeMin = 16.0f;
		m_fGammaTVRangeMax = 255.0f;
		m_fGammaTVExponent = 2.5;
		m_bGammaTVEnabled = IsX360();

		m_nAASamples = 1;
		m_bShadowDepthTexture = false;
		m_bMotionBlur = false;
		m_bSupportFlashlight = true;

		m_nVRModeAdapter = -1;

		// misc defaults
		bAllowCheats = false;
		bCompressedTextures = true;
		bEditMode = false;

		// debug modes
		bShowSpecular = true;
		bShowDiffuse = true;
		nFullbright = 0;
		bShowNormalMap = false;
		bFilterLightmaps = true;
		bFilterTextures = true;
		bMipMapTextures = true;
		nShowMipLevels = 0;
		bShowLowResImage = false;
		bReverseDepth = false;
		bBufferPrimitives = true;
		bDrawFlat = false;
		bMeasureFillRate = false;
		bVisualizeFillRate = false;
		bSoftwareLighting = false;
		bNoTransparency = false;
		proxiesTestMode = 0;
		m_bFastNoBump = false;
		m_bSuppressRendering = false;
		m_SlopeScaleDepthBias_Decal = -0.5f;
		m_SlopeScaleDepthBias_Normal = 0.0f;
		m_SlopeScaleDepthBias_ShadowMap = 0.5f;
		m_DepthBias_Decal = -262144;
		m_DepthBias_Normal = 0.0f;
		m_DepthBias_ShadowMap = 262144;
		m_WindowedSizeLimitWidth = 1280;
		m_WindowedSizeLimitHeight = 1024;
	}
};
enum LightType_t;
struct LightDesc_t
{
	LightType_t m_Type;										//< MATERIAL_LIGHT_xxx
	Vector m_Color;											//< color+intensity 
	Vector m_Position;										//< light source center position
	Vector m_Direction;										//< for SPOT, direction it is pointing
	float  m_Range;											//< distance range for light.0=infinite
	float m_Falloff;										//< angular falloff exponent for spot lights
	float m_Attenuation0;									//< constant distance falloff term
	float m_Attenuation1;									//< linear term of falloff
	float m_Attenuation2;									//< quadatic term of falloff
	float m_Theta;											//< inner cone angle. no angular falloff 
															//< within this cone
	float m_Phi;											//< outer cone angle

															// the values below are derived from the above settings for optimizations
															// These aren't used by DX8. . used for software lighting.
	float m_ThetaDot;
	float m_PhiDot;
	unsigned int m_Flags;
protected:
	float OneOver_ThetaDot_Minus_PhiDot;
	float m_RangeSquared;
public:
};
enum VertexCompressionType_t;
struct VertexDesc_t
{
	// These can be set to zero if there are pointers to dummy buffers, when the
	// actual buffer format doesn't contain the data but it needs to be safe to
	// use all the CMeshBuilder functions.
	int	m_VertexSize_Position;
	int m_VertexSize_BoneWeight;
	int m_VertexSize_BoneMatrixIndex;
	int	m_VertexSize_Normal;
	int	m_VertexSize_Color;
	int	m_VertexSize_Specular;
	int m_VertexSize_TexCoord[8];
	int m_VertexSize_TangentS;
	int m_VertexSize_TangentT;
	int m_VertexSize_Wrinkle;

	int m_VertexSize_UserData;

	int m_ActualVertexSize;	// Size of the vertices.. Some of the m_VertexSize_ elements above
							// are set to this value and some are set to zero depending on which
							// fields exist in a buffer's vertex format.

							// The type of compression applied to this vertex data
	VertexCompressionType_t m_CompressionType;

	// Number of bone weights per vertex...
	int m_NumBoneWeights;

	// Pointers to our current vertex data
	float			*m_pPosition;

	float			*m_pBoneWeight;

#ifndef NEW_SKINNING
	unsigned char	*m_pBoneMatrixIndex;
#else
	float			*m_pBoneMatrixIndex;
#endif

	float			*m_pNormal;

	unsigned char	*m_pColor;
	unsigned char	*m_pSpecular;
	float			*m_pTexCoord[8];

	// Tangent space *associated with one particular set of texcoords*
	float			*m_pTangentS;
	float			*m_pTangentT;

	float			*m_pWrinkle;

	// user data
	float			*m_pUserData;

	// The first vertex index (used for buffered vertex buffers, or cards that don't support stream offset)
	int	m_nFirstVertex;

	// The offset in bytes of the memory we're writing into 
	// from the start of the D3D buffer (will be 0 for static meshes)
	unsigned int	m_nOffset;

};
typedef uint64_t VertexFormat_t;
class IVertexBuffer
{
public:
	// Add a virtual destructor to silence the clang warning.
	// This is harmless but not important since the only derived class
	// doesn't have a destructor.
	virtual ~IVertexBuffer() {}

	// NOTE: The following two methods are only valid for static vertex buffers
	// Returns the number of vertices and the format of the vertex buffer
	virtual int VertexCount() const = 0;
	virtual VertexFormat_t GetVertexFormat() const = 0;

	// Is this vertex buffer dynamic?
	virtual bool IsDynamic() const = 0;

	// NOTE: For dynamic vertex buffers only!
	// Casts the memory of the dynamic vertex buffer to the appropriate type
	virtual void BeginCastBuffer(VertexFormat_t format) = 0;
	virtual void EndCastBuffer() = 0;

	// Returns the number of vertices that can still be written into the buffer
	virtual int GetRoomRemaining() const = 0;

	virtual bool Lock(int nVertexCount, bool bAppend, VertexDesc_t &desc) = 0;
	virtual void Unlock(int nVertexCount, VertexDesc_t &desc) = 0;

	// Spews the mesh data
	virtual void Spew(int nVertexCount, const VertexDesc_t &desc) = 0;

	// Call this in debug mode to make sure our data is good.
	virtual void ValidateData(int nVertexCount, const VertexDesc_t & desc) = 0;
};
struct IndexDesc_t
{
	// Pointers to the index data
	unsigned short	*m_pIndices;

	// The offset in bytes of the memory we're writing into 
	// from the start of the D3D buffer (will be 0 for static meshes)
	unsigned int	m_nOffset;

	// The first index (used for buffered index buffers, or cards that don't support stream offset)
	unsigned int	m_nFirstIndex;

	// 1 if the device is active, 0 if the device isn't active.
	// Faster than doing if checks for null m_pIndices if someone is
	// trying to write the m_pIndices while the device is inactive.
	unsigned char m_nIndexSize;
};
enum MaterialIndexFormat_t;
class IIndexBuffer
{
public:
	// Add a virtual destructor to silence the clang warning.
	// This is harmless but not important since the only derived class
	// doesn't have a destructor.
	virtual ~IIndexBuffer() {}

	// NOTE: The following two methods are only valid for static index buffers
	// Returns the number of indices and the format of the index buffer
	virtual int IndexCount() const = 0;
	virtual MaterialIndexFormat_t IndexFormat() const = 0;

	// Is this index buffer dynamic?
	virtual bool IsDynamic() const = 0;

	// NOTE: For dynamic index buffers only!
	// Casts the memory of the dynamic index buffer to the appropriate type
	virtual void BeginCastBuffer(MaterialIndexFormat_t format) = 0;
	virtual void EndCastBuffer() = 0;

	// Returns the number of indices that can still be written into the buffer
	virtual int GetRoomRemaining() const = 0;

	// Locks, unlocks the index buffer
	virtual bool Lock(int nMaxIndexCount, bool bAppend, IndexDesc_t &desc) = 0;
	virtual void Unlock(int nWrittenIndexCount, IndexDesc_t &desc) = 0;

	// FIXME: Remove this!!
	// Locks, unlocks the index buffer for modify
	virtual void ModifyBegin(bool bReadOnly, int nFirstIndex, int nIndexCount, IndexDesc_t& desc) = 0;
	virtual void ModifyEnd(IndexDesc_t& desc) = 0;

	// Spews the mesh data
	virtual void Spew(int nIndexCount, const IndexDesc_t &desc) = 0;

	// Ensures the data in the index buffer is valid
	virtual void ValidateData(int nIndexCount, const IndexDesc_t &desc) = 0;
};
//-----------------------------------------------------------------------------
// Used in lists of indexed primitives.
//-----------------------------------------------------------------------------
class CPrimList
{
public:
	CPrimList();
	CPrimList(int nFirstIndex, int nIndexCount);

	int			m_FirstIndex;
	int			m_NumIndices;
};

inline CPrimList::CPrimList()
{
}

inline CPrimList::CPrimList(int nFirstIndex, int nIndexCount)
{
	m_FirstIndex = nFirstIndex;
	m_NumIndices = nIndexCount;
}
//-----------------------------------------------------------------------------
// The Mesh memory descriptor
//-----------------------------------------------------------------------------
struct MeshDesc_t : public VertexDesc_t, public IndexDesc_t
{
};
class CMeshBuilder;
class IMesh : public IVertexBuffer, public IIndexBuffer
{
public:
	// -----------------------------------

	// Sets/gets the primitive type
	virtual void SetPrimitiveType(MaterialPrimitiveType_t type) = 0;

	// Draws the mesh
	virtual void Draw(int nFirstIndex = -1, int nIndexCount = 0) = 0;

	virtual void SetColorMesh(IMesh *pColorMesh, int nVertexOffset) = 0;

	// Draw a list of (lists of) primitives. Batching your lists together that use
	// the same lightmap, material, vertex and index buffers with multipass shaders
	// can drastically reduce state-switching overhead.
	// NOTE: this only works with STATIC meshes.
	virtual void Draw(CPrimList *pLists, int nLists) = 0;

	// Copy verts and/or indices to a mesh builder. This only works for temp meshes!
	virtual void CopyToMeshBuilder(
		int iStartVert,		// Which vertices to copy.
		int nVerts,
		int iStartIndex,	// Which indices to copy.
		int nIndices,
		int indexOffset,	// This is added to each index.
		CMeshBuilder &builder) = 0;

	// Spews the mesh data
	virtual void Spew(int nVertexCount, int nIndexCount, const MeshDesc_t &desc) = 0;

	// Call this in debug mode to make sure our data is good.
	virtual void ValidateData(int nVertexCount, int nIndexCount, const MeshDesc_t &desc) = 0;

	// New version
	// Locks/unlocks the mesh, providing space for nVertexCount and nIndexCount.
	// nIndexCount of -1 means don't lock the index buffer...
	virtual void LockMesh(int nVertexCount, int nIndexCount, MeshDesc_t &desc) = 0;
	virtual void ModifyBegin(int nFirstVertex, int nVertexCount, int nFirstIndex, int nIndexCount, MeshDesc_t& desc) = 0;
	virtual void ModifyEnd(MeshDesc_t& desc) = 0;
	virtual void UnlockMesh(int nVertexCount, int nIndexCount, MeshDesc_t &desc) = 0;

	virtual void ModifyBeginEx(bool bReadOnly, int nFirstVertex, int nVertexCount, int nFirstIndex, int nIndexCount, MeshDesc_t &desc) = 0;

	virtual void SetFlexMesh(IMesh *pMesh, int nVertexOffset) = 0;

	virtual void DisableFlexMesh() = 0;

	virtual void MarkAsDrawn() = 0;

	virtual unsigned ComputeMemoryUsed() = 0;
};

using OcclusionQueryObjectHandle_t = void*;

class IMorph;
struct MorphWeight_t;
enum MaterialMatrixMode_t
{
	MATERIAL_VIEW = 0,
	MATERIAL_PROJECTION,

	// Texture matrices
	MATERIAL_TEXTURE0,
	MATERIAL_TEXTURE1,
	MATERIAL_TEXTURE2,
	MATERIAL_TEXTURE3,
	MATERIAL_TEXTURE4,
	MATERIAL_TEXTURE5,
	MATERIAL_TEXTURE6,
	MATERIAL_TEXTURE7,

	MATERIAL_MODEL,

	// Total number of matrices
	NUM_MATRIX_MODES = MATERIAL_MODEL + 1,

	// Number of texture transforms
	NUM_TEXTURE_TRANSFORMS = MATERIAL_TEXTURE7 - MATERIAL_TEXTURE0 + 1
};
enum MaterialHeightClipMode_t;
enum MaterialCullMode_t;
class ICallQueue;
struct DeformationBase_t;
typedef unsigned int ColorCorrectionHandle_t;
//: public IRefCounted
class IMatRenderContext 
{
public:
	virtual void				BeginRender() = 0;
	virtual void				EndRender() = 0;

	virtual void				Flush(bool flushHardware = false) = 0;

	virtual void				BindLocalCubemap(ITexture *pTexture) = 0;

	// pass in an ITexture (that is build with "rendertarget" "1") or
	// pass in NULL for the regular backbuffer.
	virtual void				SetRenderTarget(ITexture *pTexture) = 0;
	virtual ITexture *			GetRenderTarget(void) = 0;

	virtual void				GetRenderTargetDimensions(int &width, int &height) const = 0;

	// Bind a material is current for rendering.
	virtual void				Bind(IMaterial *material, void *proxyData = 0) = 0;
	// Bind a lightmap page current for rendering.  You only have to 
	// do this for materials that require lightmaps.
	virtual void				BindLightmapPage(int lightmapPageID) = 0;

	// inputs are between 0 and 1
	virtual void				DepthRange(float zNear, float zFar) = 0;

	virtual void				ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil = false) = 0;

	// read to a unsigned char rgb image.
	virtual void				ReadPixels(int x, int y, int width, int height, unsigned char *data, ImageFormat dstFormat) = 0;

	// Sets lighting
	virtual void				SetAmbientLight(float r, float g, float b) = 0;
	virtual void				SetLight(int lightNum, const LightDesc_t& desc) = 0;

	// The faces of the cube are specified in the same order as cubemap textures
	virtual void				SetAmbientLightCube(Vector4D cube[6]) = 0;

	// Blit the backbuffer to the framebuffer texture
	virtual void				CopyRenderTargetToTexture(ITexture *pTexture) = 0;

	// Set the current texture that is a copy of the framebuffer.
	virtual void				SetFrameBufferCopyTexture(ITexture *pTexture, int textureIndex = 0) = 0;
	virtual ITexture		   *GetFrameBufferCopyTexture(int textureIndex) = 0;

	//
	// end vertex array api
	//

	// matrix api
	virtual void				MatrixMode(MaterialMatrixMode_t matrixMode) = 0;
	virtual void				PushMatrix(void) = 0;
	virtual void				PopMatrix(void) = 0;
	virtual void				LoadMatrix(VMatrix const& matrix) = 0;
	virtual void				LoadMatrix(matrix3x4_t const& matrix) = 0;
	virtual void				MultMatrix(VMatrix const& matrix) = 0;
	virtual void				MultMatrix(matrix3x4_t const& matrix) = 0;
	virtual void				MultMatrixLocal(VMatrix const& matrix) = 0;
	virtual void				MultMatrixLocal(matrix3x4_t const& matrix) = 0;
	virtual void				GetMatrix(MaterialMatrixMode_t matrixMode, VMatrix *matrix) = 0;
	virtual void				GetMatrix(MaterialMatrixMode_t matrixMode, matrix3x4_t *matrix) = 0;
	virtual void				LoadIdentity(void) = 0;
	virtual void				Ortho(double left, double top, double right, double bottom, double zNear, double zFar) = 0;
	virtual void				PerspectiveX(double fovx, double aspect, double zNear, double zFar) = 0;
	virtual void				PickMatrix(int x, int y, int width, int height) = 0;
	virtual void				Rotate(float angle, float x, float y, float z) = 0;
	virtual void				Translate(float x, float y, float z) = 0;
	virtual void				Scale(float x, float y, float z) = 0;
	// end matrix api

	// Sets/gets the viewport
	virtual void				Viewport(int x, int y, int width, int height) = 0;
	virtual void				GetViewport(int& x, int& y, int& width, int& height) const = 0;

	// The cull mode
	virtual void				CullMode(MaterialCullMode_t cullMode) = 0;

	// end matrix api

	// This could easily be extended to a general user clip plane
	virtual void				SetHeightClipMode(MaterialHeightClipMode_t nHeightClipMode) = 0;
	// garymcthack : fog z is always used for heightclipz for now.
	virtual void				SetHeightClipZ(float z) = 0;

	// Fog methods...
	virtual void				FogMode(MaterialFogMode_t fogMode) = 0;
	virtual void				FogStart(float fStart) = 0;
	virtual void				FogEnd(float fEnd) = 0;
	virtual void				SetFogZ(float fogZ) = 0;
	virtual MaterialFogMode_t	GetFogMode(void) = 0;

	virtual void				FogColor3f(float r, float g, float b) = 0;
	virtual void				FogColor3fv(float const* rgb) = 0;
	virtual void				FogColor3ub(unsigned char r, unsigned char g, unsigned char b) = 0;
	virtual void				FogColor3ubv(unsigned char const* rgb) = 0;

	virtual void				GetFogColor(unsigned char *rgb) = 0;

	// Sets the number of bones for skinning
	virtual void				SetNumBoneWeights(int numBones) = 0;

	// Creates/destroys Mesh
	virtual IMesh* CreateStaticMesh(VertexFormat_t fmt, const char *pTextureBudgetGroup, IMaterial * pMaterial = NULL) = 0;
	virtual void DestroyStaticMesh(IMesh* mesh) = 0;

	// Gets the dynamic mesh associated with the currently bound material
	// note that you've got to render the mesh before calling this function 
	// a second time. Clients should *not* call DestroyStaticMesh on the mesh 
	// returned by this call.
	// Use buffered = false if you want to not have the mesh be buffered,
	// but use it instead in the following pattern:
	//		meshBuilder.Begin
	//		meshBuilder.End
	//		Draw partial
	//		Draw partial
	//		Draw partial
	//		meshBuilder.Begin
	//		meshBuilder.End
	//		etc
	// Use Vertex or Index Override to supply a static vertex or index buffer
	// to use in place of the dynamic buffers.
	//
	// If you pass in a material in pAutoBind, it will automatically bind the
	// material. This can be helpful since you must bind the material you're
	// going to use BEFORE calling GetDynamicMesh.
	virtual IMesh* GetDynamicMesh(
		bool buffered = true,
		IMesh* pVertexOverride = 0,
		IMesh* pIndexOverride = 0,
		IMaterial *pAutoBind = 0) = 0;

	// ------------ New Vertex/Index Buffer interface ----------------------------
	// Do we need support for bForceTempMesh and bSoftwareVertexShader?
	// I don't think we use bSoftwareVertexShader anymore. .need to look into bForceTempMesh.
	virtual IVertexBuffer *CreateStaticVertexBuffer(VertexFormat_t fmt, int nVertexCount, const char *pTextureBudgetGroup) = 0;
	virtual IIndexBuffer *CreateStaticIndexBuffer(MaterialIndexFormat_t fmt, int nIndexCount, const char *pTextureBudgetGroup) = 0;
	virtual void DestroyVertexBuffer(IVertexBuffer *) = 0;
	virtual void DestroyIndexBuffer(IIndexBuffer *) = 0;
	// Do we need to specify the stream here in the case of locking multiple dynamic VBs on different streams?
	virtual IVertexBuffer *GetDynamicVertexBuffer(int streamID, VertexFormat_t vertexFormat, bool bBuffered = true) = 0;
	virtual IIndexBuffer *GetDynamicIndexBuffer(MaterialIndexFormat_t fmt, bool bBuffered = true) = 0;
	virtual void BindVertexBuffer(int streamID, IVertexBuffer *pVertexBuffer, int nOffsetInBytes, int nFirstVertex, int nVertexCount, VertexFormat_t fmt, int nRepetitions = 1) = 0;
	virtual void BindIndexBuffer(IIndexBuffer *pIndexBuffer, int nOffsetInBytes) = 0;
	virtual void Draw(MaterialPrimitiveType_t primitiveType, int firstIndex, int numIndices) = 0;
	// ------------ End ----------------------------

	// Selection mode methods
	virtual int  SelectionMode(bool selectionMode) = 0;
	virtual void SelectionBuffer(unsigned int* pBuffer, int size) = 0;
	virtual void ClearSelectionNames() = 0;
	virtual void LoadSelectionName(int name) = 0;
	virtual void PushSelectionName(int name) = 0;
	virtual void PopSelectionName() = 0;

	// Sets the Clear Color for ClearBuffer....
	virtual void		ClearColor3ub(unsigned char r, unsigned char g, unsigned char b) = 0;
	virtual void		ClearColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a) = 0;

	// Allows us to override the depth buffer setting of a material
	virtual void	OverrideDepthEnable(bool bEnable, bool bDepthEnable) = 0;

	// FIXME: This is a hack required for NVidia/XBox, can they fix in drivers?
	virtual void	DrawScreenSpaceQuad(IMaterial* pMaterial) = 0;

	// For debugging and building recording files. This will stuff a token into the recording file,
	// then someone doing a playback can watch for the token.
	virtual void	SyncToken(const char *pToken) = 0;

	// FIXME: REMOVE THIS FUNCTION!
	// The only reason why it's not gone is because we're a week from ship when I found the bug in it
	// and everything's tuned to use it.
	// It's returning values which are 2x too big (it's returning sphere diameter x2)
	// Use ComputePixelDiameterOfSphere below in all new code instead.
	virtual float	ComputePixelWidthOfSphere(const Vector& origin, float flRadius) = 0;

	//
	// Occlusion query support
	//

	// Allocate and delete query objects.
	virtual OcclusionQueryObjectHandle_t CreateOcclusionQueryObject(void) = 0;
	virtual void DestroyOcclusionQueryObject(OcclusionQueryObjectHandle_t) = 0;

	// Bracket drawing with begin and end so that we can get counts next frame.
	virtual void BeginOcclusionQueryDrawing(OcclusionQueryObjectHandle_t) = 0;
	virtual void EndOcclusionQueryDrawing(OcclusionQueryObjectHandle_t) = 0;

	// Get the number of pixels rendered between begin and end on an earlier frame.
	// Calling this in the same frame is a huge perf hit!
	virtual int OcclusionQuery_GetNumPixelsRendered(OcclusionQueryObjectHandle_t) = 0;

	virtual void SetFlashlightMode(bool bEnable) = 0;

	virtual void SetFlashlightState(const FlashlightState_t &state, const VMatrix &worldToTexture) = 0;

	// Gets the current height clip mode
	virtual MaterialHeightClipMode_t GetHeightClipMode() = 0;

	// This returns the diameter of the sphere in pixels based on 
	// the current model, view, + projection matrices and viewport.
	virtual float	ComputePixelDiameterOfSphere(const Vector& vecAbsOrigin, float flRadius) = 0;

	// By default, the material system applies the VIEW and PROJECTION matrices	to the user clip
	// planes (which are specified in world space) to generate projection-space user clip planes
	// Occasionally (for the particle system in hl2, for example), we want to override that
	// behavior and explictly specify a ViewProj transform for user clip planes
	virtual void	EnableUserClipTransformOverride(bool bEnable) = 0;
	virtual void	UserClipTransform(const VMatrix &worldToView) = 0;

	virtual bool GetFlashlightMode() const = 0;

	// Used to make the handle think it's never had a successful query before
	virtual void ResetOcclusionQueryObject(OcclusionQueryObjectHandle_t) = 0;

	// FIXME: Remove
	virtual void Unused3() {}

	// Creates/destroys morph data associated w/ a particular material
	virtual IMorph *CreateMorph(MorphFormat_t format, const char *pDebugName) = 0;
	virtual void DestroyMorph(IMorph *pMorph) = 0;

	// Binds the morph data for use in rendering
	virtual void BindMorph(IMorph *pMorph) = 0;

	// Sets flexweights for rendering
	virtual void SetFlexWeights(int nFirstWeight, int nCount, const MorphWeight_t* pWeights) = 0;

	// FIXME: Remove
	virtual void Unused4() {};
	virtual void Unused5() {};
	virtual void Unused6() {};
	virtual void Unused7() {};
	virtual void Unused8() {};

	// Read w/ stretch to a host-memory buffer
	virtual void ReadPixelsAndStretch(Rect_t *pSrcRect, Rect_t *pDstRect, unsigned char *pBuffer, ImageFormat dstFormat, int nDstStride) = 0;

	// Gets the window size
	virtual void GetWindowSize(int &width, int &height) const = 0;

	// This function performs a texture map from one texture map to the render destination, doing
	// all the necessary pixel/texel coordinate fix ups. fractional values can be used for the
	// src_texture coordinates to get linear sampling - integer values should produce 1:1 mappings
	// for non-scaled operations.
	virtual void DrawScreenSpaceRectangle(
		IMaterial *pMaterial,
		int destx, int desty,
		int width, int height,
		float src_texture_x0, float src_texture_y0,			// which texel you want to appear at
															// destx/y
		float src_texture_x1, float src_texture_y1,			// which texel you want to appear at
															// destx+width-1, desty+height-1
		int src_texture_width, int src_texture_height,		// needed for fixup
		void *pClientRenderable = NULL,
		int nXDice = 1,
		int nYDice = 1) = 0;

	virtual void LoadBoneMatrix(int boneIndex, const matrix3x4_t& matrix) = 0;

	// This version will push the current rendertarget + current viewport onto the stack
	virtual void PushRenderTargetAndViewport() = 0;

	// This version will push a new rendertarget + a maximal viewport for that rendertarget onto the stack
	virtual void PushRenderTargetAndViewport(ITexture *pTexture) = 0;

	// This version will push a new rendertarget + a specified viewport onto the stack
	virtual void PushRenderTargetAndViewport(ITexture *pTexture, int nViewX, int nViewY, int nViewW, int nViewH) = 0;

	// This version will push a new rendertarget + a specified viewport onto the stack
	virtual void PushRenderTargetAndViewport(ITexture *pTexture, ITexture *pDepthTexture, int nViewX, int nViewY, int nViewW, int nViewH) = 0;

	// This will pop a rendertarget + viewport
	virtual void PopRenderTargetAndViewport(void) = 0;

	// Binds a particular texture as the current lightmap
	virtual void BindLightmapTexture(ITexture *pLightmapTexture) = 0;

	// Blit a subrect of the current render target to another texture
	virtual void CopyRenderTargetToTextureEx(ITexture *pTexture, int nRenderTargetID, Rect_t *pSrcRect, Rect_t *pDstRect = NULL) = 0;
	virtual void CopyTextureToRenderTargetEx(int nRenderTargetID, ITexture *pTexture, Rect_t *pSrcRect, Rect_t *pDstRect = NULL) = 0;

	// Special off-center perspective matrix for DoF, MSAA jitter and poster rendering
	virtual void PerspectiveOffCenterX(double fovx, double aspect, double zNear, double zFar, double bottom, double top, double left, double right) = 0;

	// Rendering parameters control special drawing modes withing the material system, shader
	// system, shaders, and engine. renderparm.h has their definitions.
	virtual void SetFloatRenderingParameter(int parm_number, float value) = 0;
	virtual void SetIntRenderingParameter(int parm_number, int value) = 0;
	virtual void SetVectorRenderingParameter(int parm_number, Vector const &value) = 0;

	// stencil buffer operations.
	virtual void SetStencilEnable(bool onoff) = 0;
	virtual void SetStencilFailOperation(StencilOperation_t op) = 0;
	virtual void SetStencilZFailOperation(StencilOperation_t op) = 0;
	virtual void SetStencilPassOperation(StencilOperation_t op) = 0;
	virtual void SetStencilCompareFunction(StencilComparisonFunction_t cmpfn) = 0;
	virtual void SetStencilReferenceValue(int ref) = 0;
	virtual void SetStencilTestMask(uint32 msk) = 0;
	virtual void SetStencilWriteMask(uint32 msk) = 0;
	virtual void ClearStencilBufferRectangle(int xmin, int ymin, int xmax, int ymax,int value) = 0;

	virtual void SetRenderTargetEx(int nRenderTargetID, ITexture *pTexture) = 0;

	// rendering clip planes, beware that only the most recently pushed plane will actually be used in a sizeable chunk of hardware configurations
	// and that changes to the clip planes mid-frame while UsingFastClipping() is true will result unresolvable depth inconsistencies
	virtual void PushCustomClipPlane(const float *pPlane) = 0;
	virtual void PopCustomClipPlane(void) = 0;

	// Returns the number of vertices + indices we can render using the dynamic mesh
	// Passing true in the second parameter will return the max # of vertices + indices
	// we can use before a flush is provoked and may return different values 
	// if called multiple times in succession. 
	// Passing false into the second parameter will return
	// the maximum possible vertices + indices that can be rendered in a single batch
	virtual void GetMaxToRender(IMesh *pMesh, bool bMaxUntilFlush, int *pMaxVerts, int *pMaxIndices) = 0;

	// Returns the max possible vertices + indices to render in a single draw call
	virtual int GetMaxVerticesToRender(IMaterial *pMaterial) = 0;
	virtual int GetMaxIndicesToRender() = 0;
	virtual void DisableAllLocalLights() = 0;
	virtual int CompareMaterialCombos(IMaterial *pMaterial1, IMaterial *pMaterial2, int lightMapID1, int lightMapID2) = 0;

	virtual IMesh *GetFlexMesh() = 0;

	virtual void SetFlashlightStateEx(const FlashlightState_t &state, const VMatrix &worldToTexture, ITexture *pFlashlightDepthTexture) = 0;

	// Returns the currently bound local cubemap
	virtual ITexture *GetLocalCubemap() = 0;

	// This is a version of clear buffers which will only clear the buffer at pixels which pass the stencil test
	virtual void ClearBuffersObeyStencil(bool bClearColor, bool bClearDepth) = 0;

	//enables/disables all entered clipping planes, returns the input from the last time it was called.
	virtual bool EnableClipping(bool bEnable) = 0;

	//get fog distances entered with FogStart(), FogEnd(), and SetFogZ()
	virtual void GetFogDistances(float *fStart, float *fEnd, float *fFogZ) = 0;

	// Hooks for firing PIX events from outside the Material System...
	virtual void BeginPIXEvent(unsigned long color, const char *szName) = 0;
	virtual void EndPIXEvent() = 0;
	virtual void SetPIXMarker(unsigned long color, const char *szName) = 0;

	// Batch API
	// from changelist 166623:
	// - replaced obtuse material system batch usage with an explicit and easier to thread API
	virtual void BeginBatch(IMesh* pIndices) = 0;
	virtual void BindBatch(IMesh* pVertices, IMaterial *pAutoBind = NULL) = 0;
	virtual void DrawBatch(int firstIndex, int numIndices) = 0;
	virtual void EndBatch() = 0;

	// Raw access to the call queue, which can be NULL if not in a queued mode
	virtual ICallQueue *GetCallQueue() = 0;

	// Returns the world-space camera position
	virtual void GetWorldSpaceCameraPosition(Vector *pCameraPos) = 0;
	virtual void GetWorldSpaceCameraVectors(Vector *pVecForward, Vector *pVecRight, Vector *pVecUp) = 0;

	// Tone mapping
	virtual void				ResetToneMappingScale(float monoscale) = 0; 			// set scale to monoscale instantly with no chasing
	virtual void				SetGoalToneMappingScale(float monoscale) = 0; 			// set scale to monoscale instantly with no chasing

																						// call TurnOnToneMapping before drawing the 3d scene to get the proper interpolated brightness
																						// value set.
	virtual void				TurnOnToneMapping() = 0;

	// Set a linear vector color scale for all 3D rendering.
	// A value of [1.0f, 1.0f, 1.0f] should match non-tone-mapped rendering.
	virtual void				SetToneMappingScaleLinear(const Vector &scale) = 0;

	virtual Vector				GetToneMappingScaleLinear(void) = 0;
	virtual void				SetShadowDepthBiasFactors(float fSlopeScaleDepthBias, float fDepthBias) = 0;

	// Apply stencil operations to every pixel on the screen without disturbing depth or color buffers
	virtual void				PerformFullScreenStencilOperation(void) = 0;

	// Sets lighting origin for the current model (needed to convert directional lights to points)
	virtual void				SetLightingOrigin(Vector vLightingOrigin) = 0;

	// Set scissor rect for rendering
	virtual void				SetScissorRect(const int nLeft, const int nTop, const int nRight, const int nBottom, const bool bEnableScissor) = 0;

	// Methods used to build the morph accumulator that is read from when HW morph<ing is enabled.
	virtual void				BeginMorphAccumulation() = 0;
	virtual void				EndMorphAccumulation() = 0;
	virtual void				AccumulateMorph(IMorph* pMorph, int nMorphCount, const MorphWeight_t* pWeights) = 0;

	virtual void				PushDeformation(DeformationBase_t const *Deformation) = 0;
	virtual void				PopDeformation() = 0;
	virtual int					GetNumActiveDeformations() const = 0;

	virtual bool				GetMorphAccumulatorTexCoord(Vector2D *pTexCoord, IMorph *pMorph, int nVertex) = 0;

	// Version of get dynamic mesh that specifies a specific vertex format
	virtual IMesh*				GetDynamicMeshEx(VertexFormat_t vertexFormat, bool bBuffered = true,
		IMesh* pVertexOverride = 0,	IMesh* pIndexOverride = 0, IMaterial *pAutoBind = 0) = 0;

	virtual void				FogMaxDensity(float flMaxDensity) = 0;

	virtual IMaterial *GetCurrentMaterial() = 0;
	virtual int  GetCurrentNumBones() const = 0;
	virtual void *GetCurrentProxy() = 0;

	// Color correction related methods..
	// Client cannot call IColorCorrectionSystem directly because it is not thread-safe
	// FIXME: Make IColorCorrectionSystem threadsafe?
	virtual void EnableColorCorrection(bool bEnable) = 0;
	virtual ColorCorrectionHandle_t AddLookup(const char *pName) = 0;
	virtual bool RemoveLookup(ColorCorrectionHandle_t handle) = 0;
	virtual void LockLookup(ColorCorrectionHandle_t handle) = 0;
	virtual void LoadLookup(ColorCorrectionHandle_t handle, const char *pLookupName) = 0;
	virtual void UnlockLookup(ColorCorrectionHandle_t handle) = 0;
	virtual void SetLookupWeight(ColorCorrectionHandle_t handle, float flWeight) = 0;
	virtual void ResetLookupWeights() = 0;
	virtual void SetResetable(ColorCorrectionHandle_t handle, bool bResetable) = 0;

	//There are some cases where it's simply not reasonable to update the full screen depth texture (mostly on PC).
	//Use this to mark it as invalid and use a dummy texture for depth reads.
	virtual void SetFullScreenDepthTextureValidityFlag(bool bIsValid) = 0;

	// A special path used to tick the front buffer while loading on the 360
	virtual void SetNonInteractivePacifierTexture(ITexture *pTexture, float flNormalizedX, float flNormalizedY, float flNormalizedSize) = 0;
	virtual void SetNonInteractiveTempFullscreenBuffer(ITexture *pTexture, MaterialNonInteractiveMode_t mode) = 0;
	virtual void EnableNonInteractiveMode(MaterialNonInteractiveMode_t mode) = 0;
	virtual void RefreshFrontBufferNonInteractive() = 0;
	// Allocates temp render data. Renderdata goes out of scope at frame end in multicore
	// Renderdata goes out of scope after refcount goes to zero in singlecore.
	// Locking/unlocking increases + decreases refcount
	virtual void *			LockRenderData(int nSizeInBytes) = 0;
	virtual void			UnlockRenderData(void *pData) = 0;

	// Typed version. If specified, pSrcData is copied into the locked memory.
	template< class E > E*  LockRenderDataTyped(int nCount, const E* pSrcData = NULL);

	// Temp render data gets immediately freed after it's all unlocked in single core.
	// This prevents it from being freed
	virtual void			AddRefRenderData() = 0;
	virtual void			ReleaseRenderData() = 0;

	// Returns whether a pointer is render data. NOTE: passing NULL returns true
	virtual bool			IsRenderData(const void *pData) const = 0;
	virtual void			PrintfVA(char *fmt, va_list vargs) = 0;
	virtual void			Printf( const char *fmt, ...) = 0;
	virtual float			Knob(char *knobname, float *setvalue = NULL) = 0;
	// Allows us to override the alpha write setting of a material
	virtual void OverrideAlphaWriteEnable(bool bEnable, bool bAlphaWriteEnable) = 0;
	virtual void OverrideColorWriteEnable(bool bOverrideEnable, bool bColorWriteEnable) = 0;

	virtual void ClearBuffersObeyStencilEx(bool bClearColor, bool bClearAlpha, bool bClearDepth) = 0;//193

	virtual int GMOD_ForceFilterMode(bool, int) = 0;//194

	virtual void				GMOD_FlushQueue() = 0;//195

	virtual int OverrideBlend(bool a2, bool a3, int a4, int a5, int a6) = 0;//196

	virtual int OverrideBlendSeparateAlpha(bool a2, bool a3, int a4, int a5, int a6) = 0;//197

	virtual float GetFloatRenderingParameter(int parm_number) = 0;//198

	virtual int GetIntRenderingParameter(int parm_number) = 0;//199

	virtual Vector GetVectorRenderingParameter(int parm_number) = 0;//200
	
	virtual void	SwapBuffers() = 0;//201

	virtual void	SetCurrentMaterialInternal(IMaterial* pCurrentMaterial) = 0;//202

	virtual IMaterial*						GetCurrentMaterialInternal() = 0;//203

	virtual int					GetLightmapPage(void) = 0;//204

	virtual void				ForceDepthFuncEquals(bool a1) = 0;//205

	virtual bool				InFlashlightMode() = 0;//206

	virtual void BindStandardTexture(Sampler_t, StandardTextureId_t) = 0;//207

	virtual void GetLightmapDimensions(int *, int *) = 0;//208

	virtual MorphFormat_t GetBoundMorphFormat() = 0;//209

	virtual ITexture *  GetRenderTargetEx(int) = 0;//210
	/*
211	CMatRenderContext::DrawClearBufferQuad
212	CMatRenderContext::OnDrawMesh
213	CMatRenderContext::OnDrawMesh
214	CMatRenderContext::OnSetFlexMesh
215	CMatRenderContext::OnSetColorMesh
216	CMatRenderContext::OnSetPrimitiveType
217	CMatRenderContext::OnFlushBufferedPrimitives
218	CMatRenderContext::SyncMatrices
219	CMatRenderContext::SyncMatrix
220	CMatRenderContext::ForceHardwareSync
221	CMatRenderContext::BeginFrame
222	CMatRenderContext::EndFrame
223	CMatRenderContextBase::SetFrameTime
224	CMatRenderContextBase::SetCurrentProxy
225	CMatRenderContextBase::MarkRenderDataUnused
226	CMatRenderContextBase::GetCallQueueInternal
227	CMatRenderContext::DoStartupShaderPreloading
228	IMatRenderContextInternal::GMOD_IsLowOnMemory
229	CMatRenderContextBase::InitializeFrom
230	CMatRenderContext::UpdateHeightClipUserClipPlane
231	CMatRenderContext::ApplyCustomClipPlanes
232	CMatRenderContext::CommitRenderTargetAndViewport
233	CMatRenderContext::OnRenderDataUnreferenced
234	CMatRenderContext::BindStandardVertexTexture
235	CMatRenderContext::GetStandardTextureDimensions
	
	*/

};



class PIXEvent
{
public:
	PIXEvent(IMatRenderContext *pRenderContext, const char *szName, unsigned long color = 0xFFFFFFFF)
	{
		m_pRenderContext = pRenderContext;
		m_pRenderContext->BeginPIXEvent(color, szName);
	}
	~PIXEvent()
	{
		m_pRenderContext->EndPIXEvent();
	}
private:
	IMatRenderContext *m_pRenderContext;
};
template <class T>
class CBaseAutoPtr
{
public:
	CBaseAutoPtr() : m_pObject(0) {}
	CBaseAutoPtr(T *pFrom) : m_pObject(pFrom) {}

	operator const void *() const { return m_pObject; }
	operator void *() { return m_pObject; }

	operator const T *() const { return m_pObject; }
	operator const T *() { return m_pObject; }
	operator T *() { return m_pObject; }

	int			operator=(int i) { AssertMsg(i == 0, "Only NULL allowed on integer assign"); m_pObject = 0; return 0; }
	T *			operator=(T *p) { m_pObject = p; return p; }

	bool        operator !() const { return (!m_pObject); }
	bool        operator!=(int i) const { AssertMsg(i == 0, "Only NULL allowed on integer compare"); return (m_pObject != NULL); }
	bool		operator==(const void *p) const { return (m_pObject == p); }
	bool		operator!=(const void *p) const { return (m_pObject != p); }
	bool		operator==(T *p) const { return operator==((void *)p); }
	bool		operator!=(T *p) const { return operator!=((void *)p); }
	bool		operator==(const CBaseAutoPtr<T> &p) const { return operator==((const void *)p); }
	bool		operator!=(const CBaseAutoPtr<T> &p) const { return operator!=((const void *)p); }

	T *  		operator->() { return m_pObject; }
	T &  		operator *() { return *m_pObject; }
	T ** 		operator &() { return &m_pObject; }

	const T *   operator->() const { return m_pObject; }
	const T &   operator *() const { return *m_pObject; }
	T * const * operator &() const { return &m_pObject; }

protected:
	CBaseAutoPtr(const CBaseAutoPtr<T> &from) : m_pObject(from.m_pObject) {}
	void operator=(const CBaseAutoPtr<T> &from) { m_pObject = from.m_pObject; }

	T *m_pObject;
};
//-----------------------------------------------------------------------------
template <class T>
class CRefPtr : public CBaseAutoPtr<T>
{
	typedef CBaseAutoPtr<T> BaseClass;
public:
	CRefPtr() {}
	CRefPtr(T *pInit) : BaseClass(pInit) {}
	CRefPtr(const CRefPtr<T> &from) : BaseClass(from) {}
	~CRefPtr() { if (BaseClass::m_pObject) BaseClass::m_pObject->Release(); }

	void operator=(const CRefPtr<T> &from) { BaseClass::operator=(from); }

	int operator=(int i) { return BaseClass::operator=(i); }
	T *operator=(T *p) { return BaseClass::operator=(p); }

	operator bool() const { return !BaseClass::operator!(); }
	operator bool() { return !BaseClass::operator!(); }

	void SafeRelease() { if (BaseClass::m_pObject) BaseClass::m_pObject->Release(); BaseClass::m_pObject = 0; }
	void AssignAddRef(T *pFrom) { SafeRelease(); if (pFrom) pFrom->AddRef(); BaseClass::m_pObject = pFrom; }
	void AddRefAssignTo(T *&pTo) { ::SafeRelease(pTo); if (BaseClass::m_pObject) BaseClass::m_pObject->AddRef(); pTo = BaseClass::m_pObject; }
};


struct MaterialAdapterInfo_t;
enum RenderTargetSizeMode_t;
typedef void(*ModeChangeCallbackFunc_t)(void);
struct MaterialSystemHardwareIdentifier_t;
enum HDRType_t;
typedef void(*MaterialBufferReleaseFunc_t)();
typedef unsigned short MaterialHandle_t;



class IMaterialSystem : public IAppSystem
{
public:

	// Placeholder for API revision
	virtual bool Connect(CreateInterfaceFn factory) = 0;
	virtual void Disconnect() = 0;
	virtual void *QueryInterface(const char *pInterfaceName) = 0;
	virtual InitReturnVal_t Init() = 0;
	virtual void Shutdown() = 0;

	//---------------------------------------------------------
	// Initialization and shutdown
	//---------------------------------------------------------

	// Call this to initialize the material system
	// returns a method to create interfaces in the shader dll
	virtual CreateInterfaceFn	Init(char const* pShaderAPIDLL,
		IMaterialProxyFactory *pMaterialProxyFactory,
		void* fileSystemFactory,
		void*  cvarFactory = NULL) = 0;

	// Call this to set an explicit shader version to use 
	// Must be called before Init().
	virtual void				SetShaderAPI(char const *pShaderAPIDLL) = 0;

	// Must be called before Init(), if you're going to call it at all...
	virtual void				SetAdapter(int nAdapter, int nFlags) = 0;

	// Call this when the mod has been set up, which may occur after init
	// At this point, the game + gamebin paths have been set up
	virtual void				ModInit() = 0;
	virtual void				ModShutdown() = 0;

	//---------------------------------------------------------
	//
	//---------------------------------------------------------
	virtual void					SetThreadMode(MaterialThreadMode_t mode, int nServiceThread = -1) = 0;
	virtual MaterialThreadMode_t	GetThreadMode() = 0;
	virtual bool					IsRenderThreadSafe() = 0;
	virtual void					ExecuteQueued() = 0;

	//---------------------------------------------------------
	// Config management
	//---------------------------------------------------------

	virtual IMaterialSystemHardwareConfig *GetHardwareConfig(const char *pVersion, int *returnCode) = 0;


	// Call this before rendering each frame with the current config
	// for the material system.
	// Will do whatever is necessary to get the material system into the correct state
	// upon configuration change. .doesn't much else otherwise.
	virtual bool				UpdateConfig(bool bForceUpdate) = 0;

	// Force this to be the config; update all material system convars to match the state
	// return true if lightmaps need to be redownloaded
	virtual bool				OverrideConfig(const MaterialSystem_Config_t &config, bool bForceUpdate) = 0;

	// Get the current config for this video card (as last set by UpdateConfig)
	virtual const MaterialSystem_Config_t &GetCurrentConfigForVideoCard() const = 0;

	// Gets *recommended* configuration information associated with the display card, 
	// given a particular dx level to run under. 
	// Use dxlevel 0 to use the recommended dx level.
	// The function returns false if an invalid dxlevel was specified

	// UNDONE: To find out all convars affected by configuration, we'll need to change
	// the dxsupport.pl program to output all column headers into a single keyvalue block
	// and then we would read that in, and send it back to the client
	virtual bool				GetRecommendedConfigurationInfo(int nDXLevel, void * pKeyValues) = 0;


	// -----------------------------------------------------------
	// Device methods
	// -----------------------------------------------------------

	// Gets the number of adapters...
	virtual int					GetDisplayAdapterCount() const = 0;

	// Returns the current adapter in use
	virtual int					GetCurrentAdapter() const = 0;

	// Returns info about each adapter
	virtual void				GetDisplayAdapterInfo(int adapter, MaterialAdapterInfo_t& info) const = 0;

	// Returns the number of modes
	virtual int					GetModeCount(int adapter) const = 0;

	// Returns mode information..
	virtual void				GetModeInfo(int adapter, int mode, MaterialVideoMode_t& info) const = 0;

	virtual void				AddModeChangeCallBack(ModeChangeCallbackFunc_t func) = 0;

	// Returns the mode info for the current display device
	virtual void				GetDisplayMode(MaterialVideoMode_t& mode) const = 0;

	// Sets the mode...
	virtual bool				SetMode(void* hwnd, const MaterialSystem_Config_t &config) = 0;

	virtual bool				SupportsMSAAMode(int nMSAAMode) = 0;

	// FIXME: REMOVE! Get video card identitier
	virtual const MaterialSystemHardwareIdentifier_t &GetVideoCardIdentifier(void) const = 0;

	// Use this to spew information about the 3D layer 
	virtual void				SpewDriverInfo() const = 0;

	virtual void				GetDXLevelDefaults(uint &max_dxlevel,uint &recommended_dxlevel) = 0;

	// Get the image format of the back buffer. . useful when creating render targets, etc.
	virtual void				GetBackBufferDimensions(int &width, int &height) const = 0;
	virtual ImageFormat			GetBackBufferFormat() const = 0;

	virtual bool				SupportsHDRMode(HDRType_t nHDRModede) = 0;


	// -----------------------------------------------------------
	// Window methods
	// -----------------------------------------------------------

	// Creates/ destroys a child window
	virtual bool				AddView(void* hwnd) = 0;
	virtual void				RemoveView(void* hwnd) = 0;

	// Sets the view
	virtual void				SetView(void* hwnd) = 0;


	// -----------------------------------------------------------
	// Control flow
	// -----------------------------------------------------------

	virtual void				BeginFrame(float frameTime) = 0;
	virtual void				EndFrame() = 0;
	virtual void				Flush(bool flushHardware = false) = 0;

	/// FIXME: This stuff needs to be cleaned up and abstracted.
	// Stuff that gets exported to the launcher through the engine
	virtual void				SwapBuffers() = 0;

	// Flushes managed textures from the texture cacher
	virtual void				EvictManagedResources() = 0;

	virtual void				ReleaseResources(void) = 0;
	virtual void				ReacquireResources(void) = 0;


	// -----------------------------------------------------------
	// Device loss/restore
	// -----------------------------------------------------------

	// Installs a function to be called when we need to release vertex buffers + textures
	virtual void				AddReleaseFunc(MaterialBufferReleaseFunc_t func) = 0;
	virtual void				RemoveReleaseFunc(MaterialBufferReleaseFunc_t func) = 0;

	// Installs a function to be called when we need to restore vertex buffers
	virtual void				AddRestoreFunc(MaterialBufferRestoreFunc_t func) = 0;
	virtual void				RemoveRestoreFunc(MaterialBufferRestoreFunc_t func) = 0;

	// Release temporary HW memory...
	virtual void				ResetTempHWMemory(bool bExitingLevel = false) = 0;

	// For dealing with device lost in cases where SwapBuffers isn't called all the time (Hammer)
	virtual void				HandleDeviceLost() = 0;


	// -----------------------------------------------------------
	// Shaders
	// -----------------------------------------------------------

	// Used to iterate over all shaders for editing purposes
	// GetShaders returns the number of shaders it actually found
	virtual int					ShaderCount() const = 0;
	virtual int					GetShaders(int nFirstShader, int nMaxCount, IShader **ppShaderList) const = 0;

	// FIXME: Is there a better way of doing this?
	// Returns shader flag names for editors to be able to edit them
	virtual int					ShaderFlagCount() const = 0;
	virtual const char *		ShaderFlagName(int nIndex) const = 0;

	// Gets the actual shader fallback for a particular shader
	virtual void				GetShaderFallback(const char *pShaderName, char *pFallbackShader, int nFallbackLength) = 0;


	// -----------------------------------------------------------
	// Material proxies
	// -----------------------------------------------------------

	virtual IMaterialProxyFactory *GetMaterialProxyFactory() = 0;

	// Sets the material proxy factory. Calling this causes all materials to be uncached.
	virtual void				SetMaterialProxyFactory(IMaterialProxyFactory* pFactory) = 0;


	// -----------------------------------------------------------
	// Editor mode
	// -----------------------------------------------------------

	// Used to enable editor materials. Must be called before Init.
	virtual void				EnableEditorMaterials() = 0;


	// -----------------------------------------------------------
	// Stub mode mode
	// -----------------------------------------------------------

	// Force it to ignore Draw calls.
	virtual void				SetInStubMode(bool bInStubMode) = 0;


	//---------------------------------------------------------
	// Debug support
	//---------------------------------------------------------

	virtual void				DebugPrintUsedMaterials(const char *pSearchSubString, bool bVerbose) = 0;
	virtual void				DebugPrintUsedTextures(void) = 0;

	virtual void				ToggleSuppressMaterial(char const* pMaterialName) = 0;
	virtual void				ToggleDebugMaterial(char const* pMaterialName) = 0;


	//---------------------------------------------------------
	// Misc features
	//---------------------------------------------------------
	//returns whether fast clipping is being used or not - needed to be exposed for better per-object clip behavior
	virtual bool				UsingFastClipping(void) = 0;

	virtual int					StencilBufferBits(void) = 0; //number of bits per pixel in the stencil buffer


															 //---------------------------------------------------------
															 // Material and texture management
															 //---------------------------------------------------------

															 // uncache all materials. .  good for forcing reload of materials.
	virtual void				UncacheAllMaterials() = 0;

	// Remove any materials from memory that aren't in use as determined
	// by the IMaterial's reference count.
	virtual void				UncacheUnusedMaterials(bool bRecomputeStateSnapshots = false) = 0;

	// Load any materials into memory that are to be used as determined
	// by the IMaterial's reference count.
	virtual void				CacheUsedMaterials() = 0;

	// Force all textures to be reloaded from disk.
	virtual void				ReloadTextures() = 0;

	// Reloads materials
	virtual void				ReloadMaterials(const char *pSubString = NULL) = 0;

	// Create a procedural material. The keyvalues looks like a VMT file
	virtual IMaterial *			CreateMaterial(const char *pMaterialName, void *pVMTKeyValues) = 0;

	// Find a material by name.
	// The name of a material is a full path to 
	// the vmt file starting from "hl2/materials" (or equivalent) without
	// a file extension.
	// eg. "dev/dev_bumptest" refers to somethign similar to:
	// "d:/hl2/hl2/materials/dev/dev_bumptest.vmt"
	//
	// Most of the texture groups for pTextureGroupName are listed in texture_group_names.h.
	// 
	// Note: if the material can't be found, this returns a checkerboard material. You can 
	// find out if you have that material by calling IMaterial::IsErrorMaterial().
	// (Or use the global IsErrorMaterial function, which checks if it's null too).
	virtual IMaterial *			FindMaterial(char const* pMaterialName, const char *pTextureGroupName, bool complain = true, const char *pComplainPrefix = NULL) = 0;

	// Query whether a material is loaded (eg, whether FindMaterial will be nonblocking)
	virtual bool				IsMaterialLoaded(char const* pMaterialName) = 0;

	//---------------------------------
	// This is the interface for knowing what materials are available
	// is to use the following functions to get a list of materials.  The
	// material names will have the full path to the material, and that is the 
	// only way that the directory structure of the materials will be seen through this
	// interface.
	// NOTE:  This is mostly for worldcraft to get a list of materials to put
	// in the "texture" browser.in Worldcraft
	virtual MaterialHandle_t	FirstMaterial() const = 0;

	// returns InvalidMaterial if there isn't another material.
	// WARNING: you must call GetNextMaterial until it returns NULL, 
	// otherwise there will be a memory leak.
	virtual MaterialHandle_t	NextMaterial(MaterialHandle_t h) const = 0;

	// This is the invalid material
	virtual MaterialHandle_t	InvalidMaterial() const = 0;

	// Returns a particular material
	virtual IMaterial*			GetMaterial(MaterialHandle_t h) const = 0;

	// Get the total number of materials in the system.  These aren't just the used
	// materials, but the complete collection.
	virtual int					GetNumMaterials() const = 0;

	//---------------------------------

	virtual void				SetAsyncTextureLoadCache(void* hFileCache) = 0;

	virtual ITexture *			FindTexture(char const* pTextureName, const char *pTextureGroupName, bool complain = true, int nAdditionalCreationFlags = 0) = 0;

	// Checks to see if a particular texture is loaded
	virtual bool				IsTextureLoaded(char const* pTextureName) const = 0;

	// Creates a procedural texture
	virtual ITexture *			CreateProceduralTexture(const char	*pTextureName,
		const char *pTextureGroupName,
		int w,
		int h,
		ImageFormat fmt,
		int nFlags) = 0;

	//
	// Render targets
	//
	virtual void				BeginRenderTargetAllocation() = 0;
	virtual void				EndRenderTargetAllocation() = 0; // Simulate an Alt-Tab in here, which causes a release/restore of all resources

																 // Creates a render target
																 // If depth == true, a depth buffer is also allocated. If not, then
																 // the screen's depth buffer is used.
																 // Creates a texture for use as a render target
	virtual ITexture *			CreateRenderTargetTexture(int w,
		int h,
		RenderTargetSizeMode_t sizeMode,	// Controls how size is generated (and regenerated on video mode change).
		ImageFormat	format,
		MaterialRenderTargetDepth_t depth = (MaterialRenderTargetDepth_t)0) = 0;

	virtual ITexture *			CreateNamedRenderTargetTextureEx(const char *pRTName,				// Pass in NULL here for an unnamed render target.
		int w,
		int h,
		RenderTargetSizeMode_t sizeMode,	// Controls how size is generated (and regenerated on video mode change).
		ImageFormat format,
		MaterialRenderTargetDepth_t depth = (MaterialRenderTargetDepth_t)0,
		unsigned int textureFlags = 0 | 8,
		unsigned int renderTargetFlags = 0) = 0;

	virtual ITexture *			CreateNamedRenderTargetTexture(const char *pRTName,
		int w,
		int h,
		RenderTargetSizeMode_t sizeMode,	// Controls how size is generated (and regenerated on video mode change).
		ImageFormat format,
		MaterialRenderTargetDepth_t depth = (MaterialRenderTargetDepth_t)0,
		bool bClampTexCoords = true,
		bool bAutoMipMap = false) = 0;

	// Must be called between the above Begin-End calls!
	virtual ITexture *			CreateNamedRenderTargetTextureEx2(const char *pRTName,				// Pass in NULL here for an unnamed render target.
		int w,
		int h,
		RenderTargetSizeMode_t sizeMode,	// Controls how size is generated (and regenerated on video mode change).
		ImageFormat format,
		MaterialRenderTargetDepth_t depth = (MaterialRenderTargetDepth_t)0,
		unsigned int textureFlags = 4 | 8,
		unsigned int renderTargetFlags = 0) = 0;

	// -----------------------------------------------------------
	// Lightmaps
	// -----------------------------------------------------------

	// To allocate lightmaps, sort the whole world by material twice.
	// The first time through, call AllocateLightmap for every surface.
	// that has a lightmap.
	// The second time through, call AllocateWhiteLightmap for every 
	// surface that expects to use shaders that expect lightmaps.
	virtual void				BeginLightmapAllocation() = 0;
	virtual void				EndLightmapAllocation() = 0;

	// returns the sorting id for this surface
	virtual int 				AllocateLightmap(int width, int height,
		int offsetIntoLightmapPage[2],
		IMaterial *pMaterial) = 0;
	// returns the sorting id for this surface
	virtual int					AllocateWhiteLightmap(IMaterial *pMaterial) = 0;

	// lightmaps are in linear color space
	// lightmapPageID is returned by GetLightmapPageIDForSortID
	// lightmapSize and offsetIntoLightmapPage are returned by AllocateLightmap.
	// You should never call UpdateLightmap for a lightmap allocated through
	// AllocateWhiteLightmap.
	virtual void				UpdateLightmap(int lightmapPageID, int lightmapSize[2],
		int offsetIntoLightmapPage[2],
		float *pFloatImage, float *pFloatImageBump1,
		float *pFloatImageBump2, float *pFloatImageBump3) = 0;

	// fixme: could just be an array of ints for lightmapPageIDs since the material
	// for a surface is already known.
	virtual int					GetNumSortIDs() = 0;
	virtual void				GetSortInfo(MaterialSystem_SortInfo_t *sortInfoArray) = 0;

	// Read the page size of an existing lightmap by sort id (returned from AllocateLightmap())
	virtual void				GetLightmapPageSize(int lightmap, int *width, int *height) const = 0;

	virtual void				ResetMaterialLightmapPageInfo() = 0;



	virtual void				ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil = false) = 0;

	// -----------------------------------------------------------
	// Access the render contexts
	// -----------------------------------------------------------
	virtual IMatRenderContext *	GetRenderContext() = 0;//98

	virtual bool				SupportsShadowDepthTextures(void) = 0;//99
	virtual void				BeginUpdateLightmaps(void) = 0;//100
	virtual void				EndUpdateLightmaps(void) = 0;//101

	// -----------------------------------------------------------
	// Methods to force the material system into non-threaded, non-queued mode
	// -----------------------------------------------------------
	virtual void*		Lock() = 0;//102
	virtual void				Unlock(void*) = 0;//103

	// Vendor-dependent shadow depth texture format
	virtual ImageFormat			GetShadowDepthTextureFormat() = 0;//104

	virtual bool				SupportsFetch4(void) = 0;//105

	virtual bool				SupportsCSAAMode(int nNumSamples, int nQualityLevel) = 0;//106

	virtual void				RemoveModeChangeCallBack(ModeChangeCallbackFunc_t func) = 0;//107

	// Finds or create a procedural material.
	virtual IMaterial *			FindProceduralMaterial(const char *pMaterialName, const char *pTextureGroupName, void *pVMTKeyValues) = 0;//108

	virtual ImageFormat			GetNullTextureFormat() = 0;//109

	virtual void				AddTextureAlias(const char *pAlias, const char *pRealName) = 0;//110
	virtual void				RemoveTextureAlias(const char *pAlias) = 0;//111

	// returns a lightmap page ID for this allocation, -1 if none available
	// frameID is a number that should be changed every frame to prevent locking any textures that are
	// being used to draw in the previous frame
	virtual int					AllocateDynamicLightmap(int lightmapSize[2], int *pOutOffsetIntoPage, int frameID) = 0;//112

	virtual void				SetExcludedTextures(const char *pScriptName) = 0;//113
	virtual void				UpdateExcludedTextures(void) = 0;//114

	virtual bool				IsInFrame() const = 0;//115

	virtual void				CompactMemory() = 0;//116

	// For sv_pure mode. The filesystem figures out which files the client needs to reload to be "pure" ala the server's preferences.
	virtual void ReloadFilesInList(void *pFilesToReload) = 0;//117
	virtual	bool				AllowThreading(bool bAllow, int nServiceThread) = 0;//118

	// Extended version of FindMaterial().
	// Contains context in so it can make decisions (i.e. if it's a model, ignore certain cheat parameters)
	virtual IMaterial *			FindMaterialEx(char const* pMaterialName, const char *pTextureGroupName, int nContext, bool complain = true, const char *pComplainPrefix = NULL) = 0;//119

	virtual void				DoStartupShaderPreloading(void) = 0;//120

	virtual void				GMOD_FlushQueue() = 0;//121

	virtual bool				GMOD_TextureExists(const char * name) = 0;//122

	virtual IMaterial *			GetCurrentMaterial() = 0;//123

	virtual int					GetLightmapPage(void) = 0;//124

	virtual int					GetLightmapWidth(int lightmap) = 0;//125

	virtual int					GetLightmapHeight(int lightmap) = 0;//126

	virtual ITexture *			GetLocalCubemap(void) = 0;//127

	virtual void				ForceDepthFuncEquals(bool a1) = 0;//128

	virtual virtual MaterialHeightClipMode_t GetHeightClipMode() = 0;//129

	virtual void				AddMaterialToMaterialList(IMaterial* mat) = 0;//130

	virtual void				RemoveMaterial(IMaterial* mat) = 0;//131

	virtual void				RemoveMaterialSubRect(IMaterial* mat) = 0;//132

	virtual bool				InFlashlightMode() = 0;//133

	virtual bool				CanUseEditorMaterials() = 0;//134

	virtual const char *		GetForcedTextureLoadPathID() = 0;//135

	virtual void *				GetRenderCallQueue() = 0;//136

	virtual void				UnbindMaterial(IMaterial *pMaterial) = 0;//137

	virtual uint				GetRenderThreadId() = 0;//138

	virtual int					DetermineProxyReplacements(IMaterial * mat, void * kv) = 0;//139

	virtual MaterialSystem_Config_t &GetConfig() = 0;//140

	virtual void				NoteAnisotropicLevel(int currentLevel) = 0;//141

	virtual void									ReleaseShaderObjects() = 0;//142

	virtual void									RestoreShaderObjects(CreateInterfaceFn shaderFactory, int nChangeFlags = 0) = 0;//143

	virtual bool									InEditorMode() = 0;//144

	virtual bool									IsInStubMode() = 0;//145
	
	virtual ImageFormatInfo_t &				ImageFormatInfo(ImageFormat fmt) = 0;//146

	virtual int										GetMemRequired(int width, int height, int depth, ImageFormat format, bool mipmap) = 0;//147

	virtual bool									ConvertImageFormat(unsigned char *src, enum ImageFormat srcImageFormat,
		unsigned char *dst, enum ImageFormat dstImageFormat,
		int width, int height, int srcStride = 0, int dstStride = 0) = 0;//148

	virtual bool									OnDrawMesh(IMesh *pMesh, int firstIndex, int numIndices) = 0;//149

	virtual bool									OnDrawMesh(IMesh *pMesh, CPrimList *pLists, int nLists) = 0;//150

	virtual bool OnSetFlexMesh(IMesh *, IMesh *, int) = 0;//151
	virtual bool OnSetColorMesh(IMesh *, IMesh *, int) = 0;//152
	virtual bool OnSetPrimitiveType(IMesh *, MaterialPrimitiveType_t) = 0;//153

	virtual void SyncMatrices() = 0;//154

	virtual void SyncMatrix(MaterialMatrixMode_t) = 0;//155

	virtual bool OnFlushBufferedPrimitives() = 0;//156

	virtual int OnThreadEvent(unsigned int) = 0;//157

	virtual void BindStandardTexture(Sampler_t, StandardTextureId_t) = 0;//158

	virtual void BindStandardVertexTexture(VertexTextureSampler_t, StandardTextureId_t) = 0;//159

	virtual void GetLightmapDimensions(int *, int *) = 0;//160

	virtual void GetStandardTextureDimensions(int *, int *, StandardTextureId_t) = 0;//161

	virtual MorphFormat_t GetBoundMorphFormat() = 0;//162

	virtual ITexture *  GetRenderTargetEx(int) = 0;//163

	virtual void DrawClearBufferQuad(unsigned char, unsigned char, unsigned char, unsigned char, bool, bool) = 0;//164

	virtual int  MaxHWMorphBatchCount() = 0;//165

	virtual void GetCurrentColorCorrection(ShaderColorCorrectionInfo_t*) = 0;//166

};

/*
class CMatRenderContextPtr : public CRefPtr<IMatRenderContext>
{
	typedef CRefPtr<IMatRenderContext> BaseClass;
public:
	CMatRenderContextPtr() {}
	CMatRenderContextPtr(IMatRenderContext *pInit) : BaseClass(pInit) { if (BaseClass::m_pObject) BaseClass::m_pObject->BeginRender(); }
	CMatRenderContextPtr(IMaterialSystem *pFrom) : BaseClass(pFrom->GetRenderContext()) { if (BaseClass::m_pObject) BaseClass::m_pObject->BeginRender(); }
	~CMatRenderContextPtr() { if (BaseClass::m_pObject) BaseClass::m_pObject->EndRender(); }

	IMatRenderContext *operator=(IMatRenderContext *p) { if (p) p->BeginRender(); return BaseClass::operator=(p); }

	void SafeRelease() { if (BaseClass::m_pObject) BaseClass::m_pObject->EndRender(); BaseClass::SafeRelease(); }
	void AssignAddRef(IMatRenderContext *pFrom) { if (BaseClass::m_pObject) BaseClass::m_pObject->EndRender(); BaseClass::AssignAddRef(pFrom); BaseClass::m_pObject->BeginRender(); }

	void GetFrom(IMaterialSystem *pFrom) { AssignAddRef(pFrom->GetRenderContext()); }


private:
	CMatRenderContextPtr(const CMatRenderContextPtr &from);
	void operator=(const CMatRenderContextPtr &from);

};*/

struct ShaderStencilState_t
{
	bool m_bEnable;
	StencilOperation_t m_FailOp;
	StencilOperation_t m_ZFailOp;
	StencilOperation_t m_PassOp;
	StencilComparisonFunction_t m_CompareFunc;
	int m_nReferenceValue;
	uint32 m_nTestMask;
	uint32 m_nWriteMask;

	ShaderStencilState_t()
	{
		m_bEnable = false;
		m_PassOp = m_FailOp = m_ZFailOp = (StencilOperation_t)1;
		m_CompareFunc = (StencilComparisonFunction_t)8;
		m_nReferenceValue = 0;
		m_nTestMask = m_nWriteMask = 0xFFFFFFFF;
	}

	void SetStencilState(IMatRenderContext*pRenderContext)
	{
		pRenderContext->SetStencilEnable(m_bEnable);
		pRenderContext->SetStencilFailOperation(m_FailOp);
		pRenderContext->SetStencilZFailOperation(m_ZFailOp);
		pRenderContext->SetStencilPassOperation(m_PassOp);
		pRenderContext->SetStencilCompareFunction(m_CompareFunc);
		pRenderContext->SetStencilReferenceValue(m_nReferenceValue);
		pRenderContext->SetStencilTestMask(m_nTestMask);
		pRenderContext->SetStencilWriteMask(m_nWriteMask);
	}
};