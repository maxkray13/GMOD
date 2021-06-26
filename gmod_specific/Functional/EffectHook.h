#pragma once
namespace EffectHook {

	enum OverlayType_t
	{
		OVERLAY_BOX = 0,
		OVERLAY_SPHERE,
		OVERLAY_LINE,
		OVERLAY_TRIANGLE,
		OVERLAY_SWEPT_BOX,
		OVERLAY_BOX2
	};

	struct OverlayBase_t
	{
		OverlayBase_t()
		{
			m_Type = OVERLAY_BOX;
			m_nServerCount = -1;
			m_nCreationTick = -1;
			m_flEndTime = 0.0f;
			m_pNextOverlay = NULL;
		}
		OverlayType_t	m_Type;				// What type of overlay is it?
		int				m_nCreationTick;	// Duration -1 means go away after this frame #
		int				m_nServerCount;		// Latch server count, too
		float			m_flEndTime;		// When does this box go away
		OverlayBase_t	*m_pNextOverlay;
	};

	struct OverlayBox_t : public OverlayBase_t
	{
		OverlayBox_t() { m_Type = OVERLAY_BOX; }

		Vector			origin;
		Vector			mins;
		Vector			maxs;
		QAngle			angles;
		int				r;
		int				g;
		int				b;
		int				a;
	};

	struct OverlayBox2_t : public OverlayBase_t
	{
		OverlayBox2_t() { m_Type = OVERLAY_BOX2; }

		Vector			origin;
		Vector			mins;
		Vector			maxs;
		QAngle			angles;
		cColor			edgeColor;
		cColor			faceColor;
	};

	struct OverlaySphere_t : public OverlayBase_t
	{
		OverlaySphere_t() { m_Type = OVERLAY_SPHERE; }

		Vector			vOrigin;
		float			flRadius;
		int				nTheta;
		int				nPhi;
		int				r;
		int				g;
		int				b;
		int				a;
	};

	struct OverlayLine_t : public OverlayBase_t
	{
		OverlayLine_t() { m_Type = OVERLAY_LINE; }

		Vector			origin;
		Vector			dest;
		int				r;
		int				g;
		int				b;
		int				a;
		bool			noDepthTest;
	};

	struct OverlayTriangle_t : public OverlayBase_t
	{
		OverlayTriangle_t() { m_Type = OVERLAY_TRIANGLE; }

		Vector			p1;
		Vector			p2;
		Vector			p3;
		int				r;
		int				g;
		int				b;
		int				a;
		bool			noDepthTest;
	};

	struct OverlaySweptBox_t : public OverlayBase_t
	{
		OverlaySweptBox_t() { m_Type = OVERLAY_SWEPT_BOX; }

		Vector			start;
		Vector			end;
		Vector			mins;
		Vector			maxs;
		QAngle			angles;
		int				r;
		int				g;
		int				b;
		int				a;
	};

	extern std::vector<OverlayBase_t*> g_Impact_data;
	void DrawOverlay(OverlayBase_t *pOverlay);

	struct CEffectData {
		Vector			m_vOrigin;
		Vector			m_vStart;
		Vector			m_vNormal;
		QAngle			m_vAngles;
		int				m_fFlags;
		int				m_nEntIndex;
		float			m_flScale;
		float			m_flMagnitude;
		float			m_flRadius;
		int				m_nAttachmentIndex;
		short			m_nSurfaceProp;
		int				m_nMaterial;
		int				m_nDamageType;
		int				m_nHitBox;
		unsigned char	m_nColor;

	};

	typedef void(*ClientEffectCallback)(const CEffectData &data);

	class CClientEffectRegistration
	{
	public:
		const char *m_pEffectName;
		ClientEffectCallback m_pFunction;
		CClientEffectRegistration *m_pNext;
	};

	void PrintfAllNetworkedEffects();

	ClientEffectCallback Hook(ClientEffectCallback func, const char * effect_name);
	void INITIALIZE();
	void Impact(const CEffectData &data);


}