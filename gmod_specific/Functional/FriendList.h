#pragma once
#include "../../engine/Core/Basic/Basic.h"
#include "../SDK.h"

class FriendList : public Basic::Singleton<FriendList>
{
public:
	bool IsFriend(C_GMOD_Player* ply);
	void AddToFriend(C_GMOD_Player* ply);
	void DelFromFriend(C_GMOD_Player* ply);
	void Update();

	std::vector<Core::VarUtil::unk_Var*>Friend_vec;
	bool is_vec_change = false;
};