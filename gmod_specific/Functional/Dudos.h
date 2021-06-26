#pragma once
#include "../../engine/Core/Basic/Basic.h"
#include "../SDK.h"

class Dudos:public Basic::Singleton<Dudos>  {
public:
	void Dudos_tick();


	bf_write * GetMethodOneTap();
	bf_write * GetMethod1();
	bf_write * GetMethod2();
	bf_write * GetMethod4();
	bf_write * GetMethodTh();

};