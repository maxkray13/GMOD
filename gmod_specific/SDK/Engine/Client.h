#pragma once

class CNetChan;

class IVEngineClient
{
public:
	// Find the model's surfaces that intersect the given sphere.
	// Returns the number of surfaces filled in.
	virtual int					GetIntersectingSurfaces(
		const void *model,
		const Vector &vCenter,
		const float radius,
		const bool bOnlyVisibleSurfaces,	// Only return surfaces visible to vCenter.
		void *pInfos,
		const int nMaxInfos) = 0;

	// Get the lighting intensivty for a specified point
	// If bClamp is specified, the resulting Vector is restricted to the 0.0 to 1.0 for each element
	virtual Vector				GetLightForPoint(const Vector &pos, bool bClamp) = 0;

	// Traces the line and reports the material impacted as well as the lighting information for the impact point
	virtual void			*TraceLineMaterialAndLighting(const Vector &start, const Vector &end,
		Vector &diffuseLightColor, Vector& baseColor) = 0;

	// Given an input text buffer data pointer, parses a single token into the variable token and returns the new
	//  reading position
	virtual const char			*ParseFile(const char *data, char *token, int maxlen) = 0;
	virtual bool				CopyLocalFile(const char *source, const char *destination) = 0;

	// Gets the dimensions of the game window
	virtual void				GetScreenSize(int& width, int& height) = 0;

	// Forwards szCmdString to the server, sent reliably if bReliable is set
	virtual void				ServerCmd(const char *szCmdString, bool bReliable = true) = 0;
	// Inserts szCmdString into the command buffer as if it was typed by the client to his/her console.
	// Note: Calls to this are checked against FCVAR_CLIENTCMD_CAN_EXECUTE (if that bit is not set, then this function can't change it).
	//       Call ClientCmd_Unrestricted to have access to FCVAR_CLIENTCMD_CAN_EXECUTE vars.
	virtual void				ClientCmd(const char *szCmdString) = 0;

	// Fill in the player info structure for the specified player index (name, model, etc.)
	virtual bool				GetPlayerInfo(int ent_num, player_info_t *pinfo) = 0;

	// Retrieve the player entity number for a specified userID
	virtual int					GetPlayerForUserID(int userID) = 0;

	// Retrieves text message system information for the specified message by name
	virtual client_textmessage_t *TextMessageGet(const char *pName) = 0;

	// Returns true if the console is visible
	virtual bool				Con_IsVisible(void) = 0;

	// Get the entity index of the local player
	virtual int					GetLocalPlayer(void) = 0;

	// Client DLL is hooking a model, loads the model into memory and returns  pointer to the model_t
	virtual const void		*LoadModel(const char *pName, bool bProp = false) = 0;

	// Get accurate, sub-frame clock ( profiling use )
	virtual float				Time(void) = 0;

	// Get the exact server timesstamp ( server time ) from the last message received from the server
	virtual float				GetLastTimeStamp(void) = 0;

	// Given a CAudioSource (opaque pointer), retrieve the underlying CSentence object ( stores the words, phonemes, and close
	//  captioning data )
	virtual void			*GetSentence(void *pAudioSource) = 0;
	// Given a CAudioSource, determines the length of the underlying audio file (.wav, .mp3, etc.)
	virtual float				GetSentenceLength(void *pAudioSource) = 0;
	// Returns true if the sound is streaming off of the hard disk (instead of being memory resident)
	virtual bool				IsStreaming(void *pAudioSource) const = 0;

	// Copy current view orientation into va
	virtual void				GetViewAngles(QAngle& va) = 0;
	// Set current view orientation from va
	virtual void				SetViewAngles(QAngle& va) = 0;

	// Retrieve the current game's maxclients setting
	virtual int					GetMaxClients(void) = 0;

	// Given the string pBinding which may be bound to a key, 
	//  returns the string name of the key to which this string is bound. Returns NULL if no such binding exists
	virtual	const char			*Key_LookupBinding(const char *pBinding) = 0;

	// Given the name of the key "mouse1", "e", "tab", etc., return the string it is bound to "+jump", "impulse 50", etc.
	virtual const char			*Key_BindingForKey(ButtonCode_t &code) = 0;

