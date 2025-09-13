#pragma once

#include "../../../sdk/sdk.h"

#include "../entities/entities.h"

class c_engineprediction
{
	CMoveData m_MoveData = {};
	float m_fOldCurrentTime = 0.0f;
	float m_fOldFrameTime = 0.0f;
	int m_nOldTickCount = 0;

	int get_tickbase(CUserCmd* pCmd, C_TFPlayer* pLocal);

public:
	int flags{};

	void start(CUserCmd* pCmd);
	void end();
};

MAKE_SINGLETON_SCOPED(c_engineprediction, engineprediction, f)
