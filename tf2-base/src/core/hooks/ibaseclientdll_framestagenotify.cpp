#include "../../sdk/sdk.h"

#include "../features/entities/entities.h"

MAKE_HOOK(IBaseClientDLL_FrameStageNotify, Memory::GetVFunc(I::BaseClientDLL, 35), void, __fastcall,
	void* ecx, ClientFrameStage_t curStage)
{
	CALL_ORIGINAL(ecx, curStage);

	switch (curStage)
	{
	case FRAME_NET_UPDATE_START:
	{
		f::entities->clearcache();

		break;
	}

	case FRAME_NET_UPDATE_END:
	{
		f::entities->updatecache();

		//MessageBoxA(0, "FRAME_NET_UPDATE_END", "tf2-base", MB_OK);

		if (const auto local = f::entities->getlocal())
		{
			for (const auto entity : f::entities->getgroup(EEntGroup::PLAYERS_ALL))
			{
				if (!entity || entity == local)
					continue;

				const auto player = entity->As<C_TFPlayer>();

				if (const auto difference = std::clamp(TIME_TO_TICKS(player->m_flSimulationTime() - player->m_flOldSimulationTime()), 0, 22))
				{
					//deal with animations, local player is dealt with in RunCommand
					const float oldframetime = I::GlobalVars->frametime;

					I::GlobalVars->frametime = I::Prediction->m_bEnginePaused ? 0.0f : TICK_INTERVAL;

					for (int n = 0; n < difference; n++)
					{
						g::updatinganims = true;
						player->UpdateClientSideAnimation();
						g::updatinganims = false;
					}

					I::GlobalVars->frametime = oldframetime;
				}
			}
		}

		break;
	}

	case FRAME_RENDER_START:
	{
		break;
	}

	default: break;

	}
}