	// key trapping (for binding keys)
	virtual void				StartKeyTrapMode(void) = 0;
	virtual bool				CheckDoneKeyTrapping(ButtonCode_t &code) = 0;

	// Returns true if the player is fully connected and active in game (i.e, not still loading)
	virtual bool				IsInGame(void) = 0;
	// Returns true if the player is connected, but not necessarily active in game (could still be loading)
	virtual bool				IsConnected(void) = 0;
	// Returns true if the loading plaque should be drawn
	virtual bool				IsDrawingLoadingImage(void) = 0;

	// Prints the formatted string to the notification area of the screen ( down the right hand edge
	//  numbered lines starting at position 0
	virtual void				Con_NPrintf(int pos, const char *fmt, ...) = 0;
	// Similar to Con_NPrintf, but allows specifying custom text color and duration information
	virtual void				Con_NXPrintf(const struct con_nprint_s *info, const char *fmt, ...) = 0;

	// Is the specified world-space bounding box inside the view frustum?
	virtual int					IsBoxVisible(const Vector& mins, const Vector& maxs) = 0;

	// Is the specified world-space boudning box in the same PVS cluster as the view origin?
	virtual int					IsBoxInViewCluster(const Vector& mins, const Vector& maxs) = 0;

	// Returns true if the specified box is outside of the view frustum and should be culled
	virtual bool				CullBox(const Vector& mins, const Vector& maxs) = 0;

	// Allow the sound system to paint additional data (during lengthy rendering operations) to prevent stuttering sound.
	virtual void				Sound_ExtraUpdate(void) = 0;

	// Get the current game directory ( e.g., hl2, tf2, cstrike, hl1 )
	virtual const char			*GetGameDirectory(void) = 0;

	// Get access to the world to screen transformation matrix
	virtual const VMatrix& 		WorldToScreenMatrix() = 0;

	// Get the matrix to move a point from world space into view space
	// (translate and rotate so the camera is at the origin looking down X).
	virtual const matrix3x4_t& 		WorldToViewMatrix() = 0;

	// The .bsp file can have mod-specified data lumps. These APIs are for working with such game lumps.

	// Get mod-specified lump version id for the specified game data lump
	virtual int					GameLumpVersion(int lumpId) const = 0;
	// Get the raw size of the specified game data lump.
	virtual int					GameLumpSize(int lumpId) const = 0;
	// Loads a game lump off disk, writing the data into the buffer pointed to bye pBuffer
	// Returns false if the data can't be read or the destination buffer is too small
	virtual bool				LoadGameLump(int lumpId, void* pBuffer, int size) = 0;

	// Returns the number of leaves in the level
	virtual int					LevelLeafCount() const = 0;

	// Gets a way to perform spatial queries on the BSP tree
	virtual void*		GetBSPTreeQuery() = 0;

	// Convert texlight to gamma...
	virtual void		LinearToGamma(float* linear, float* gamma) = 0;

	// Get the lightstyle value
	virtual float		LightStyleValue(int style) = 0;

	// Computes light due to dynamic lighting at a point
	// If the normal isn't specified, then it'll return the maximum lighting
	virtual void		ComputeDynamicLighting(const Vector& pt, const Vector* pNormal, Vector& color) = 0;

	// Returns the color of the ambient light
	virtual void		GetAmbientLightColor(Vector& color) = 0;

	// Returns the dx support level
	virtual int			GetDXSupportLevel() = 0;

	// GR - returns the HDR support status
	virtual bool        SupportsHDR() = 0;

	// Replace the engine's material system pointer.
	virtual void		Mat_Stub(void *pMatSys) = 0;

	// Get the name of the current map
	virtual void GetChapterName(char *pchBuff, int iMaxLength) = 0;
	virtual char const	*GetLevelName(void) = 0;
	virtual int	GetLevelVersion(void) = 0;

	///

	// Obtain access to the voice tweaking API
	virtual struct IVoiceTweak_s *GetVoiceTweakAPI(void) = 0;

	// Tell engine stats gathering system that the rendering frame is beginning/ending
	virtual void		EngineStats_BeginFrame(void) = 0;
	virtual void		EngineStats_EndFrame(void) = 0;

