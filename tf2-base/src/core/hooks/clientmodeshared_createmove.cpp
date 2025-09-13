#include "../../sdk/sdk.h"

#include "../features/engineprediction/engineprediction.h"

MAKE_HOOK(ClientModeShared_CreateMove, Memory::GetVFunc(I::ClientModeShared, 21), bool, __fastcall,
    CClientModeShared* ecx, float inputsampletime, CUserCmd* cmd)
{
    if (!cmd || !cmd->command_number)
        return CALL_ORIGINAL(ecx, inputsampletime, cmd);

    I::Prediction->Update
    (
        I::ClientState->m_nDeltaTick,
        I::ClientState->m_nDeltaTick > 0,
        I::ClientState->last_command_ack,
        I::ClientState->lastoutgoingcommand + I::ClientState->chokedcommands
    );

	bool* send_packet = reinterpret_cast<bool*>(uintptr_t(_AddressOfReturnAddress()) + 0x128);

	Vec3 oldangles = cmd->viewangles;
	float oldside = cmd->sidemove;
	float oldforward = cmd->forwardmove;

   // MessageBoxA(0, "creatmove", "tf2-base", MB_OK);

	f::engineprediction->start(cmd);
	{
		// run aimbot and triggerbot here
	}
	f::engineprediction->end();

	{
		static bool set = false;

		if (g::psilentangles)
		{
			*send_packet = false;
			set = true;
		}

		else
		{
			if (set)
			{
				*send_packet = true;
				cmd->viewangles = oldangles;
				cmd->sidemove = oldside;
				cmd->forwardmove = oldforward;
				set = false;
			}
		}
	}

    return (g::silentangles || g::psilentangles) ? false : CALL_ORIGINAL(ecx, inputsampletime, cmd);
}
