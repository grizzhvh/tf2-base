#pragma once

#include "TF2/IMatSystemSurface.h"
#include "TF2/cdll_int.h"
#include "TF2/icliententitylist.h"
#include "TF2/ivmodelinfo.h"
#include "TF2/ivdebugoverlay.h"
#include "TF2/clientmode_shared.h"
#include "TF2/imaterialsystem.h"
#include "TF2/ivmodelrender.h"
#include "TF2/ienginevgui.h"
#include "TF2/ivrenderview.h"
#include "TF2/globalvars_base.h"
#include "TF2/icvar.h"
#include "TF2/prediction.h"
#include "TF2/igamemovement.h"
#include "TF2/netmessages.h"
#include "TF2/iinputsystem.h"
#include "TF2/iinput.h"
#include "TF2/istudiorender.h"
#include "TF2/igameevents.h"
#include "TF2/c_tf_playerresource.h"
#include "TF2/itoolentity.h"
#include "TF2/random.h"
#include "TF2/client.h"
#include "TF2/iviewrender.h"
#include "TF2/iviewrender_beams.h"
#include "TF2/vphysics.h"
#include "TF2/renderutils.h"

#include "TF2/keyvalues.h"
#include "TF2/c_baseobject.h"
#include "TF2/imaterialvar.h"
#include "TF2/itexture.h"
#include "TF2/MD5.h"
#include "TF2/c_tf_player.h"

MAKE_SIGNATURE(RandomSeed, "client.dll", "0F B6 1D ? ? ? ? 89 9D", 0x0);
MAKE_SIGNATURE(SharedRandomInt, "client.dll", "48 89 5C 24 ? 57 48 83 EC ? 8B FA 41 8B D8", 0x0);
MAKE_SIGNATURE(BInEndOfMatch, "client.dll", "48 83 EC ? 48 8B 05 ? ? ? ? 48 85 C0 74 ? 83 78 ? ? 75", 0x0);
MAKE_SIGNATURE(GetClientInterpAmount, "client.dll", "40 53 48 83 EC ? 8B 05 ? ? ? ? A8 ? 75 ? 48 8B 0D ? ? ? ? 48 8D 15", 0x0);
MAKE_SIGNATURE(LookupSequence, "client.dll", "48 89 5C 24 ? 55 48 83 EC ? 48 8B EA 48 8B D9 48 85 C9", 0x0);

namespace sdk
{
	inline BOOL CALLBACK teamfortresswindow(HWND hwnd, LPARAM lParam)
	{
		char windowtitle[1024];
		GetWindowTextA(hwnd, windowtitle, sizeof(windowtitle));
		if (std::string(windowtitle).find("Team Fortress 2 - ") == 0)
			*reinterpret_cast<HWND*>(lParam) = hwnd;

		return TRUE;
	}

	inline HWND getteamfortresswindow()
	{
		static HWND hwwindow = nullptr;
		while (!hwwindow)
		{
			EnumWindows(teamfortresswindow, reinterpret_cast<LPARAM>(&hwwindow));
			Sleep(100);
		}
		return hwwindow;
	}

	inline bool isgamewindowinfocus()
	{
		return GetForegroundWindow() == sdk::getteamfortresswindow();
	}

	inline int* randomseed()
	{
		static auto dest = Memory::RelToAbs(Signatures::RandomSeed.Get());
		return reinterpret_cast<int*>(dest);
	}

	inline void getprojectilefiresetuprebuilt(C_TFPlayer* player, Vec3 offset, const Vec3& ang_in, Vec3& pos_out, Vec3& ang_out, bool pipes)
	{
		static auto cl_flipviewmodels{ I::CVar->FindVar("cl_flipviewmodels") };

		if (!cl_flipviewmodels)
		{
			return;
		}

		if (cl_flipviewmodels->GetBool())
		{
			offset.y *= -1.0f;
		}

		Vec3 forward{}, right{}, up{};
		Math::AngleVectors(ang_in, &forward, &right, &up);

		auto shoot_pos{ player->GetShootPos() };

		pos_out = shoot_pos + (forward * offset.x) + (right * offset.y) + (up * offset.z);

		if (pipes)
		{
			ang_out = ang_in;
		}
		else
		{
			auto end_pos{ shoot_pos + (forward * 2000.0f) };
			Math::VectorAngles(end_pos - pos_out, ang_out);
		}
	}

	static float getlerp()
	{
		static ConVar* cl_interp = I::CVar->FindVar("cl_interp");
		static ConVar* cl_interp_ratio = I::CVar->FindVar("cl_interp_ratio");
		static ConVar* cl_updaterate = I::CVar->FindVar("cl_updaterate");

		return std::max(cl_interp->GetFloat(), cl_interp_ratio->GetFloat() / cl_updaterate->GetFloat());
	}