	// This tells the engine to fire any events (temp entity messages) that it has queued up this frame. 
	// It should only be called once per frame.
	virtual void		FireEvents() = 0;

	// Returns an area index if all the leaves are in the same area. If they span multple areas, then it returns -1.
	virtual int			GetLeavesArea(int *pLeaves, int nLeaves) = 0;

	// Returns true if the box touches the specified area's frustum.
	virtual bool		DoesBoxTouchAreaFrustum(const Vector &mins, const Vector &maxs, int iArea) = 0;

	// Sets the hearing origin (i.e., the origin and orientation of the listener so that the sound system can spatialize 
	//  sound appropriately ).
	virtual void		SetAudioState(const void* state) = 0;

	// Sentences / sentence groups
	virtual int			SentenceGroupPick(int groupIndex, char *name, int nameBufLen) = 0;
	virtual int			SentenceGroupPickSequential(int groupIndex, char *name, int nameBufLen, int sentenceIndex, int reset) = 0;
	virtual int			SentenceIndexFromName(const char *pSentenceName) = 0;
	virtual const char *SentenceNameFromIndex(int sentenceIndex) = 0;
	virtual int			SentenceGroupIndexFromName(const char *pGroupName) = 0;
	virtual const char *SentenceGroupNameFromIndex(int groupIndex) = 0;
	virtual float		SentenceLength(int sentenceIndex) = 0;

	// Computes light due to dynamic lighting at a point
	// If the normal isn't specified, then it'll return the maximum lighting
	// If pBoxColors is specified (it's an array of 6), then it'll copy the light contribution at each box side.
	virtual void		ComputeLighting(const Vector& pt, const Vector* pNormal, bool bClamp, Vector& color, Vector *pBoxColors = NULL) = 0;

	// Activates/deactivates an occluder...
	virtual void		ActivateOccluder(int nOccluderIndex, bool bActive) = 0;
	virtual bool		IsOccluded(const Vector &vecAbsMins, const Vector &vecAbsMaxs) = 0;

	// The save restore system allocates memory from a shared memory pool, use this allocator to allocate/free saverestore 
	//  memory.
	virtual void		*SaveAllocMemory(size_t num, size_t size) = 0;
	virtual void		SaveFreeMemory(void *pSaveMem) = 0;

	// returns info interface for client netchannel
	virtual CNetChan	*GetNetChannelInfo(void) = 0;

	// Debugging functionality:
	// Very slow routine to draw a physics model
	virtual void		DebugDrawPhysCollide(void *pCollide, void *pMaterial, void* transform, void*color) = 0;
	// This can be used to notify test scripts that we're at a particular spot in the code.
	virtual void		CheckPoint(const char *pName) = 0;
	// Draw portals if r_DrawPortals is set (Debugging only)
	virtual void		DrawPortals() = 0;
	// Determine whether the client is playing back or recording a demo
	virtual bool		IsPlayingDemo(void) = 0;
	virtual bool		IsRecordingDemo(void) = 0;
	virtual bool		IsPlayingTimeDemo(void) = 0;
	virtual int			GetDemoRecordingTick(void) = 0;
	virtual int			GetDemoPlaybackTick(void) = 0;
	virtual int			GetDemoPlaybackStartTick(void) = 0;
	virtual float		GetDemoPlaybackTimeScale(void) = 0;
	virtual int			GetDemoPlaybackTotalTicks(void) = 0;

	// Is the game paused?
	virtual bool		IsPaused(void) = 0;
	// Is the game currently taking a screenshot?
	virtual bool		IsTakingScreenshot(void) = 0;
	// Is this a HLTV broadcast ?
	virtual bool		IsHLTV(void) = 0;
	// is this level loaded as just the background to the main menu? (active, but unplayable)
	virtual bool		IsLevelMainMenuBackground(void) = 0;
	// returns the name of the background level
	virtual void		GetMainMenuBackgroundName(char *dest, int destlen) = 0;
	// Get video modes
	virtual void		GetVideoModes(int &nCount, void*pModes) = 0;// vmode_s *&pModes

	// Occlusion system control
	virtual void		SetOcclusionParameters(const void*params) = 0;

	// What language is the user expecting to hear .wavs in, "english" or another...
	virtual void		GetUILanguage(char *dest, int destlen) = 0;

