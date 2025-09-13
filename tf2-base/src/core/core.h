#pragma once

#include "../sdk/sdk.h"

class c_core
{
	bool unload = false;

public:
	void start();
	void loop();
	void shutdown();
};

MAKE_SINGLETON(c_core, core);