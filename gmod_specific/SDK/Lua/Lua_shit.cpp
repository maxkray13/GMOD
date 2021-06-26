#include "Lua_shit.h"
#include "../../SDK.h"
//#include "../../../engine/Render/imGui1.52/imgui.h"
#include "../../../engine/Render/Color.h"
#include "../../Hooks/Hooks.h"
namespace Lua {

	int Lua::GetTeam(int EntIdx) {
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		int Team = -1337;
		if (g_pLuaShitInterface) {
			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "FindMetaTable");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return Team;
			}
			g_pLuaShitInterface->PushString("Player");
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return Team;
			}

			g_pLuaShitInterface->GetField(-1, "Team");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return Team;
			}
			g_pLuaShitInterface->GetField(-3, "Entity");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(4);
				return Team;
			}

			g_pLuaShitInterface->PushNumber(EntIdx);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(4);
				return Team;
			}

			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return Team;
			}
			Team = g_pLuaShitInterface->GetFlags(-1);
			g_pLuaShitInterface->Pop(3);
		}
		return Team;


	}

	cColor Lua::GetTeamColor(int TeamIdx) {

		cColor ret(0, 0, 0, 255);
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);

		if (!g_pLuaShitInterface) return ret;

		g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
		g_pLuaShitInterface->GetField(-1, "team");

		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(2);
			return ret;
		}
		g_pLuaShitInterface->GetField(-1, "GetColor");

		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(3);
			return ret;
		}
		g_pLuaShitInterface->PushNumber(TeamIdx);
		g_pLuaShitInterface->Call(1, 1);
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(3);
			return ret;
		}
		ILuaObject* colorObject = g_pLuaShitInterface->GetObjectC(-1);
		if (colorObject) {
			ret.SetColor(
				(int)colorObject->GetMemberDouble("r", 0.0),
				(int)colorObject->GetMemberDouble("g", 0.0),
				(int)colorObject->GetMemberDouble("b", 0.0),
				(int)colorObject->GetMemberDouble("a", 255.0)
			);
			g_pLuaShitInterface->Pop(3);
			return ret;
		}
		g_pLuaShitInterface->Pop(3);
		return ret;

	}

	const char * Lua::GetName(int EntIdx) {
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		const char* name = nullptr;
		if (g_pLuaShitInterface) {
			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "FindMetaTable");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return name;
			}
			g_pLuaShitInterface->PushString("Player");
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return name;
			}
			g_pLuaShitInterface->GetField(-1, "Nick");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return name;
			}
			g_pLuaShitInterface->GetField(-3, "Entity");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(4);
				return name;
			}
			g_pLuaShitInterface->PushNumber(EntIdx);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(4);
				return name;
			}
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return name;
			}
			name = g_pLuaShitInterface->GetString(-1);
			g_pLuaShitInterface->Pop(3);
		}
		return name;

	}

	const char * Lua::GetTeamName(int TeamIdx) {
		const char * ret = nullptr;
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);

		if (!g_pLuaShitInterface) return ret;

		g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
		g_pLuaShitInterface->GetField(-1, "team");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(2);
			return ret;
		}
		g_pLuaShitInterface->GetField(-1, "GetName");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(3);
			return ret;
		}
		g_pLuaShitInterface->PushNumber(TeamIdx);
		g_pLuaShitInterface->Call(1, 1);
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(3);
			return ret;
		}

		auto str = g_pLuaShitInterface->GetString();

		if (str) {
			ret = str;
			g_pLuaShitInterface->Pop(3);
			return ret;
		}
		g_pLuaShitInterface->Pop(3);
		return ret;

	}

	Lua::Lua_Role Lua::GetRole(int EntIdx) {
		Lua_Role RetRole = Lua_Role::invalid;
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {

			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "Entity");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return RetRole;
			}
			g_pLuaShitInterface->PushNumber(EntIdx);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return RetRole;
			}
			g_pLuaShitInterface->GetField(-1, "IsTraitor");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return RetRole;
			}
			g_pLuaShitInterface->Push(-2);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return RetRole;
			}
			auto is_triator = g_pLuaShitInterface->GetBool(-1);
			if (is_triator) {
				RetRole = Lua_Role::TRAITOR;
				g_pLuaShitInterface->Pop();
				g_pLuaShitInterface->Pop(2);
				return RetRole;
			}
			g_pLuaShitInterface->Pop();
			g_pLuaShitInterface->GetField(-1, "IsDetective");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return RetRole;
			}
			g_pLuaShitInterface->Push(-2);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return RetRole;
			}
			auto is_detective = g_pLuaShitInterface->GetBool(-1);
			if (is_detective) {
				RetRole = Lua_Role::DETECTIVE;
				g_pLuaShitInterface->Pop();
				g_pLuaShitInterface->Pop(2);
				return RetRole;
			}
			RetRole = Lua_Role::INNOCENT;
			g_pLuaShitInterface->Pop();
			g_pLuaShitInterface->Pop(2);
		}
		return RetRole;

	}
	/*
	[CODE]
			bool ret = false;
			g_pLuaInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaInterface->GetField(-1, "Entity");
			if (g_pLuaInterface->GetType(-1) == Type::NIL) {
				g_pLuaInterface->Pop(2);
				return ret;
			}
			g_pLuaInterface->PushNumber(EntIdx);
			g_pLuaInterface->Call(1, 1);
			if (g_pLuaInterface->GetType(-1) == Type::NIL) {
				g_pLuaInterface->Pop(2);
				return ret;
			}
			g_pLuaInterface->GetField(-1, "IsAdmin");
			if (g_pLuaInterface->GetType(-1) == Type::NIL) {
				g_pLuaInterface->Pop(3);
				return ret;
			}
			g_pLuaInterface->Push(-2);
			g_pLuaInterface->Call(1, 1);

			if (g_pLuaInterface->GetType(-1) != Type::BOOL) {
				g_pLuaInterface->Pop(3);
				return ret;
			}

			ret = g_pLuaInterface->GetBool();

			g_pLuaInterface->Pop(3);

			return ret;
[/CODE]
	*/
	bool Lua::IsKeypad(int EntIdx) {

		bool ret = false;

		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "Entity");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->PushNumber(EntIdx);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->GetField(-1, "IsKeypad");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return ret;
			}

			ret = g_pLuaShitInterface->GetBool(-1);
			g_pLuaShitInterface->Pop(3);
		}
		return ret;
	}

	const char * Lua::GetKeypadStaticCode(int EntIdx) {
		const char * ret = nullptr;
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "Entity");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->PushNumber(EntIdx);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->GetField(-1, "GetText");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return ret;
			}
			g_pLuaShitInterface->Push(-2);
			g_pLuaShitInterface->Call(1, 1);


			ret = g_pLuaShitInterface->GetString();

			g_pLuaShitInterface->Pop(3);
		}
		return ret;
	}

	const char * Lua::GetKeypadTempCode(int EntIdx) {
		const char * ret = nullptr;
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "Entity");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->PushNumber(EntIdx);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->GetField(-1, "GetText");//GetDisplayText
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(1);
				g_pLuaShitInterface->GetField(-1, "GetDisplayText");
				if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {

					g_pLuaShitInterface->Pop(3);
					return ret;
				}

			}
			g_pLuaShitInterface->Push(-2);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return ret;
			}

			ret = g_pLuaShitInterface->GetString();
			g_pLuaShitInterface->Pop(3);
		}
		return ret;

	}

	bool Lua::GetSecureKeypad(int EntIdx) {

		bool ret = true;
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "Entity");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->PushNumber(EntIdx);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->GetField(-1, "GetSecure");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return ret;
			}
			g_pLuaShitInterface->Push(-2);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return ret;
			}
			ret = g_pLuaShitInterface->GetBool(-1);


			g_pLuaShitInterface->Pop(3);
		}
		return ret;
	}

	Lua::KeypadStatus Lua::GetStatusKeypad(int EntIdx) {
		KeypadStatus ret = KeypadStatus::Status_None;
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "Entity");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->PushNumber(EntIdx);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->GetField(-1, "GetStatus");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return ret;
			}
			g_pLuaShitInterface->Push(-2);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return ret;
			}
			ret = (KeypadStatus)g_pLuaShitInterface->GetFlags(-1);

			g_pLuaShitInterface->Pop(3);
		}
		return ret;

	}
	Vector CalcRPos(IClientEntity* ent, int idx) {
		Vector ret = {};
		auto pos = *ent->GetRenderPos();
		auto max = ent->OBBMaxs();

		Vector forward = {};


		Vector right = {};

		Vector up = {};

		ent->GetVectors(&forward, &right, &up);

		pos += forward * max.m_x;
		pos += right * max.m_y;
		pos += up * max.m_z;

		pos += forward * 0.15;


		return pos;

		/*
		local pos = self:GetPos()
		pos:Add(self:GetForward() * self.Maxs.x) -- Translate to front
		pos:Add(self:GetRight() * self.Maxs.y) -- Translate to left
		pos:Add(self:GetUp() * self.Maxs.z) -- Translate to top

		pos:Add(self:GetForward() * 0.15) -- Pop out of front to stop culling

	return pos
		*/
	}



	void RotateAroundAxis(QAngle& to_rotate, Vector axis, float rotation) {

		matrix3x4_t rot = {};
		matrix3x4_t qmatrix = {};
		matrix3x4_t contactmatrix = {};
		Vector axis_out = {};
		Quaternion q = {};
		QAngle New_ang = {};
		AngleMatrix(to_rotate, rot);
		VectorIRotate((float*)&axis, rot, (float*)&axis_out);
		AxisAngleQuaternion(axis_out, rotation, q);
		QuaternionMatrix(q, Vector(0.f, 0.f, 0.f), qmatrix);
		ConcatTransforms(rot, qmatrix, contactmatrix);
		MatrixAngles(contactmatrix, (float*)&New_ang);


		to_rotate = New_ang;
	}
	QAngle CalcRAng(IClientEntity* ent) {
		auto angle = *ent->GetRenderAngle();
		Vector forward = {};
		Vector right = {};
		Vector up = {};
		AngleVectors(angle, &forward, &right, &up);
		//angle.ToVector(&forward, &right, &up);
		//ent->GetVectors(&forward, &right, &up);
		RotateAroundAxis(angle, right, -90.f);
		AngleVectors(angle, &forward, &right, &up);
		RotateAroundAxis(angle, up, 90.f);

		return angle;
		/*
		local ang = self:GetAngles()
		ang:RotateAroundAxis(ang:Right(), -90)
		ang:RotateAroundAxis(ang:Up(), 90)

	return ang

		*/
	}





	Vector * Lua::CalculateRenderPos(int EntIdx) {
		Vector* ret = nullptr;
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "Entity");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->PushNumber(EntIdx);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->GetField(-1, "CalculateRenderPos");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return ret;
			}
			g_pLuaShitInterface->Push(-2);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return ret;
			}
			ret = g_pLuaShitInterface->GetVector(-1);

			g_pLuaShitInterface->Pop(3);
		}
		return ret;

	}

	QAngle * Lua::CalculateRenderAng(int EntIdx) {
		QAngle* ret = nullptr;
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "Entity");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->PushNumber(EntIdx);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->GetField(-1, "CalculateRenderAng");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return ret;
			}
			g_pLuaShitInterface->Push(-2);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return ret;
			}
			ret = g_pLuaShitInterface->GetAngle(-1);

			g_pLuaShitInterface->Pop(3);
		}
		return ret;

	}

	double Lua::GetScale(int EntIdx) {
		auto ret = -1337.0;
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "Entity");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->PushNumber(EntIdx);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->GetField(-1, "Scale");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return ret;
			}
			ret = g_pLuaShitInterface->GetNumber(-1);

			g_pLuaShitInterface->Pop(3);
		}
		return ret;

	}

	Vector * Lua::GetAimVector(int EntIdx) {
		Vector* ret = nullptr;
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "Entity");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->PushNumber(EntIdx);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->GetField(-1, "GetAimVector");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return ret;
			}
			g_pLuaShitInterface->Push(-2);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return ret;
			}
			ret = g_pLuaShitInterface->GetVector(-1);
			g_pLuaShitInterface->Pop(3);
		}
		return ret;
	}

	Vector * Lua::GetForward(int EntIdx) {
		Vector* ret = nullptr;
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "Entity");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->PushNumber(EntIdx);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->GetField(-1, "GetForward");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return ret;
			}
			g_pLuaShitInterface->Push(-2);
			g_pLuaShitInterface->Call(1, 1);
			ret = g_pLuaShitInterface->GetVector(-1);


			g_pLuaShitInterface->Pop(3);
		}
		return ret;

	}
	Vector * Lua::GetRight(int EntIdx) {
		Vector* ret = nullptr;
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "Entity");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->PushNumber(EntIdx);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->GetField(-1, "GetRight");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return ret;
			}
			g_pLuaShitInterface->Push(-2);
			g_pLuaShitInterface->Call(1, 1);
			ret = g_pLuaShitInterface->GetVector(-1);


			g_pLuaShitInterface->Pop(3);
		}
		return ret;

	}
	Vector * Lua::GetUp(int EntIdx) {
		Vector* ret = nullptr;
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "Entity");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->PushNumber(EntIdx);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->GetField(-1, "GetUp");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return ret;
			}
			g_pLuaShitInterface->Push(-2);
			g_pLuaShitInterface->Call(1, 1);
			ret = g_pLuaShitInterface->GetVector(-1);


			g_pLuaShitInterface->Pop(3);
		}
		return ret;

	}
	double Lua::GetWidth2D(int EntIdx) {
		auto ret = -1337.0;
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "Entity");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->PushNumber(EntIdx);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->GetField(-1, "Width2D");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return ret;
			}
			ret = g_pLuaShitInterface->GetNumber(-1);

			g_pLuaShitInterface->Pop(3);
		}
		return ret;

	}

	double Lua::GetHeight2D(int EntIdx) {
		auto ret = -1337.0;
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "Entity");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->PushNumber(EntIdx);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->GetField(-1, "Height2D");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return ret;
			}
			ret = g_pLuaShitInterface->GetNumber(-1);

			g_pLuaShitInterface->Pop(3);
		}
		return ret;
	}

	Vector * Lua::GetEyePos(int EntIdx) {
		Vector* ret = nullptr;
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "Entity");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->PushNumber(EntIdx);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->GetField(-1, "EyePos");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return ret;
			}
			g_pLuaShitInterface->Push(-2);
			g_pLuaShitInterface->Call(1, 1);
			ret = g_pLuaShitInterface->GetVector(-1);


			g_pLuaShitInterface->Pop(3);
		}
		return ret;
	}

	Vector * Lua::IntersectRayWithPlane(Vector rayOrigin, Vector rayDirection, Vector planePosition, Vector planeNormal) {
		Vector* Ret = nullptr;
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (!g_pLuaShitInterface)
			return Ret;

		g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
		g_pLuaShitInterface->GetField(-1, "util");

		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(2);
			return Ret;
		}
		g_pLuaShitInterface->GetField(-1, "IntersectRayWithPlane");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(3);
			return Ret;
		}
		/*	g_pLuaShitInterface->PushVector(planeNormal);
		g_pLuaShitInterface->PushVector(planePosition);
		g_pLuaShitInterface->PushVector(rayDirection);
		g_pLuaShitInterface->PushVector(rayOrigin);*/
		g_pLuaShitInterface->PushVector(rayOrigin);
		g_pLuaShitInterface->PushVector(rayDirection);
		g_pLuaShitInterface->PushVector(planePosition);
		g_pLuaShitInterface->PushVector(planeNormal);
		g_pLuaShitInterface->Call(4, 1);

		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(3);
			return Ret;
		}
		Ret = g_pLuaShitInterface->GetVector(-1);
		g_pLuaShitInterface->Pop(3);
		return Ret;
	}

	Vector2D CalculateKeypadCursorPos(IClientEntity * Enemy, IClientEntity * Keypad)
	{
		Vector2D Ret;
		Ret.Init();
		Vector EyePos;
		Vector aimvec;
		Vector pos;
		QAngle ang;
		double scale;
		Vector aForward, aRight, aUp;
		Vector KeypadForward;
		Vector intersection;
		if (!Enemy || !Keypad)
			return Ret;


		if (GetAimVector(Enemy->entindex()))
			aimvec = *GetAimVector(Enemy->entindex());
		else
			return Ret;


		if (!GetEyePos(Enemy->entindex()))
			return Ret;
		EyePos = *GetEyePos(Enemy->entindex());

		//Vector start = EyePos;
		//Vector end = EyePos + aimvec * 65.f;
		//trace_t Trace;
		//SDK::Function::UTIL_TraceLine(start, end, MASK_SOLID, Enemy, 0, &Trace);

		//if (Trace.m_pEnt != Keypad)
		//	return Ret;
		scale = GetScale(Keypad->entindex());
		if (!scale || scale == -1337.0) {
			scale = 0.02;
			/*	//some_shit
				Vector start = EyePos;
				Vector end = EyePos + aimvec * 65.f;
				trace_t Trace;
				SDK::Function::UTIL_TraceLine(start, end, MASK_SOLID, Enemy, 0, &Trace);
				Vector Screens = {};
				;
				Vector out = {};
				auto temp_endtrace = EyePos + (aimvec * 65.f)*Trace.fraction;
				VectorITransform((float*)&temp_endtrace, ((C_BaseEntity*)Keypad)->m_rgflCoordinateFrame(), (float*)&out);


			//	SDK::W2S(EyePos + (aimvec * 65.f)*Trace.fraction, Screens);*/
		}

		/*
		if (!CalculateRenderPos(Keypad->entindex()))
			return Ret;
		auto luapos = *CalculateRenderPos(Keypad->entindex());

		if (!CalculateRenderAng(Keypad->entindex()))
			return Ret;
		auto luaang = *CalculateRenderAng(Keypad->entindex());

		*/
		pos = CalcRPos((C_GMOD_Player*)Keypad, Keypad->entindex());
		ang = CalcRAng((C_GMOD_Player*)Keypad);


		if (pos.IsZero() || !scale)
			return Ret;

		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			g_pLuaShitInterface->PushAngle(ang);
			g_pLuaShitInterface->GetField(-1, "Forward");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return Ret;
			}
			g_pLuaShitInterface->Push(-2);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return Ret;
			}
			aForward = *g_pLuaShitInterface->GetVector(-1);
			g_pLuaShitInterface->Pop(2);
		}

		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			g_pLuaShitInterface->PushAngle(ang);
			g_pLuaShitInterface->GetField(-1, "Right");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return Ret;
			}
			g_pLuaShitInterface->Push(-2);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return Ret;
			}
			aRight = *g_pLuaShitInterface->GetVector(-1);
			g_pLuaShitInterface->Pop(2);
		}

		if (!GetForward(Keypad->entindex()))
			return Ret;
		KeypadForward = *GetForward(Keypad->entindex());

		if (!IntersectRayWithPlane(EyePos, aimvec, pos, KeypadForward))
			return Ret;
		intersection = *IntersectRayWithPlane(EyePos, aimvec, pos, KeypadForward);

		if (intersection.IsZero())
			return Ret;
		Vector diff = pos - intersection;

		Ret.m_x = diff.Dot(-aForward) / scale;
		Ret.m_y = diff.Dot(-aRight) / scale;
		return Ret;

	}
	struct KeyPadElemets {

		float x, y, w, h;
		const char * text = nullptr;
	};
	char* numbers[] = {
		"0","1","2","3","4","5","6","7","8","9",
	};



	KeyPadElemets GetKeypadElement(int number) {
		KeyPadElemets ret = {};
		int column = (number - 1) % 3;
		int row = floor((number - 1) / 3);
		/*ret.x = 0.075f + (0.3f * column);
		ret.y = 0.175f + 0.25f + 0.05f + ((0.5f / 3.f) * row);
		ret.w = 0.25f;
		ret.h = 0.13f;*/

		ret.x = 0.075f + (0.3f * column);
		ret.y = 0.175f + 0.25f + 0.05f + ((0.5f / 3.f) * row);
		ret.w = 0.28f;
		ret.h = 0.16f;


		ret.text = numbers[number];
		return ret;
	}


	int GetHoveredElement(IClientEntity * Enemy, IClientEntity * Keypad)
	{

		int ret = -1;
		float w = GetWidth2D(Keypad->entindex());
		float h = GetHeight2D(Keypad->entindex());
		auto Vec = CalculateKeypadCursorPos(Enemy, Keypad);
		float x = Vec.m_x;
		float y = Vec.m_y;

		//self.Width2D, self.Height2D = (self.Maxs.y - self.Mins.y) / self.Scale , (self.Maxs.z - self.Mins.z) / self.Scale
		if (w == -1337.f) {
			w = (Keypad->OBBMaxs().m_y - Keypad->OBBMins().m_y) / 0.02;
		}
		if (h == -1337.f) {
			h = (Keypad->OBBMaxs().m_z - Keypad->OBBMins().m_z) / 0.02;
		}

		for (int iter = 0; iter <= 9; iter++) {
			auto Key = GetKeypadElement(iter);

			Key.x *= w;
			Key.y *= h;
			Key.w *= w;
			Key.h *= h;

			if (Key.x < x && Key.x + Key.w > x &&
				Key.y < y && Key.y + Key.h > y) {
				ret = iter;
				break;
			}
		}
		return ret;
	}

	bool weapon_is_m9k(IClientEntity * weapon)
	{
		if (!weapon || !weapon->IsValid())
			return false;
		if (!weapon->GetScriptName())
			return false;
		std::string sc_n = weapon->GetScriptName();
		if (sc_n.find("m9k") != -1)
			return true;

		return false;
	}

	enum {
		FAS_STAT_IDLE = 0,
		FAS_STAT_ADS = 1,
		FAS_STAT_SPRINT = 2,
		FAS_STAT_HOLSTER = 3,
		FAS_STAT_CUSTOMIZE = 4,
		FAS_STAT_HOLSTER_START = 5,
		FAS_STAT_HOLSTER_END = 6,
		FAS_STAT_QUICKGRENADE = 7
	};
	const char * GetWeaponBase(int EntIdx)
	{
		const char * ret = nullptr;
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "Entity");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->PushNumber(EntIdx);
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return ret;
			}
			g_pLuaShitInterface->GetField(-1, "Base");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return ret;
			}
			ret = g_pLuaShitInterface->GetString(-1);

			g_pLuaShitInterface->Pop(3);
		}
		return ret;


		return nullptr;
	}
	QAngle fas2_nospread(C_GMOD_Player * ply, C_BaseCombatWeapon * weap, CUserCmd* pCmd)
	{
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (!g_pLuaShitInterface)
			return QAngle(0.f, 0.f, 0.f);

		Vector vForward, vRight, vUp;
		float CurrCone = 0.f;
		g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
		g_pLuaShitInterface->GetField(-1, "FindMetaTable");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(2);
			return QAngle(0.f, 0.f, 0.f);
		}
		g_pLuaShitInterface->PushString("Player");
		g_pLuaShitInterface->Call(1, 1);
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(2);
			return QAngle(0.f, 0.f, 0.f);
		}
		g_pLuaShitInterface->GetField(-1, "GetActiveWeapon");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(3);
			return QAngle(0.f, 0.f, 0.f);
		}
		g_pLuaShitInterface->GetField(-3, "Entity");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}
		g_pLuaShitInterface->PushNumber(g_pEngine->GetLocalPlayer());
		g_pLuaShitInterface->Call(1, 1);
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}
		g_pLuaShitInterface->Call(1, 1);
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(3);
			return QAngle(0.f, 0.f, 0.f);
		}
		g_pLuaShitInterface->GetField(-1, "AimCone");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}
		auto AimCone = g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop();

		g_pLuaShitInterface->GetField(-1, "MaxSpreadInc");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}
		auto MaxSpreadInc = g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop();


		g_pLuaShitInterface->GetField(-1, "VelocitySensitivity");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}
		auto VelocitySensitivity = g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop();

		g_pLuaShitInterface->GetField(-1, "HipCone");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}
		auto HipCone = g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop();



		g_pLuaShitInterface->GetField(-1, "SpreadPerShot");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}
		auto SpreadPerShot = g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop();


		g_pLuaShitInterface->GetField(-1, "Owner");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}
		g_pLuaShitInterface->GetField(-1, "LastView");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(5);
			return QAngle(0.f, 0.f, 0.f);
		}
		auto LastView = g_pLuaShitInterface->GetVector(-1);
		g_pLuaShitInterface->Pop(2);

		g_pLuaShitInterface->GetField(-1, "Owner");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}
		g_pLuaShitInterface->GetField(-1, "ViewAff");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(5);
			return QAngle(0.f, 0.f, 0.f);
		}
		auto ViewAff = g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop(2);

		g_pLuaShitInterface->GetField(-1, "dt");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}
		g_pLuaShitInterface->GetField(-1, "Status");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(5);
			return QAngle(0.f, 0.f, 0.f);
		}
		auto Status = g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop(2);

		g_pLuaShitInterface->GetField(-1, "BurstAmount");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}
		auto BurstAmount = g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop();

		g_pLuaShitInterface->GetField(-1, "SpreadWait");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}
		auto SpreadWait = g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop();

		g_pLuaShitInterface->GetField(-1, "dt");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}
		g_pLuaShitInterface->GetField(-1, "Bipod");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(5);
			return QAngle(0.f, 0.f, 0.f);
		}
		auto Bipod = g_pLuaShitInterface->GetFlags(-1);
		g_pLuaShitInterface->Pop(2);

		g_pLuaShitInterface->GetField(-1, "AddSpread");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}
		auto AddSpread = g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop();

		g_pLuaShitInterface->GetField(-1, "ClumpSpread");
		double ClumpSpread;
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			ClumpSpread = 0;
		}
		else {
			ClumpSpread = g_pLuaShitInterface->GetNumber(-1);
		}

		g_pLuaShitInterface->Pop();

		g_pLuaShitInterface->GetField(-1, "CurCone");
		double g_CurrCone;
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_CurrCone = 0;
		}
		else {
			g_CurrCone = g_pLuaShitInterface->GetNumber(-1);
		}

		g_pLuaShitInterface->Pop();

		g_pLuaShitInterface->Pop(3);
		///Calculate Spread for FA:S 2

		float FinalViewAff;
		pCmd->viewangles.ToVector(&vForward, &vRight, &vUp);
		auto  Lerp = [](double t, double from, double to) {
			return ((1. - t) * from + t * to);
		};
		FinalViewAff = Lerp(0.25, ViewAff, Vector(vForward - *LastView).Length() * 0.5);

		float cone = HipCone * (pCmd->buttons& IN_DUCK ? 0.75 : 1.)*(Bipod ? 0.3 : 1.);////
		if (Status == FAS_STAT_ADS) {
			Ray_t ray = {};
			trace_t tr = {};
			Vector vEnd = {};
			CTraceFilter traceFilter = {};
			Vector view_dir = {};
			pCmd->viewangles.ToVector(&view_dir);
			vEnd = ply->EyePosition() + (view_dir * 30.f);
			ray.Init(ply->EyePosition(), vEnd);

			traceFilter.pSkip = ply;

			g_pEngineTrace->TraceRay(ray, MASK_SOLID, &traceFilter, &tr);
			if (!tr.DidHit())
				cone = AimCone;
		}
		Vector vel = {};
		ply->EstimateAbsVelocity(vel);

		//velocity problem

		//self.CurCone = math.Clamp(cone + self.AddSpread * (self.dt.Bipod and 0.5 or 1) + (vel / 10000 * self.VelocitySensitivity) * (self.dt.Status == FAS_STAT_ADS and 0.25 or 1) + self.Owner.ViewAff, 0, 0.09 + self.MaxSpreadInc)
	


		CurrCone = clamp(cone + AddSpread * (Bipod ? 0.5f : 1.f) + ((vel.Length()/ 10000.) * VelocitySensitivity) * (Status == FAS_STAT_ADS ? 0.25 : 1.) + FinalViewAff,0., 0.09f + MaxSpreadInc);      //((vel.Length()*VelocitySensitivity*10.f) / 10000.f)*(Status == FAS_STAT_ADS ? 0.25f : 1.f) + FinalViewAff, 0, 0.09f + MaxSpreadInc);

		g_CurrCone = CurrCone;

		auto serv_time = (SDK::Vars::g_tick*g_pGlobalVars->interval_per_tick);
		//auto nci = g_pEngine->GetNetChannelInfo();
		//if (nci) {
		//	serv_time += nci->GetLatency(FLOW_OUTGOING);
			// TIME_TO_TICKS(nci->GetLatency(FLOW_INCOMING)) + pLocal->m_nTickBase();
		//}

		g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
		g_pLuaShitInterface->GetField(-1, "math");
		g_pLuaShitInterface->GetField(-1, "randomseed");
		g_pLuaShitInterface->PushNumber(serv_time);
		g_pLuaShitInterface->Call(1, 0);
		g_pLuaShitInterface->Pop(2);

		g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
		g_pLuaShitInterface->GetField(-1, "math");
		g_pLuaShitInterface->GetField(-1, "Rand");
		g_pLuaShitInterface->PushNumber(-g_CurrCone);
		g_pLuaShitInterface->PushNumber(g_CurrCone);
		g_pLuaShitInterface->Call(2, 1);
		auto RandX = (float)g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop(3);

		g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
		g_pLuaShitInterface->GetField(-1, "math");
		g_pLuaShitInterface->GetField(-1, "Rand");
		g_pLuaShitInterface->PushNumber(-g_CurrCone);
		g_pLuaShitInterface->PushNumber(g_CurrCone);
		g_pLuaShitInterface->Call(2, 1);
		auto RandY = (float)g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop(3);
		QAngle LuaShitSp(RandX, RandY, 0.f);

		LuaShitSp *= 25.f;
		if (ClumpSpread&&ClumpSpread > 0) {
			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "math");
			g_pLuaShitInterface->GetField(-1, "Rand");
			g_pLuaShitInterface->PushNumber(-1.f);
			g_pLuaShitInterface->PushNumber(1.f);
			g_pLuaShitInterface->Call(2, 1);
			auto RandDirX = (float)g_pLuaShitInterface->GetNumber(-1);
			g_pLuaShitInterface->Pop(3);

			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "math");
			g_pLuaShitInterface->GetField(-1, "Rand");
			g_pLuaShitInterface->PushNumber(-1.f);
			g_pLuaShitInterface->PushNumber(1.f);
			g_pLuaShitInterface->Call(2, 1);
			auto RandDirY = (float)g_pLuaShitInterface->GetNumber(-1);
			g_pLuaShitInterface->Pop(3);

			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "math");
			g_pLuaShitInterface->GetField(-1, "Rand");
			g_pLuaShitInterface->PushNumber(-1.f);
			g_pLuaShitInterface->PushNumber(1.f);
			g_pLuaShitInterface->Call(2, 1);
			auto RandDirZ = (float)g_pLuaShitInterface->GetNumber(-1);
			g_pLuaShitInterface->Pop(3);

			QAngle RandVec(RandDirX, RandDirY, RandDirZ);

			RandVec *= ClumpSpread;

			LuaShitSp += RandVec;
		}
		return -LuaShitSp;
	}
	QAngle cw2_nospread(C_GMOD_Player * ply, C_BaseCombatWeapon * weap, CUserCmd * pCmd)
	{
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (!g_pLuaShitInterface)
			return QAngle(0.f, 0.f, 0.f);
		float CW2CurrCone = 0.f;

		g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
		g_pLuaShitInterface->GetField(-1, "FindMetaTable");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(2);
			return QAngle(0.f, 0.f, 0.f);
		}
		g_pLuaShitInterface->PushString("Player");
		g_pLuaShitInterface->Call(1, 1);
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(2);
			return QAngle(0.f, 0.f, 0.f);
		}
		g_pLuaShitInterface->GetField(-1, "GetActiveWeapon");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(3);
			return QAngle(0.f, 0.f, 0.f);
		}
		g_pLuaShitInterface->GetField(-3, "Entity");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}
		g_pLuaShitInterface->PushNumber(g_pEngine->GetLocalPlayer());
		g_pLuaShitInterface->Call(1, 1);
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}
		g_pLuaShitInterface->Call(1, 1);
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(3);
			return QAngle(0.f, 0.f, 0.f);
		}

		g_pLuaShitInterface->GetField(-1, "dt");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}

		//--
		g_pLuaShitInterface->GetField(-1, "State");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(5);
			return QAngle(0.f, 0.f, 0.f);
		}

		//Engine::msg_box("State %s", g_pLuaShitInterface->GetTypeName(g_pLuaShitInterface->GetType(-1)));
		auto State = g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop(1);

		g_pLuaShitInterface->Pop(1);
		//--

		g_pLuaShitInterface->GetField(-1, "MaxSpreadInc");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}
		//Engine::msg_box("MaxSpreadInc %s", g_pLuaShitInterface->GetTypeName(g_pLuaShitInterface->GetType(-1)));
		auto MaxSpreadInc = g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop(1);



		g_pLuaShitInterface->GetField(-1, "BaseCone");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}

		//Engine::msg_box("BaseCone %s", g_pLuaShitInterface->GetTypeName(g_pLuaShitInterface->GetType(-1)));
		auto BaseCone = g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop(1);

		g_pLuaShitInterface->GetField(-1, "AddSpread");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}

		//Engine::msg_box("AddSpread %s", g_pLuaShitInterface->GetTypeName(g_pLuaShitInterface->GetType(-1)));
		auto AddSpread = g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop(1);

		g_pLuaShitInterface->GetField(-1, "VelocitySensitivity");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}

		//Engine::msg_box("VelocitySensitivity %s", g_pLuaShitInterface->GetTypeName(g_pLuaShitInterface->GetType(-1)));
		auto VelocitySensitivity = g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop(1);


		g_pLuaShitInterface->GetField(-1, "AimMobilitySpreadMod");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}

		//Engine::msg_box("AimMobilitySpreadMod %s", g_pLuaShitInterface->GetTypeName(g_pLuaShitInterface->GetType(-1)));
		auto AimMobilitySpreadMod = g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop(1);

		g_pLuaShitInterface->GetField(-1, "HipSpread");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}

		//Engine::msg_box("HipSpread %s", g_pLuaShitInterface->GetTypeName(g_pLuaShitInterface->GetType(-1)));
		auto HipSpread = g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop(1);



		g_pLuaShitInterface->GetField(-1, "Owner");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}
		g_pLuaShitInterface->GetField(-1, "ViewAff");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(5);
			return QAngle(0.f, 0.f, 0.f);
		}
		auto ViewAff = g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop(1);

		g_pLuaShitInterface->GetField(-1, "LastView");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(5);
			return QAngle(0.f, 0.f, 0.f);
		}
		auto LastView = g_pLuaShitInterface->GetVector(-1);
		if (!LastView) {
			g_pLuaShitInterface->Pop(5);
			return QAngle(0.f, 0.f, 0.f);
		}

		g_pLuaShitInterface->Pop(1);
		g_pLuaShitInterface->Pop(1);

		auto LerpCW20 = [](double val, double min, double max)// --basically a wrapper that limits 'val' (aka progress) to a max of 1
		{
			auto  Lerp = [](double t, double from, double to) {
				return ((1. - t) * from + t * to);
			};
			val = val > 1.f ? 1.f : val;
			return Lerp(val, min, max);
		};
		g_pLuaShitInterface->GetField(-1, "CurCone");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return QAngle(0.f, 0.f, 0.f);
		}
		auto CurCone = (float)g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop(1);
		g_pLuaShitInterface->GetField(-1, "ClumpSpread");
		double ClumpSpread;
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			ClumpSpread = 0;
		}
		else {
			ClumpSpread = g_pLuaShitInterface->GetNumber(-1);
		}
		g_pLuaShitInterface->Pop(4);


		enum {
			CW_IDLE = 0,
			CW_RUNNING = 1,
			CW_AIMING = 2,
			CW_ACTION = 3,
			CW_CUSTOMIZE = 4,
			CW_HOLSTER_START = 5,
			CW_HOLSTER_END = 6,
			CW_PRONE_BUSY = 7,// --entering and leaving prone state
			CW_PRONE_MOVING = 8,// --crawling while prone
		};
		
		Vector vel = {};
		ply->EstimateAbsVelocity(vel);


		double final_cone = BaseCone;
		bool aiming = State == CW_AIMING;


		final_cone = final_cone + AddSpread;


		auto my_ViewAff = LerpCW20(double(TICK_INTERVAL * 10.f), ViewAff, double((pCmd->viewangles.ToVector() - *LastView).Length() * 0.5));

		if (aiming) {
			final_cone = min(final_cone + double((vel.Length() / 10000.) * VelocitySensitivity) * AimMobilitySpreadMod, HipSpread);
		}
		else {
			final_cone = final_cone + (double(vel.Length() / 10000. )* VelocitySensitivity);
		}

		/*
		if self.ShootWhileProne and self:isPlayerProne() then
			final = final + vel / 1000
		end
		*/
		final_cone = final_cone + my_ViewAff;

		final_cone = clamp(final_cone, 0., 0.09 + MaxSpreadInc);

		CW2CurrCone = final_cone;

		//CW2CurrCone = CurCone;
		///
		g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
		g_pLuaShitInterface->GetField(-1, "math");
		g_pLuaShitInterface->GetField(-1, "randomseed");
		g_pLuaShitInterface->PushNumber(pCmd->command_number);
		g_pLuaShitInterface->Call(1, 0);
		g_pLuaShitInterface->Pop(2);
		///
		if (pCmd->buttons &IN_DUCK)
			CW2CurrCone *= 0.85f;

		g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
		g_pLuaShitInterface->GetField(-1, "math");
		g_pLuaShitInterface->GetField(-1, "Rand");
		g_pLuaShitInterface->PushNumber(-CW2CurrCone);
		g_pLuaShitInterface->PushNumber(CW2CurrCone);
		g_pLuaShitInterface->Call(2, 1);
		auto RandX = (float)g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop(3);

		g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
		g_pLuaShitInterface->GetField(-1, "math");
		g_pLuaShitInterface->GetField(-1, "Rand");
		g_pLuaShitInterface->PushNumber(-CW2CurrCone);
		g_pLuaShitInterface->PushNumber(CW2CurrCone);
		g_pLuaShitInterface->Call(2, 1);
		auto RandY = (float)g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop(3);
		QAngle LuaShitSp(RandX, RandY, 0.f);

		LuaShitSp *= 25.f;

		if (ClumpSpread&&ClumpSpread > 0) {
			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "math");
			g_pLuaShitInterface->GetField(-1, "Rand");
			g_pLuaShitInterface->PushNumber(-1.f);
			g_pLuaShitInterface->PushNumber(1.f);
			g_pLuaShitInterface->Call(2, 1);
			auto RandDirX = (float)g_pLuaShitInterface->GetNumber(-1);
			g_pLuaShitInterface->Pop(3);

			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "math");
			g_pLuaShitInterface->GetField(-1, "Rand");
			g_pLuaShitInterface->PushNumber(-1.f);
			g_pLuaShitInterface->PushNumber(1.f);
			g_pLuaShitInterface->Call(2, 1);
			auto RandDirY = (float)g_pLuaShitInterface->GetNumber(-1);
			g_pLuaShitInterface->Pop(3);

			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "math");
			g_pLuaShitInterface->GetField(-1, "Rand");
			g_pLuaShitInterface->PushNumber(-1.f);
			g_pLuaShitInterface->PushNumber(1.f);
			g_pLuaShitInterface->Call(2, 1);
			auto RandDirZ = (float)g_pLuaShitInterface->GetNumber(-1);
			g_pLuaShitInterface->Pop(3);

			QAngle RandVec(RandDirX, RandDirY, RandDirZ);

			RandVec *= ClumpSpread;

			LuaShitSp += RandVec;
		}
		return -LuaShitSp;
	}
	struct m9k_weap {
		ConVar * M9KDamageMultiplier;
		ConVar * M9KDisablePenetration;
		float Damage;
		const char* Ammo;
		int MaxRicochet;
		int RicochetCoin;
		bool Ricochet;
		int MaxPenetration;
		float PainMulti;
		float basedamage;
	};
	bool build_m9k_info(m9k_weap& target) {
		target.M9KDamageMultiplier = g_pCvar->FindVar("M9KDamageMultiplier");
		target.PainMulti = 1.f;
		if (!target.M9KDamageMultiplier)
			target.PainMulti = 1.f;
		else {
			target.PainMulti = target.M9KDamageMultiplier->flVal;
		}

		target.M9KDisablePenetration = g_pCvar->FindVar("M9KDisablePenetration");
		if (!target.M9KDisablePenetration)
			return false;

		g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
		g_pLuaShitInterface->GetField(-1, "FindMetaTable");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(2);
			return false;
		}
		g_pLuaShitInterface->PushString("Player");
		g_pLuaShitInterface->Call(1, 1);
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(2);
			return false;
		}
		g_pLuaShitInterface->GetField(-1, "GetActiveWeapon");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(3);
			return false;
		}
		g_pLuaShitInterface->GetField(-3, "Entity");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return false;
		}
		g_pLuaShitInterface->PushNumber(g_pEngine->GetLocalPlayer());
		g_pLuaShitInterface->Call(1, 1);
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return false;
		}
		g_pLuaShitInterface->Call(1, 1);
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(3);
			return false;
		}

		g_pLuaShitInterface->GetField(-1, "Primary");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return false;
		}
		g_pLuaShitInterface->GetField(-1, "Damage");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(5);
			return false;
		}
		target.Damage = (float)g_pLuaShitInterface->GetFlags(-1);///work
		g_pLuaShitInterface->Pop(1);

		g_pLuaShitInterface->GetField(-1, "Ammo");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(5);
			return false;
		}
		target.Ammo = g_pLuaShitInterface->GetString();///
		g_pLuaShitInterface->Pop(2);//

		g_pLuaShitInterface->GetField(-1, "MaxRicochet");

		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return false;
		}
		target.MaxRicochet = g_pLuaShitInterface->GetFlags(-1);///
		g_pLuaShitInterface->Pop();

		g_pLuaShitInterface->GetField(-1, "RicochetCoin");

		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return false;
		}
		target.RicochetCoin = g_pLuaShitInterface->GetFlags(-1);///
		g_pLuaShitInterface->Pop();

		g_pLuaShitInterface->Pop(3);

		target.Ricochet = false;

		if (strcmp(target.Ammo, "pistol") == 0 || strcmp(target.Ammo, "buckshot") == 0 || strcmp(target.Ammo, "slam") == 0 || strcmp(target.Ammo, "SniperPenetratedRound") == 0) {
			target.Ricochet = true;
		}
		else {
			if (target.RicochetCoin == 1) {
				target.Ricochet = true;
			}
			else if (target.RicochetCoin >= 2) {
				target.Ricochet = true;
			}
		}
		target.MaxPenetration;
		if (strcmp(target.Ammo, "SniperPenetratedRound") == 0) {
			target.MaxPenetration = 20;
		}
		else if (strcmp(target.Ammo, "pistol") == 0) {
			target.MaxPenetration = 9;
		}
		else if (strcmp(target.Ammo, "357") == 0) {
			target.MaxPenetration = 12;
		}
		else if (strcmp(target.Ammo, "smg1") == 0) {
			target.MaxPenetration = 14;
		}
		else if (strcmp(target.Ammo, "ar2") == 0) {
			target.MaxPenetration = 16;
		}
		else if (strcmp(target.Ammo, "buckshot") == 0) {
			target.MaxPenetration = 5;
		}
		else if (strcmp(target.Ammo, "slam") == 0) {
			target.MaxPenetration = 5;
		}
		else if (strcmp(target.Ammo, "AirboatGun") == 0) {
			target.MaxPenetration = 17;
		}
		else {
			target.MaxPenetration = 14;
		}
		return true;

	}

	bool m9k_bullet_penetrate(m9k_weap &current_weap, const Vector &vecEyePos, const Vector &point, const C_BaseCombatWeapon * weap, Vector& EndPos, int& bouncenum);
	bool m9k_wallscan(const Vector &vecEyePos, const Vector &point, const C_BaseCombatWeapon * weap, C_GMOD_Player*& ply_out) {

		ply_out = NULL;
		auto pLocal = C_GMOD_Player::GetLocalPlayer();
		if (!pLocal || !pLocal->IsGood())
			return false;

		m9k_weap current_weap = {};
		if (!build_m9k_info(current_weap))
			return false;

		current_weap.basedamage = current_weap.PainMulti * current_weap.Damage;
		if (!current_weap.Ricochet || current_weap.M9KDisablePenetration->flVal)
			return false;
		Vector start = vecEyePos;
		Vector end = point;
		trace_t Trace = {};
		surfacedata_t *SurfaceData;
		Vector dir = { };
		for (auto bouncenum = 0; bouncenum <= current_weap.MaxRicochet; bouncenum++) {

			SDK::Function::UTIL_TraceLine(start, end, MASK_VISIBLE, pLocal, 0, &Trace);
			SurfaceData = g_PhysicsSurfaceProps->GetSurfaceData(Trace.surface.surfaceProps);

			Vector dir = (end - start);
			dir.Normalize();
			Vector hit_pos = start + (end - start)*Trace.fraction;

			//constexpr auto kek = (int)CHAR_TEX_FLESH;
			//70
			if (Trace.m_pEnt&&ToGMOD_Player(ToBasePlayer((C_BaseEntity*)Trace.m_pEnt))) {


				ply_out = ToGMOD_Player(ToBasePlayer((C_BaseEntity*)Trace.m_pEnt));
				return true;
				//return current_weap.basedamage > 15.f;//uffya
			}
			if (Trace.startsolid || Trace.fraction >= 1.f || Trace.fraction <= 0.f) {
				return false;//well that's kinda bad
			}

			for (; bouncenum <= current_weap.MaxRicochet; bouncenum++) {//try to penetrate

				start = hit_pos;
				if (SurfaceData->game.material == CHAR_TEX_PLASTIC || SurfaceData->game.material == CHAR_TEX_GLASS || SurfaceData->game.material == CHAR_TEX_WOOD || SurfaceData->game.material == CHAR_TEX_FLESH || SurfaceData->game.material == CHAR_TEX_ALIENFLESH) {
					end = start + (dir * current_weap.MaxPenetration * 2);
				}
				else {
					end = start + (dir * current_weap.MaxPenetration);
				}
				SDK::Function::UTIL_TraceLine(start, end, MASK_VISIBLE, pLocal, 0, &Trace);
				SurfaceData = g_PhysicsSurfaceProps->GetSurfaceData(Trace.surface.surfaceProps);

				hit_pos = start + (end - start)*Trace.fraction;

				double fDamageMulti = 0.5;
				if (strcmp(current_weap.Ammo, "SniperPenetratedRound") == 0) {
					fDamageMulti = 1.;
				}
				else if (SurfaceData->game.material == CHAR_TEX_CONCRETE || SurfaceData->game.material == CHAR_TEX_METAL) {
					fDamageMulti = 0.3;
				}
				else if (SurfaceData->game.material == CHAR_TEX_WOOD || SurfaceData->game.material == CHAR_TEX_PLASTIC || SurfaceData->game.material == CHAR_TEX_GLASS) {
					fDamageMulti = 0.8;
				}
				else if (SurfaceData->game.material == CHAR_TEX_FLESH || SurfaceData->game.material == CHAR_TEX_ALIENFLESH) {
					fDamageMulti = 0.9;
				}

				current_weap.basedamage *= fDamageMulti;
				if (Trace.m_pEnt&&ToGMOD_Player(ToBasePlayer((C_BaseEntity*)Trace.m_pEnt))) {


					ply_out = ToGMOD_Player(ToBasePlayer((C_BaseEntity*)Trace.m_pEnt));
					//return current_weap.basedamage > 15.f;//uffya
					return true;
				}
				if (Trace.startsolid || Trace.fraction >= 1.f || Trace.fraction <= 0.f) {
					hit_pos = end;//we penetate that fucking wall
					break;
				}

			}
			start = hit_pos;
			end = point;

		}
		return false;
	}

	bool weapon_is_cw2(IClientEntity * weapon)
	{
		if (strcmp("CWeaponSWEP", weapon->GetClientClass()->GetName()) == 0) {
			if (weapon->GetScriptName()) {
				auto str = std::string(weapon->GetScriptName());
				auto lua_base = Lua::GetWeaponBase(weapon->entindex());
				if (str.find("cw") != -1 || str.find("atow") != -1 || str.find("khr") != -1) {
					return true;
				}
				else if (lua_base != nullptr) {
					if (std::string(lua_base).find("swb") != -1) {
						return true;
					}
				}
			}
		}
		return false;
	}

	bool cw2_wallscan(const Vector & vecEyePos, const Vector & point, const C_BaseCombatWeapon * weap, C_GMOD_Player *& ply_out)
	{
		ply_out = NULL;
		auto pLocal = C_GMOD_Player::GetLocalPlayer();
		if (!pLocal || !pLocal->IsGood())
			return false;
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (!g_pLuaShitInterface)
			return false;



		double PenStr;
		double PenMod;
		{
			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "FindMetaTable");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return false;
			}
			g_pLuaShitInterface->PushString("Player");
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(2);
				return false;
			}
			g_pLuaShitInterface->GetField(-1, "GetActiveWeapon");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return false;
			}
			g_pLuaShitInterface->GetField(-3, "Entity");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(4);
				return false;
			}
			g_pLuaShitInterface->PushNumber(g_pEngine->GetLocalPlayer());
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(4);
				return false;
			}
			g_pLuaShitInterface->Call(1, 1);
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(3);
				return false;
			}
			g_pLuaShitInterface->GetField(-1, "PenStr");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(4);
				return false;
			}
			PenStr = g_pLuaShitInterface->GetNumber(-1);

			g_pLuaShitInterface->GetField(-2, "PenMod");

			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(5);
				return false;
			}
			PenMod = g_pLuaShitInterface->GetNumber(-1);
			g_pLuaShitInterface->Pop(5);
		}

		auto get_PenetrationMaterialInteraction = [](unsigned short material) -> double
		{
			auto out = 1.;
			if (material == CHAR_TEX_SAND) {
				out = 0.5;
			}
			else if (material == CHAR_TEX_DIRT) {
				out = 0.8;
			}
			else if (material == CHAR_TEX_METAL) {
				out = 1.1;
			}
			else if (material == CHAR_TEX_TILE) {
				out = 0.9;
			}
			else if (material == CHAR_TEX_WOOD) {
				out = 1.2;
			}
			return out;
		};
		/*
		SWEP.NormalTraceMask = bit.bor(CONTENTS_SOLID, CONTENTS_OPAQUE, CONTENTS_MOVEABLE, CONTENTS_DEBRIS, CONTENTS_MONSTER, CONTENTS_HITBOX, 402653442, CONTENTS_WATER)
		SWEP.WallTraceMask = bit.bor(CONTENTS_TESTFOGVOLUME, CONTENTS_EMPTY, CONTENTS_MONSTER, CONTENTS_HITBOX)
		*/

		DWORD NormalTraceMask = CONTENTS_SOLID| CONTENTS_OPAQUE| CONTENTS_MOVEABLE| CONTENTS_DEBRIS| CONTENTS_MONSTER| CONTENTS_HITBOX| 402653442| CONTENTS_WATER;
		DWORD WallTraceMask = CONTENTS_TESTFOGVOLUME| CONTENTS_EMPTY| CONTENTS_MONSTER| CONTENTS_HITBOX;


		Vector start = vecEyePos;
		Vector end = point;
		trace_t Trace = {};
		SDK::Function::UTIL_TraceLine(start, end, NormalTraceMask, pLocal, 0, &Trace);
		surfacedata_t *SurfaceData = g_PhysicsSurfaceProps->GetSurfaceData(Trace.surface.surfaceProps);

		Vector dir = (end - start);
		dir.Normalize();
		Vector hit_pos = start + (end - start)*Trace.fraction;
		//SWEP:Think()
		//CalculateSpread

		if (Trace.m_pEnt&&Trace.m_pEnt->GetClientClass() && Trace.m_pEnt->GetClientClass()->GetName() && strcmp(Trace.m_pEnt->GetClientClass()->GetName(), "CGMOD_Player") == 0) {
			ply_out = ToGMOD_Player(ToBasePlayer((C_BaseEntity*)Trace.m_pEnt));
			return true;
		}
		else {
			double dot_reflection = 0.0;
			if (SurfaceData->game.material != CHAR_TEX_SLOSH) {
				dot_reflection = -dir.Dot(Trace.plane.normal);

				if (dot_reflection > 0.26) {
					start = hit_pos;
					end = start + dir * PenStr * get_PenetrationMaterialInteraction(SurfaceData->game.material) * PenMod;
					SDK::Function::UTIL_TraceLine(start, end, WallTraceMask, pLocal, 0, &Trace);
					SurfaceData = g_PhysicsSurfaceProps->GetSurfaceData(Trace.surface.surfaceProps);
					hit_pos = start + (end - start)*Trace.fraction;

					start = hit_pos;
					end = start + dir * 0.1;
					SDK::Function::UTIL_TraceLine(start, end, NormalTraceMask, pLocal, 0, &Trace);
					SurfaceData = g_PhysicsSurfaceProps->GetSurfaceData(Trace.surface.surfaceProps);
					hit_pos = start + (end - start)*Trace.fraction;


					//bul.Damage = bul.Damage * 0.5
					start = hit_pos;
					end = point;
					SDK::Function::UTIL_TraceLine(start, end, MASK_SHOT, pLocal, 0, &Trace);
					if (Trace.m_pEnt&&Trace.m_pEnt->GetClientClass() && Trace.m_pEnt->GetClientClass()->GetName() && strcmp(Trace.m_pEnt->GetClientClass()->GetName(), "CGMOD_Player") == 0) {
						ply_out = ToGMOD_Player(ToBasePlayer((C_BaseEntity*)Trace.m_pEnt));
						return true;
					}


				}
				else {
					return false;
				}

			}
			else {
				return false;
			}
		}
		return false;

	}


	bool m9k_bullet_penetrate(m9k_weap &current_weap, const Vector &vecEyePos, const Vector &point, const C_BaseCombatWeapon * weap, Vector& EndPos, int& bouncenum)
	{
		auto pLocal = C_GMOD_Player::GetLocalPlayer();
		if (!pLocal || !pLocal->IsGood())
			return false;

		Vector start = vecEyePos;
		Vector end = point;
		trace_t Trace = {};

		SDK::Function::UTIL_TraceLine(start, end, MASK_SHOT, pLocal, 0, &Trace);
		surfacedata_t *SurfaceData = g_PhysicsSurfaceProps->GetSurfaceData(Trace.surface.surfaceProps);

		Vector dir = (end - start);
		dir.Normalize();
		Vector hit_pos = start + (end - start)*Trace.fraction;
		EndPos = hit_pos;
		//if (Trace.m_pEnt&&ToGMOD_Player(ToBasePlayer((C_BaseEntity*)Trace.m_pEnt))) {
		//	return false;
		//}

		if (Trace.m_pEnt&&ToGMOD_Player(ToBasePlayer((C_BaseEntity*)Trace.m_pEnt))) {
			g_pEngine->ClientCmd("echo hit_client");
		}
		if (Trace.fraction >= 1.f || Trace.fraction <= 0.f) {

			return false;
		}

		for (; bouncenum <= current_weap.MaxRicochet; bouncenum++) {

			start = hit_pos;
			if (SurfaceData->game.material == CHAR_TEX_PLASTIC || SurfaceData->game.material == CHAR_TEX_GLASS || SurfaceData->game.material == CHAR_TEX_WOOD || SurfaceData->game.material == CHAR_TEX_FLESH || SurfaceData->game.material == CHAR_TEX_ALIENFLESH) {
				end = start * (dir * current_weap.MaxPenetration * 2);
			}
			else {
				end = start * (dir * current_weap.MaxPenetration);
			}
			SDK::Function::UTIL_TraceLine(start, end, MASK_SHOT, pLocal, 0, &Trace);
			SurfaceData = g_PhysicsSurfaceProps->GetSurfaceData(Trace.surface.surfaceProps);

			hit_pos = start + (end - start)*Trace.fraction;

			double fDamageMulti = 0.5;
			if (strcmp(current_weap.Ammo, "SniperPenetratedRound") == 0) {
				fDamageMulti = 1.;
			}
			else if (SurfaceData->game.material == CHAR_TEX_CONCRETE || SurfaceData->game.material == CHAR_TEX_METAL) {
				fDamageMulti = 0.3;
			}
			else if (SurfaceData->game.material == CHAR_TEX_WOOD || SurfaceData->game.material == CHAR_TEX_PLASTIC || SurfaceData->game.material == CHAR_TEX_GLASS) {
				fDamageMulti = 0.8;
			}
			else if (SurfaceData->game.material == CHAR_TEX_FLESH || SurfaceData->game.material == CHAR_TEX_ALIENFLESH) {
				fDamageMulti = 0.9;
			}

			current_weap.basedamage *= fDamageMulti;

			if (Trace.m_pEnt&&ToGMOD_Player(ToBasePlayer((C_BaseEntity*)Trace.m_pEnt))) {
				g_pEngine->ClientCmd("echo hit_client");
			}
			if (Trace.fraction >= 1.f || Trace.fraction <= 0.f) {
				g_pEngine->ClientCmd("echo did't penetrate in m9k_bullet_penetrate");
				hit_pos = start + dir * 15.f;
				EndPos = hit_pos;
				bouncenum++;
				return true;
			}
			EndPos = hit_pos;
		}
		return true;
	}
}