	// Can skybox be seen from a particular point?
	virtual void* IsSkyboxVisibleFromPoint(const Vector &vecPoint) = 0;


	// Get the pristine map entity lump string.  (e.g., used by CS to reload the map entities when restarting a round.)
	virtual const char*	GetMapEntitiesString() = 0;

	// Is the engine in map edit mode ?
	virtual bool		IsInEditMode(void) = 0;

	// current screen aspect ratio (eg. 4.0f/3.0f, 16.0f/9.0f)
	virtual float		GetScreenAspectRatio() = 0;

	// allow the game UI to login a user
	virtual bool		REMOVED_SteamRefreshLogin(const char *password, bool isSecure) = 0;
	virtual bool		REMOVED_SteamProcessCall(bool & finished) = 0;

	// allow other modules to know about engine versioning (one use is a proxy for network compatability)
	virtual unsigned int	GetEngineBuildNumber() = 0; // engines build
	virtual const char *	GetProductVersionString() = 0; // mods version number (steam.inf)

														   // Communicates to the color correction editor that it's time to grab the pre-color corrected frame
														   // Passes in the actual size of the viewport
	virtual void			GrabPreColorCorrectedFrame(int x, int y, int width, int height) = 0;

	virtual bool			IsHammerRunning() const = 0;

	// Inserts szCmdString into the command buffer as if it was typed by the client to his/her console.
	// And then executes the command string immediately (vs ClientCmd() which executes in the next frame)
	//
	// Note: this is NOT checked against the FCVAR_CLIENTCMD_CAN_EXECUTE vars.
	virtual void			ExecuteClientCmd(const char *szCmdString) = 0;

	// returns if the loaded map was processed with HDR info. This will be set regardless
	// of what HDR mode the player is in.
	virtual bool MapHasHDRLighting(void) = 0;

	virtual int	GetAppID() = 0;

	// Just get the leaf ambient light - no caching, no samples
	virtual Vector			GetLightForPointFast(const Vector &pos, bool bClamp) = 0;

	// This version does NOT check against FCVAR_CLIENTCMD_CAN_EXECUTE.
	virtual void			ClientCmd_Unrestricted(const char *szCmdString) = 0;

	// This used to be accessible through the cl_restrict_server_commands cvar.
	// By default, Valve games restrict the server to only being able to execute commands marked with FCVAR_SERVER_CAN_EXECUTE.
	// By default, mods are allowed to execute any server commands, and they can restrict the server's ability to execute client
	// commands with this function.
	virtual void			SetRestrictServerCommands(bool bRestrict) = 0;

	// If set to true (defaults to true for Valve games and false for others), then IVEngineClient::ClientCmd
	// can only execute things marked with FCVAR_CLIENTCMD_CAN_EXECUTE.
	virtual void			SetRestrictClientCommands(bool bRestrict) = 0;

	// Sets the client renderable for an overlay's material proxy to bind to
	virtual void			SetOverlayBindProxy(int iOverlayID, void *pBindProxy) = 0;

	virtual bool			CopyFrameBufferToMaterial(const char *pMaterialName) = 0;

	// Matchmaking
	virtual void			ChangeTeam(const char *pTeamName) = 0;

	// Causes the engine to read in the user's configuration on disk
	virtual void			ReadConfiguration(const bool readDefault = false) = 0;

	virtual void SetAchievementMgr(void *pAchievementMgr) = 0;
	virtual void *GetAchievementMgr() = 0;

	virtual bool			MapLoadFailed(void) = 0;
	virtual void			SetMapLoadFailed(bool bState) = 0;

	virtual bool			IsLowViolence() = 0;
	virtual const char		*GetMostRecentSaveGame(void) = 0;
	virtual void			SetMostRecentSaveGame(const char *lpszFilename) = 0;

	virtual void			StartXboxExitingProcess() = 0;
	virtual bool			IsSaveInProgress() = 0;
	virtual uint32_t		OnStorageDeviceAttached(void) = 0;
	virtual void			OnStorageDeviceDetached(void) = 0;

	virtual void			ResetDemoInterpolation(void) = 0;


	virtual void SetGamestatsData(void *pGamestatsData) = 0;
	virtual void *GetGamestatsData() = 0;