	static Vec3 gethitboxposfrommatrix(C_BaseAnimating* pAnimating, int nHitbox, matrix3x4_t* pMatrix)
	{
		auto pModel = pAnimating->GetModel();
		if (!pModel)
			return {};

		auto pHDR = I::ModelInfoClient->GetStudiomodel(pModel);
		if (!pHDR)
			return {};

		auto pSet = pHDR->pHitboxSet(pAnimating->m_nHitboxSet());
		if (!pSet)
			return {};

		auto pBox = pSet->pHitbox(nHitbox);
		if (!pBox)
			return {};

		Vec3 vOut = {};
		Math::VectorTransform((pBox->bbmin + pBox->bbmax) * 0.5f, pMatrix[pBox->bone], vOut);
		return vOut;
	}

	static void gethitboxinfofrommatrix(C_BaseAnimating* pAnimating, int nHitbox, matrix3x4_t* pMatrix, Vec3* pCenter, Vec3* pMins, Vec3* pMaxs)
	{
		auto pModel = pAnimating->GetModel();
		if (!pModel)
			return;

		auto pHDR = I::ModelInfoClient->GetStudiomodel(pModel);
		if (!pHDR)
			return;

		auto pSet = pHDR->pHitboxSet(pAnimating->m_nHitboxSet());
		if (!pSet)
			return;

		auto pBox = pSet->pHitbox(nHitbox);
		if (!pBox)
			return;

		if (pMins)
			*pMins = pBox->bbmin;

		if (pMaxs)
			*pMaxs = pBox->bbmax;

		if (pCenter)
			Math::VectorTransform((pBox->bbmin + pBox->bbmax) * 0.5f, pMatrix[pBox->bone], *pCenter);
	}

	inline float getlatency()
	{
		if (auto pNet = I::EngineClient->GetNetChannelInfo())
			return pNet->GetLatency(FLOW_INCOMING) + pNet->GetLatency(FLOW_OUTGOING);

		return 0.0f;
	}

	inline float getgravity()
	{
		static ConVar* sv_gravity = I::CVar->FindVar("sv_gravity");
		return sv_gravity ? sv_gravity->GetFloat() : 0.0f;
	}

	static float randomfloat(float min_val, float max_val)
	{
		static auto fn = reinterpret_cast<float(__cdecl*)(float, float)>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomFloat"));
		return fn(min_val, max_val);
	}

	static int randomint(int min_val, int max_val)
	{
		static auto fn = reinterpret_cast<int(__cdecl*)(int, int)>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomInt"));
		return fn(min_val, max_val);
	}

	static void randomseed(unsigned int seed)
	{
		static auto fn = reinterpret_cast<void(__cdecl*)(unsigned int)>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomSeed"));
		fn(seed);
	}

	static int findcmdnumwithseed(int nCommandNumber, int nDesiredSeed)
	{
		int nCmdNum = nCommandNumber, nIter = 0;

		while (nIter++ < 1024)
		{
			int nSeed = MD5_PseudoRandom(++nCmdNum) & std::numeric_limits<int>::max();
			if ((nSeed & 255) == nDesiredSeed)
				return nCmdNum;
		}

		return nCommandNumber;
	}

	static int sharedrandomint(const char* sharedname, int iMinVal, int iMaxVal, int additionalSeed)
	{
		using fn = int(__fastcall*)(const char*, int, int, int);
		return reinterpret_cast<fn>(Signatures::SharedRandomInt.Get())(sharedname, iMinVal, iMaxVal, additionalSeed);
	}

	static bool binendofmatch()
	{
		return reinterpret_cast<bool(__cdecl*)()>(Signatures::BInEndOfMatch.Get())();
	}

	static int lookupsequence(CStudioHdr* pstudiohdr, const char* label)
	{
		return reinterpret_cast<int(__cdecl*)(CStudioHdr*, const char*)>(Signatures::LookupSequence.Get())(pstudiohdr, label);
	}

	static int createtexturefromarray(const unsigned char* rgba, int w, int h)
	{
		int nTextureIdOut = I::MatSystemSurface->CreateNewTextureID(true);
		I::MatSystemSurface->DrawSetTextureRGBAEx(nTextureIdOut, rgba, w, h, IMAGE_FORMAT_BGRA8888);
		return nTextureIdOut;
	}

	static void walkto(CUserCmd* pCmd, const Vec3& from, const Vec3& to, float scale)
	{
		auto delta = to - from;

		if (delta.Length() == 0.0f)
			return;

		Vec3 deltaDir{};
		Math::VectorAngles({ delta.x, delta.y, 0.0f }, deltaDir);

		const auto yaw{ DEG2RAD(deltaDir.y - pCmd->viewangles.y) };
		pCmd->forwardmove = std::cosf(yaw) * (450.0f * scale);
		pCmd->sidemove = -std::sinf(yaw) * (450.0f * scale);
	}
}

inline float getclientinterpamount()
{
	return reinterpret_cast<float(__cdecl*)()>(Signatures::GetClientInterpAmount.Get())();
}

inline double plat_floattime()
{
	static auto fn{ reinterpret_cast<double(__cdecl*)()>(GetProcAddress(GetModuleHandleA("tier0.dll"), "Plat_FloatTime")) };
	return fn();
}

namespace g
{
	inline bool silentangles = false;
	inline bool psilentangles = false;

	inline bool updatinganims = false;
}