#pragma once

class CInput
{
public:
	virtual		void		Init_All(void) = 0;//0
	virtual		void		Shutdown_All(void) = 0;//1
	virtual		int			GetButtonBits(int) = 0;//2
	virtual		void		CreateMove(int sequence_number, float input_sample_frametime, bool active) = 0;//3
	virtual		void		ExtraMouseSample(float frametime, bool active) = 0;
	virtual		bool		WriteUsercmdDeltaToBuffer(void *buf, int from, int to, bool isnewcommand) = 0;//bf_write *buf
	virtual		void		EncodeUserCmdToBuffer(bf_write* buf, int slot) = 0;//bf_write& buf
	virtual		void		DecodeUserCmdFromBuffer(bf_read* buf, int slot) = 0;//bf_read&

	virtual		CUserCmd	*GetUserCmd(int sequence_number) = 0;

	virtual		void		MakeWeaponSelection(void *weapon) = 0;//C_BaseCombatWeapon *weapon

	virtual		float		KeyState(kbutton_t *key) = 0;
	virtual		int			KeyEvent(int down, ButtonCode_t keynum, const char *pszCurrentBinding) = 0;
	virtual		kbutton_t	*FindKey(const char *name) = 0;

	virtual		void		ControllerCommands(void) = 0;
	virtual		void		Joystick_Advanced(void) = 0;
	virtual		void		Joystick_SetSampleTime(float frametime) = 0;
	virtual		void		IN_SetSampleTime(float frametime) = 0;

	virtual		void		AccumulateMouse(void) = 0;
	virtual		void		ActivateMouse(void) = 0;
	virtual		void		DeactivateMouse(void) = 0;

	virtual		void		ClearStates(void) = 0;
	virtual		float		GetLookSpring(void) = 0;

	virtual		void		GetFullscreenMousePos(int *mx, int *my, int *unclampedx = NULL, int *unclampedy = NULL) = 0;
	virtual		void		SetFullscreenMousePos(int mx, int my) = 0;
	virtual		void		ResetMouse(void) = 0;

	//	virtual		bool		IsNoClipping( void );
	virtual		float		GetLastForwardMove(void) = 0;
	virtual		float		Joystick_GetForward(void) = 0;
	virtual		float		Joystick_GetSide(void) = 0;
	virtual		float		Joystick_GetPitch(void) = 0;
	virtual		float		Joystick_GetYaw(void) = 0;
//	virtual		void		ClearInputButton(int bits);

	virtual		void		CAM_Think(void) = 0;
	virtual		int			CAM_IsThirdPerson(void) = 0;
	virtual		void		CAM_ToThirdPerson(void) = 0;
	virtual		void		CAM_ToFirstPerson(void) = 0;
	virtual		void		CAM_StartMouseMove(void) = 0;
	virtual		void		CAM_EndMouseMove(void) = 0;
	virtual		void		CAM_StartDistance(void) = 0;
	virtual		void		CAM_EndDistance(void) = 0;
	virtual		int			CAM_InterceptingMouse(void) = 0;

	// orthographic camera info
	virtual		void		CAM_ToOrthographic() = 0;
	virtual		bool		CAM_IsOrthographic() const = 0;
	virtual		void		CAM_OrthographicSize(float& w, float& h) const = 0;




	// IK back channel info
	virtual		void		AddIKGroundContactInfo(int entindex, float minheight, float maxheight) = 0;

	virtual		void		LevelInit(void) = 0;

	virtual		void		CAM_SetCameraThirdData(CameraThirdData_t *pCameraData, const QAngle &vecCameraOffset) = 0;
	virtual		void		CAM_CameraThirdThink(void) = 0;

	virtual	bool		EnableJoystickMode() = 0;

	CUserCmd* FORCE_GetUserCmd(int sequence_number)
	{
		if (m_pCommands == NULL) return NULL;
		//return (CUserCmd*) (*(unsigned long*)this->m_pCommands) + (0x160 * (sequence_number % 90));
		return &m_pCommands[(sequence_number % 90)];
	}
	CVerifiedUserCmd* FORCE_GetVerifiedUserCmd(int sequence_number)
	{
		if (m_pVerifiedCommands == NULL) return NULL;

		return &m_pVerifiedCommands[sequence_number % 90];
	}


public:
	char _0x0000[196];
	CUserCmd         *m_pCommands;        //0x00C4
	CVerifiedUserCmd *m_pVerifiedCommands;//0x00C8


//	__int32 CMD; //0x00C4 
//	__int32 Verifed; //0x00C8 

//	virtual void funct() = 0;
//	virtual		float		CAM_CapYaw(float fVal) { return fVal; }

};

class C_CommandContext
{
public:
	bool			needsprocessing;

	CUserCmd		cmd;
	int				command_number;
};