	// we need to pull delta's from the cocoa mgr, the engine vectors this for us
//	virtual void GetMouseDelta(int &x, int &y, bool bIgnoreNextMouseDelta = false) = 0;


	// Sends a key values server command, not allowed from scripts execution
	// Params:
	//	pKeyValues	- key values to be serialized and sent to server
	//				  the pointer is deleted inside the function: pKeyValues->deleteThis()
	virtual void ServerCmdKeyValues(PVOID pKeyValues) = 0;

	virtual DWORD IsSkippingPlayback(void) = 0;
	virtual	DWORD IsLoadingDemo(void) = 0;
	virtual	DWORD IsPlayingDemoALocallyRecordedDemo(void) = 0;
	virtual	DWORD Key_LookupBindingExact(char const*) = 0;
	virtual	DWORD GMOD_SetTimeManipulator(float) = 0;

	virtual	void GMOD_SendToServer(void* pData,unsigned int size, bool force) = 0;
		/*	CEngineClient::GMOD_PlaceDecalMaterial(IMaterial *, bool, int, IClientEntity *, Vector const&, Vector const&, color32_s const&, float, float)
			CEngineClient::GMOD_GetSpew(char *, unsigned long)
			CEngineClient::GMOD_SetViewEntity(unsigned int)
			CEngineClient::GMOD_BrushMaterialOverride(IMaterial *)
			CEngineClient::GMOD_R_RedownloadAllLightmaps(bool)
			CEngineClient::GMOD_RawClientCmd_Unrestricted(char const*)
			CEngineClient::GMOD_CreateDataTable(void(*)(void *, int, CGMODVariant const&))
			CEngineClient::GMOD_DestroyDataTable(IGMODDataTable *)
			CEngineClient::GMOD_LoadModel(char const*)
			CEngineClient::GetProtocolVersion(void)
			CEngineClient::IsWindowedMode(void)
			CEngineClient::FlashWindow(void)
			CEngineClient::GetClientVersion(void)const
			CEngineClient::IsActiveApp(void)
			CEngineClient::DisconnectInternal(void)
			CEngineClient::IsInCommentaryMode(void)*/


};

class IBaseClientDLL
{
public:
	// Called once when the client DLL is loaded
	virtual int				Init(void) = 0;//0

	virtual void			PostInit() = 0;//1

	// Called once when the client DLL is being unloaded
	virtual void			Shutdown(void) = 0;//2

	// Called once the client is initialized to setup client-side replay interface pointers
	virtual bool			ReplayInit(void) = 0;//3
	virtual bool			ReplayPostInit() = 0;//4

	// Called at the start of each level change
	virtual void			LevelInitPreEntity(char const* pMapName) = 0;//5
	// Called at the start of a new level, after the entities have been received and created
	virtual void			LevelInitPostEntity() = 0;//6
	// Called at the end of a level
	virtual void			LevelShutdown(void) = 0;//7

	// Request a pointer to the list of client datatable classes
	virtual ClientClass		*GetAllClasses(void) = 0;//8

	// Called once per level to re-initialize any hud element drawing stuff
	virtual int				HudVidInit(void) = 0;//9
	// Called by the engine when gathering user input
	virtual void			HudProcessInput(bool bActive) = 0;//10
	// Called oncer per frame to allow the hud elements to think
	virtual void			HudUpdate(bool bActive) = 0;//11
	// Reset the hud elements to their initial states
	virtual void			HudReset(void) = 0;
	// Display a hud text message
	virtual void			HudText(const char * message) = 0;

	// Mouse Input Interfaces
	// Activate the mouse (hides the cursor and locks it to the center of the screen)
	virtual void			IN_ActivateMouse(void) = 0;//14
	// Deactivates the mouse (shows the cursor and unlocks it)
	virtual void			IN_DeactivateMouse(void) = 0;
	// This is only called during extra sound updates and just accumulates mouse x, y offets and recenters the mouse.
	//  This call is used to try to prevent the mouse from appearing out of the side of a windowed version of the engine if 
	//  rendering or other processing is taking too long
	virtual void			IN_Accumulate(void) = 0;
	// Reset all key and mouse states to their initial, unpressed state
	virtual void			IN_ClearStates(void) = 0;
	// If key is found by name, returns whether it's being held down in isdown, otherwise function returns false
	virtual bool			IN_IsKeyDown(const char *name, bool& isdown) = 0;
	// Notify the client that the mouse was wheeled while in game - called prior to executing any bound commands.
	virtual void			IN_OnMouseWheeled(int nDelta) = 0;
	// Raw keyboard signal, if the client .dll returns 1, the engine processes the key as usual, otherwise,
	//  if the client .dll returns 0, the key is swallowed.
	virtual int				IN_KeyEvent(int eventcode, ButtonCode_t keynum, const char *pszCurrentBinding) = 0;

