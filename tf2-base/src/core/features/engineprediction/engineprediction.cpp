#include "EnginePrediction.h"

int c_engineprediction::get_tickbase(CUserCmd* pCmd, C_TFPlayer* local)
{
	static int nTick = 0;
	static CUserCmd* pLastCmd = nullptr;

	if (pCmd)
	{
		if (!pLastCmd || pLastCmd->hasbeenpredicted)
			nTick = local->m_nTickBase();

		else nTick++;

		pLastCmd = pCmd;
	}

	return nTick;
}

void c_engineprediction::start(CUserCmd* pCmd)
{
	if (!I::MoveHelper)
		return;

	if (const auto local = f::entities->get_local())
	{
		flags = local->m_fFlags();

		I::MoveHelper->SetHost(local);
		local->SetCurrentCommand(pCmd);

		static constexpr int MAX_INT = std::numeric_limits<int>::max();
		*sdk::randomseed() = MD5_PseudoRandom(pCmd->command_number) & MAX_INT;

		m_fOldCurrentTime = I::GlobalVars->curtime;
		m_fOldFrameTime = I::GlobalVars->frametime;
		m_nOldTickCount = I::GlobalVars->tickcount;

		const int old_tickbase = local->m_nTickBase();
		const bool old_infirsttimepred = I::Prediction->m_bFirstTimePredicted;
		const bool old_inpred = I::Prediction->m_bInPrediction;

		const int server_ticks = get_tickbase(pCmd, local);

		I::GlobalVars->curtime = TICKS_TO_TIME(server_ticks);
		I::GlobalVars->frametime = (I::Prediction->m_bEnginePaused ? 0.0f : TICK_INTERVAL);
		I::GlobalVars->tickcount = server_ticks;

		I::Prediction->m_bFirstTimePredicted = false;
		I::Prediction->m_bInPrediction = true;

		I::Prediction->SetLocalViewAngles(pCmd->viewangles);

		I::Prediction->SetupMove(local, pCmd, I::MoveHelper, &m_MoveData);
		I::GameMovement->ProcessMovement(local, &m_MoveData);
		I::Prediction->FinishMove(local, pCmd, &m_MoveData);

		local->m_nTickBase() = old_tickbase;

		I::Prediction->m_bInPrediction = old_inpred;
		I::Prediction->m_bFirstTimePredicted = old_infirsttimepred;
	}
}

void c_engineprediction::end()
{
	if (const auto local = f::entities->get_local())
	{
		I::GlobalVars->curtime = m_fOldCurrentTime;
		I::GlobalVars->frametime = m_fOldFrameTime;
		I::GlobalVars->tickcount = m_nOldTickCount;

		I::MoveHelper->SetHost(nullptr);
		local->SetCurrentCommand(nullptr);

		*sdk::randomseed() = -1;
	}
}
