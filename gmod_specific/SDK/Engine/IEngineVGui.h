#pragma once
class CEngineVGui
{
public:
	virtual ~CEngineVGui() = 0;

	// Methods of IEngineVGui
	virtual VPANEL GetPanel(VGuiPanel_t type) = 0;
	virtual bool IsGameUIVisible() = 0;


	// Methods of IEngineVGuiInternal
	virtual void Init() = 0;
	virtual void Connect() = 0;
	virtual void Shutdown() = 0;
	virtual bool SetVGUIDirectories() = 0;
	virtual bool IsInitialized() = 0;
	virtual CreateInterfaceFn GetGameUIFactory() = 0;
	virtual bool Key_Event(const InputEvent_t &event) = 0;
	virtual void BackwardCompatibility_Paint() = 0;
	virtual void UpdateButtonState(const InputEvent_t &event) = 0;
	virtual void PostInit() = 0;

	virtual void Paint(PaintMode_t mode) = 0;
	

	
	

	// handlers for game UI (main menu)
	virtual void ActivateGameUI() = 0;
	virtual bool HideGameUI() = 0;
	// console
	virtual void ShowConsole() = 0;
	virtual void HideConsole() = 0;
	virtual bool IsConsoleVisible() = 0;
	virtual void ClearConsole() = 0;
	virtual void HideDebugSystem() = 0;


	

	// level loading
	virtual void OnLevelLoadingStarted() = 0;
	virtual void OnLevelLoadingFinished() = 0;
	virtual void NotifyOfServerConnect(const char *game, int IP, int connectionPort, int queryPort) = 0;
	virtual void NotifyOfServerDisconnect() = 0;
	virtual void EnabledProgressBarForNextLoad() = 0;

	virtual void UpdateProgressBar(LevelLoadingProgress_e progress) = 0;
	virtual void UpdateCustomProgressBar(float progress, const wchar_t *desc) = 0;
	virtual void StartCustomProgress() = 0;
	virtual void FinishCustomProgress() = 0;
	virtual void ShowErrorMessage() = 0;
	// Should pause?
	virtual bool ShouldPause() = 0;

	virtual void SetGameDLLPanelsVisible(bool show) = 0;


	virtual void ShowNewGameDialog(int chapter = -1) = 0; // -1 means just keep the currently select chapter
	virtual void Simulate() = 0;

	virtual void SetNotAllowedToHideGameUI(bool bNotAllowedToHide) = 0;
	virtual void SetNotAllowedToShowGameUI(bool bNotAllowedToShow) = 0;


	virtual void SessionNotification(const int notification, const int param = 0) = 0;;
	virtual void SystemNotification(const int notification) = 0;;
	virtual void ShowMessageDialog(const uint nType, void *pOwner = NULL) = 0;;
	virtual void UpdatePlayerInfo(uint64 nPlayerId, const char *pName, int nTeam, byte cVoiceState, int nPlayersNeeded, bool bHost) = 0;;
	virtual void SessionSearchResult(int searchIdx, void *pHostData, void *pResult, int ping) = 0;;
	virtual void OnCreditsFinished(void) = 0;;
};