	// This function is called once per tick to create the player CUserCmd (used for prediction/physics simulation of the player)
	// Because the mouse can be sampled at greater than the tick interval, there is a separate input_sample_frametime, which
	//  specifies how much additional mouse / keyboard simulation to perform.
	virtual void			CreateMove(
		int sequence_number,			// sequence_number of this cmd
		float input_sample_frametime,	// Frametime for mouse input sampling
		bool active) = 0;				// True if the player is active (not paused)

										// If the game is running faster than the tick_interval framerate, then we do extra mouse sampling to avoid jittery input
										//  This code path is much like the normal move creation code, except no move is created
	virtual void			ExtraMouseSample(float frametime, bool active) = 0;

	// Encode the delta (changes) between the CUserCmd in slot from vs the one in slot to.  The game code will have
	//  matching logic to read the delta.
	virtual bool			WriteUsercmdDeltaToBuffer(void *buf, int from, int to, bool isnewcommand) = 0;//bf_write *buf
	// Demos need to be able to encode/decode CUserCmds to memory buffers, so these functions wrap that
	virtual void			EncodeUserCmdToBuffer(void * buf, int slot) = 0;//bf_write& buf
	virtual void			DecodeUserCmdFromBuffer(void * buf, int slot) = 0;//bf_read& 

	// Set up and render one or more views (e.g., rear view window, etc.).  This called into RenderView below
	virtual void			View_Render(vrect_t *rect) = 0;

	// Allow engine to expressly render a view (e.g., during timerefresh)
	// See IVRenderView.h, PushViewFlags_t for nFlags values
	virtual void			RenderView(const CViewSetup &view, int nClearFlags, int whatToDraw) = 0;//const CViewSetup &view

	// Apply screen fade directly from engine
	virtual void			View_Fade(ScreenFade_t *pSF) = 0;//ScreenFade_t *pSF

	// The engine has parsed a crosshair angle message, this function is called to dispatch the new crosshair angle
	virtual void			SetCrosshairAngle(const QAngle& angle) = 0;

	// Sprite (.spr) model handling code
	// Load a .spr file by name
	virtual void			InitSprite(void *pSprite, const char *loadname) = 0;//CEngineSprite *pSprite
	// Shutdown a .spr file
	virtual void			ShutdownSprite(void *pSprite) = 0;//CEngineSprite *pSprite
	// Returns sizeof( CEngineSprite ) so the engine can allocate appropriate memory
	virtual int				GetSpriteSize(void) const = 0;

	// Called when a player starts or stops talking.
	// entindex is -1 to represent the local client talking (before the data comes back from the server). 
	// entindex is -2 to represent the local client's voice being acked by the server.
	// entindex is GetPlayer() when the server acknowledges that the local client is talking.
	virtual void			VoiceStatus(int entindex, qboolean bTalking) = 0;

	// Networked string table definitions have arrived, allow client .dll to 
	//  hook string changes with a callback function ( see INetworkStringTableClient.h )
	virtual void			InstallStringTableCallback(char const *tableName) = 0;

	// Notification that we're moving into another stage during the frame.
	virtual void			FrameStageNotify(ClientFrameStage_t curStage) = 0;//35

	// The engine has received the specified user message, this code is used to dispatch the message handler
	virtual bool			DispatchUserMessage(int msg_type, void*msg_data) = 0;//bf_read &msg_data

