#pragma once


class IViewRender
{
public:
	// SETUP
	// Initialize view renderer
	virtual void		Init(void) = 0;

	// Clear any systems between levels
	virtual void		LevelInit(void) = 0;
	virtual void		LevelShutdown(void) = 0;

	// Shutdown
	virtual void		Shutdown(void) = 0;

	// RENDERING
	// Called right before simulation. It must setup the view model origins and angles here so 
	// the correct attachment points can be used during simulation.	
	virtual void		OnRenderStart() = 0;

	// Called to render the entire scene
	virtual	void		Render(vrect_t *rect) = 0;

	// Called to render just a particular setup ( for timerefresh and envmap creation )
	virtual void		RenderView(const CViewSetup &view, int nClearFlags, int whatToDraw) = 0;

	// What are we currently rendering? Returns a combination of DF_ flags.
	virtual int GetDrawFlags() = 0;

	// MISC
	// Start and stop pitch drifting logic
	virtual void		StartPitchDrift(void) = 0;
	virtual void		StopPitchDrift(void) = 0;

	// This can only be called during rendering (while within RenderView).
	virtual VPlane*		GetFrustum() = 0;

	virtual bool		ShouldDrawBrushModels(void) = 0;

	virtual const CViewSetup *GetPlayerViewSetup(void) const = 0;
	virtual const CViewSetup *GetViewSetup(void) const = 0;

	virtual void		DisableVis(void) = 0;

	virtual int			BuildWorldListsNumber() const = 0;

	virtual void		SetCheapWaterStartDistance(float flCheapWaterStartDistance) = 0;
	virtual void		SetCheapWaterEndDistance(float flCheapWaterEndDistance) = 0;

	virtual void		GetWaterLODParams(float &flCheapWaterStartDistance, float &flCheapWaterEndDistance) = 0;

	virtual void		DriftPitch(void) = 0;

	virtual void		SetScreenOverlayMaterial(IMaterial *pMaterial) = 0;
	virtual IMaterial	*GetScreenOverlayMaterial() = 0;

	virtual void		WriteSaveGameScreenshot(const char *pFilename) = 0;
	virtual void		WriteSaveGameScreenshotOfSize(const char *pFilename, int width, int height) = 0;


	virtual void		WriteReplayScreenshot() = 0;
	virtual void		UpdateReplayScreenshotCache() = 0;

	// Draws another rendering over the top of the screen
	virtual void		QueueOverlayRenderView(const CViewSetup &view, int nClearFlags, int whatToDraw) = 0;

	// Returns znear and zfar
	virtual float		GetZNear() = 0;
	virtual float		GetZFar() = 0;

	virtual void		GetScreenFadeDistances(float *min, float *max) = 0;

	virtual IClientEntity *GetCurrentlyDrawingEntity() = 0;
	virtual void		SetCurrentlyDrawingEntity(IClientEntity *pEnt) = 0;

	virtual bool		UpdateShadowDepthTexture(void *pRenderTarget, void *pDepthTexture, const CViewSetup &shadowView) = 0;//ITexture

	virtual void		FreezeFrame(float flFreezeTime) = 0;
};