	// Save/restore system hooks
	virtual void  *SaveInit(int size) = 0;
	virtual void			SaveWriteFields(void *, const char *, void *, void *, void *, int) = 0;
	virtual void			SaveReadFields(void *, const char *, void *, void *, void *, int) = 0;
	virtual void			PreSave(void *) = 0;
	virtual void			Save(void *) = 0;
	virtual void			WriteSaveHeaders(void *) = 0;
	virtual void			ReadRestoreHeaders(void *) = 0;
	virtual void			Restore(void *, bool) = 0;
	virtual void			DispatchOnRestore() = 0;

	// Hand over the StandardRecvProxies in the client DLL's module.
	virtual CStandardRecvProxies* GetStandardRecvProxies() = 0;

	// save game screenshot writing
	virtual void			WriteSaveGameScreenshot(const char *pFilename) = 0;

	// Given a list of "S(wavname) S(wavname2)" tokens, look up the localized text and emit
	//  the appropriate close caption if running with closecaption = 1
	virtual void			EmitSentenceCloseCaption(char const *tokenstream) = 0;
	// Emits a regular close caption by token name
	virtual void			EmitCloseCaption(char const *captionname, float duration) = 0;

	// Returns true if the client can start recording a demo now.  If the client returns false,
	// an error message of up to length bytes should be returned in errorMsg.
	virtual bool			CanRecordDemo(char *errorMsg, int length) const = 0;

	// Give the Client a chance to do setup/cleanup.
	virtual void			OnDemoRecordStart(char const* pDemoBaseName) = 0;
	virtual void			OnDemoRecordStop() = 0;
	virtual void			OnDemoPlaybackStart(char const* pDemoBaseName) = 0;
	virtual void			OnDemoPlaybackStop() = 0;

	// Draw the console overlay?
	virtual bool			ShouldDrawDropdownConsole() = 0;

	// Get client screen dimensions
	virtual int				GetScreenWidth() = 0;
	virtual int				GetScreenHeight() = 0;

	// Added interface

	// save game screenshot writing
	virtual void			WriteSaveGameScreenshotOfSize(const char *pFilename, int width, int height, bool bCreatePowerOf2Padded = false, bool bWriteVTF = false) = 0;

	// Gets the current view
	virtual bool			GetPlayerView(CViewSetup &playerView) = 0;

	// Matchmaking
	virtual void			SetupGameProperties(void *contexts, void *properties) = 0;//CUtlVector< XUSER_CONTEXT > &contexts, CUtlVector< XUSER_PROPERTY > &properties
	virtual uint32_t			GetPresenceID(const char *pIDName) = 0;
	virtual const char		*GetPropertyIdString(const uint32_t id) = 0;
	virtual void			GetPropertyDisplayString(uint32_t id, uint32_t value, char *pOutput, int nBytes) = 0;


	virtual void			InvalidateMdlCache() = 0;

	virtual void			IN_SetSampleTime(float frametime) = 0;


	// For sv_pure mode. The filesystem figures out which files the client needs to reload to be "pure" ala the server's preferences.
	virtual void			ReloadFilesInList(void *pFilesToReload) = 0;//IFileList *pFilesToReload

	// AR: Same as above win32 defn but down here at the end of the vtable for back compat
	virtual void			StartStatsReporting(void* handle, bool bArbitrated) = 0;


	// Let the client handle UI toggle - if this function returns false, the UI will toggle, otherwise it will not.
	virtual bool			HandleUiToggle() = 0;

	// Allow the console to be shown?
	virtual bool			ShouldAllowConsole() = 0;

	// Get renamed recv tables
	virtual void	*GetRenamedRecvTableInfos() = 0;

	// Get the mouthinfo for the sound being played inside UI panels
	virtual void		*GetClientUIMouthInfo() = 0;

	// Notify the client that a file has been received from the game server
	virtual void			FileReceived(const char * fileName, unsigned int transferID) = 0;

	virtual const char* TranslateEffectForVisionFilter(const char *pchEffectType, const char *pchEffectName) = 0;

	// Give the client a chance to modify sound settings however they want before the sound plays. This is used for
	// things like adjusting pitch of voice lines in Pyroland in TF2.
	virtual void			ClientAdjustStartSoundParams(struct StartSoundParams_t& params) = 0;

	// Returns true if the disconnect command has been handled by the client
	virtual bool DisconnectAttempt(void) = 0;

	//	virtual bool IsConnectedUserInfoChangeAllowed(IConVar *pCvar) = 0;
};

